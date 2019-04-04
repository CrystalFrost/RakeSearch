// ����� ��� ������������ ��������� ��������� ������� "�����������" �����

# include "RakeSearch.h"

// ����������� �� ���������
RakeSearch::RakeSearch()
{
	Reset();
}


// ������� ���� ������ ���������� � ����������� �����
void RakeSearch::SetFileNames(string start, string result, string checkpoint, string temp)
{
	startParametersFileName = start;
	resultFileName = result;
	checkpointFileName = checkpoint;
	tempCheckpointFileName = temp;
}


// ����� �������� ���������� ��������
void RakeSearch::Reset()
{
	// ������� ������ ���������
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			squareA[rowId][columnId] = Square::Empty;
			squareB[rowId][columnId] = Square::Empty;
		}
	}

	// ����� �������� �������� ��������� ���������
		// ����� ��������, ��������������� �������� ������
		keyRowId = Square::Empty;
		keyColumnId = Square::Empty;
		keyValue = Square::Empty;

		// ����� ��������, ��������� � ���� ���������� ������
		for (int i = 0; i < MaxCellsInPath; i++)
		{
			path[i][0] = Square::Empty;
			path[i][1] = Square::Empty;
		}

		// ����� �������� � �������� ������������� ��������� �� ���������
		flagsPrimary = (1u << Rank) - 1;
		flagsSecondary = (1u << Rank) - 1;

		// ����� �������� � �������� ������������� ��������� � �������� � �������
		for (int i = 0; i < Rank; i++)
		{
			flagsColumns[i] = (1u << Rank) - 1;
			flagsRows[i] = (1u << Rank) - 1;
		}

		// ����� �������� � ���� ������� ������������� �������� � �������
		for (int rowId = 0; rowId < Rank; rowId++)
		{
			for (int columnId = 0; columnId < Rank; columnId++)
			{
				flagsCellsHistory[rowId][columnId] = (1u << Rank) - 1;
			}
		}

		// ����� ��������� �������������� ������
		rowId = Square::Empty;
		columnId = Square::Empty;

		// ����� �������� ������
		checkpointFileName.clear();
		tempCheckpointFileName.clear();
		resultFileName.clear();

		// ����� ����� ��������������� ���������
		squaresCount = 0;

	// ����� ��������, ��������� � ������������� �����
		// ����� ����� �������� ��� ��� ��������� ���
		pairsCount = 0;

		// ����� �������� ���������� ���������
		totalPairsCount = 0;
		totalSquaresWithPairs = 0;

		// ������� ��� ������� ������
		startParametersFileName = "start_parameters.txt";
		resultFileName = "result.txt";
		checkpointFileName = "checkpoint.txt";
		tempCheckpointFileName = "tmp_checkpoint.txt";

		// ������� ��������� - ��������� � ����� ���������� ��� ����������� �����
		workunitHeader = "# RakeSearch of diagonal Latin squares";

		// ����� ����� �������������
		isInitialized = 0;
}


// ������������� ������
void RakeSearch::Initialize(string start, string result, string checkpoint, string temp)
{
	fstream startFile;
	fstream checkpointFile;

	// ���������� �������� ���� ������
	startParametersFileName = start;
	resultFileName = result;
	checkpointFileName = checkpoint;
	tempCheckpointFileName = temp;

	// ��������� ��������� ���������� � ������ �� ����� ����������� ����� ��� ��������� ��������
		// �������� ������ �� ���������� ����������� � ����� ����������� �����
		startFile.open(startParametersFileName.c_str(), std::ios_base::in);
		checkpointFile.open(checkpointFileName.c_str(), std::ios_base::in);

		// ���������� ��������� �� ����� ����������� �����
		if (checkpointFile.is_open())
		{
			// ���������� ��������� �� ������������� ����� ����������� �����
			try
			{
				Read(checkpointFile);
				isStartFromCheckpoint = 1;
			}
			catch (...)
			{
				cerr << "Error opening checkpoint file! Starting with workunit start parameters." << endl;
				isStartFromCheckpoint = 0;
			}
		}

		// ���������� ��������� �� ����� ��������� ����������
		if (isStartFromCheckpoint != 1)
		{
			// ���������� ��������� �� ������������� ����� ��������� ����������
			Read(startFile);
			isStartFromCheckpoint = 0;
		}

	// �������� ������
	startFile.close();
	checkpointFile.close();
}


// ������ ��������� ������ �� ������
void RakeSearch::Read(istream& is)
{
	string marker;
	int rankToVerify;
	unsigned int storedBit = 0;
	Square currentSquare;

	// ����� ����� ��������������������
	isInitialized = 0;

	// ���������� ��������� ������
		// ������� ������ ������ ���������
		do
		{
			std::getline(is, marker);

			if (is.eof())
			{
				throw ("Expected start marker, but EOF found.");
			}
		}
		while (marker != workunitHeader);

		// ��������� ��������� ���������� ���
			// ���������� �� ������ ����� ��������
			is >> rankToVerify;

			// ���������� ������ ������ ������� ��� �����
			if (rankToVerify == Square::Rank)
			{
				// ���������� �� ������ �������� A - ������� �������� ����
				is >> currentSquare;
				for (int rowId = 0; rowId < Rank; rowId++)
				{
					for (int columnId = 0; columnId < Rank; columnId++)
					{
						squareA[rowId][columnId] = currentSquare.Matrix[rowId][columnId];
					}
				}

				// ���������� ����� ������ � ���� ������
				is >> cellsInPath;

				// ���������� �� ������ ���� ������ ������
				for (int i = 0; i < cellsInPath; i++)
				{
					is >> path[i][0];
					is >> path[i][1];
				}

				// ���������� �� ������ ���������� � �������� ������
				is >> keyRowId;
				is >> keyColumnId;
				is >> keyValue;

				// ���������� ���������� �� �������������� ������
				is >> rowId;
				is >> columnId;
				is >> cellId;

				// ���������� �� ������ ���������� � ��������������� ��������� � ������� ��������
					// ���������� ���������� � ��������� �� ������� ���������
					for (int i = 0; i < Rank; i++)
					{
						is >> storedBit;
						if (storedBit)
						{
							flagsPrimary |= 1u << i;
						}
						else
						{
							flagsPrimary &= ~(1u << i);
						}
					}

					// ���������� ���������� � ��������� �� �������� ���������
					for (int i = 0; i < Rank; i++)
					{
						is >> storedBit;
						if (storedBit)
						{
							flagsSecondary |= 1u << i;
						}
						else
						{
							flagsSecondary &= ~(1u << i);
						}
					}

					// ���������� ���������� � ��������� � �������
					for (int i = 0; i < Rank; i++)
					{
						for (int j = 0; j < Rank; j++)
						{
							is >> storedBit;
							if (storedBit)
							{
								flagsRows[i] |= 1u << j;
							}
							else
							{
								flagsRows[i] &= ~(1u << j);
							}
						}
					}

					// ���������� ���������� � ��������� � ��������
					for (int i = 0; i < Rank; i++)
					{
						for (int j = 0; j < Rank; j++)
						{
							is >> storedBit;
							if (storedBit)
							{
								flagsColumns[i] |= 1u << j;
							}
							else
							{
								flagsColumns[i] &= ~(1u << j);
							}
						}
					}

					// ���������� ���������� �� ������� �������� � ������� ��������
					for (int h = 0; h < Rank; h++)
					{
						for (int i = 0; i < Rank; i++)
						{
							for (int j = 0; j < Rank; j++)
							{
								is >> storedBit;
								if (storedBit)
								{
									flagsCellsHistory[i][j] |= 1u << h;
								}
								else
								{
									flagsCellsHistory[i][j] &= ~(1u << h);
								}
							}
						}
					}

					// ��������� ����� ��������������� ���������
					is >> squaresCount;

					// ���������� ���� ��������������������
					isInitialized = Yes;
			}

		// ��������� ���������� ������ ������������ (�� ����� - ���������� �� �����������)
		is >> pairsCount;
		is >> totalPairsCount;
		is >> totalSquaresWithPairs;

		// ����������� ����� ��������������������
		isInitialized = 1;
}


// ������ ��������� ������ � �����
void RakeSearch::Write(std::ostream& os)
{
	Square currentSquare(squareA);	// ������ ������� ����, �������������� � ������� ������

	// ������ ��������� ������
		// ������ ���������
		os << workunitHeader << endl;
		os << endl;

		// ������ ��������� ���������� ���
			// ������ � ����� ����� ��������
			os << Square::Rank << endl;

			// ������ � ����� ��������
			os << currentSquare;

			// ������ ����� ������ � ���� ������
			os << cellsInPath << endl;
			os << endl;

			// ������ � ����� ���� ������ ������
			for (int i = 0; i < cellsInPath; i++)
			{
				os << path[i][0] << " ";
				os << path[i][1] << " ";
				os << endl;
			}
			os << endl;

			// ������ � ����� ���������� � �������� ������
			os << keyRowId << " " << keyColumnId << " " << keyValue << endl;

			// ������ ���������� � ������� ������
			os << rowId << " " << columnId << " " << cellId << endl;

			// ���������� ������ ������ ��� ��������
			os << endl;

			// ������ ���������� � ��������������� ��������� � ������� ��������
				// ������ ���������� � ��������� �� ������� ���������
				for (int i = 0; i < Rank; i++)
				{
					os << ((flagsPrimary & (1u << i)) > 0 ? 1 : 0) << " ";
				}
				os << endl;

				// ������ ���������� � ��������� �� �������� ���������
				for (int i = 0; i < Rank; i++)
				{
					os << ((flagsSecondary & (1u << i)) > 0 ? 1 : 0) << " ";
				}
				os << endl;

				// �������������� ������ ������
				os << endl;

				// ������ ���������� � ��������� � �������
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						os << ((flagsRows[i] & (1u << j)) > 0 ? 1 : 0) << " ";
					}
					os << endl;
				}
				os << endl;

				// ������ ���������� � ��������� � ��������
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						os << ((flagsColumns[i] & (1u << j)) > 0 ? 1 : 0) << " ";
					}
					os << endl;
				}
				os << endl;

				// ������ ���������� �� ������� �������� � ������� ��������
				for (int h = 0; h < Rank; h++)
				{
					for (int i = 0; i < Rank; i++)
					{
						for (int j = 0; j < Rank; j++)
						{
							os << ((flagsCellsHistory[i][j] & (1u << h)) > 0 ? 1 : 0) << " ";
						}
						os << endl;
					}
					os << endl;
				}
				os << endl;

				// ������ � ����� ���������� � ����� ��������������� ���������
				os << squaresCount << endl;
				os << endl;

				// ������ �������������� �����������
				os << pairsCount << " " << totalPairsCount << " " << totalSquaresWithPairs << endl;
				os << endl;
}


// �������� ����������� �����
void RakeSearch::CreateCheckpoint()
{
	ofstream checkpointFile;

	checkpointFile.open(tempCheckpointFileName.c_str(), std::ios_base::out);
	if (checkpointFile.is_open())
	{
		Write(checkpointFile);
		checkpointFile.close();
		remove(checkpointFileName.c_str());
		rename(tempCheckpointFileName.c_str(), checkpointFileName.c_str());
	}
	else
	{
		cerr << "Error opening checkpoint file!" << endl;
	}
}


// ��������� ����������, �������� ��� �������������� ��������
void RakeSearch::ProcessOrthoSquare()
{
	ofstream resultFile;			// ����� ��� I/O � ���� � ������������

	Square a(squareA);				// ������� A ��� ������
	Square b(squareB);				// ������� B ��� ������

	int orthoMetric = Rank * Rank;	// �������� ������� ���������������, ��������� � ���, ��� �������� - ��������� ������������

	// ��������� ���������� ��������
	if (Square::OrthoDegree(a, b) == orthoMetric && b.IsDiagonal() && b.IsLatin() && a.IsDiagonal() && a.IsLatin())
	{
		// ������ ���������� � ��������� ��������
			// ���������� �������� ���������
			pairsCount++;
			totalPairsCount++;

			// ����������� �������� ��������
			if (pairsCount == 1)
			{
				orthoSquares[pairsCount - 1] = a;
				totalSquaresWithPairs++;
			}

			// ����������� �������� - ����
			if (pairsCount < OrhoSquaresCacheSize)
			{
				orthoSquares[pairsCount] = b;
			}

			// ����� ���������
			if (pairsCount == 1)
			{
				if (isDebug)
				{
					// ����� ���������� � ������ �������� ���� � ���� ���������
					cout << "{" << endl;
					cout << "# ------------------------" << endl;
					cout << "# Detected pair for the square: " << endl;
					cout << "# ------------------------" << endl;
					cout << a;
					cout << "# ------------------------" << endl;
				}
				// ����� ���������� � ����
				resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
				if (resultFile.is_open())
				{
					resultFile << "{" << endl;
					resultFile << "# ------------------------" << endl;
					resultFile << "# Detected pair for the square: " << endl;
					resultFile << "# ------------------------" << endl;
					resultFile << a;
					resultFile << "# ------------------------" << endl;
					resultFile.close();
				}
				else
				{
					std::cerr << "Error opening file!";
				}
			}

			// ����� ���������� � ��������� ����
			if (isDebug)
			{
				// ����� ���������� � �������
				cout << b << endl;
			}

			// ����� ���������� � ����
			resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
			if (resultFile.is_open())
			{
				resultFile << b << endl;
				resultFile.close();
			}
			else
			{
				std::cerr << "Error opening file!";
			}
	}
}


// �������� �������� ��������������� ������ ���������, ���������� � ������� ������
void RakeSearch::CheckMutualOrthogonality()
{
	int orthoMetric = Rank * Rank;
	int maxSquareId;
	ofstream resultFile;

	// ����������� ������� ������� �������������� ���������
	if (pairsCount < OrhoSquaresCacheSize)
	{
		maxSquareId = pairsCount;
	}
	else
	{
		maxSquareId = OrhoSquaresCacheSize - 1;
	}

	// ��������� ���� � ������������
	resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
	if (!resultFile.is_open()) { cout << "Error opening file!"; return; }

	// �������� �������� ��������������� ������ ���������
	for (int i = 0; i <= maxSquareId; i++)
	{
		for (int j = i + 1; j <= maxSquareId; j++)
		{
			if (Square::OrthoDegree(orthoSquares[i], orthoSquares[j]) == orthoMetric)
			{
				if (isDebug) cout << "# Square " << i << " # " << j << endl;
				resultFile << "# Square " << i << " # " << j << endl;
			}
		}
	}
	if (isDebug) cout << endl;
	resultFile << endl;

	// ������� ����� ����� �������� ����
	if (isDebug) cout << "# Pairs found: " << pairsCount << endl;
	resultFile << "# Pairs found: " << pairsCount << endl;

	// ������ ������� �� ��������� ������ �����������
	if (isDebug) cout << "}" << endl;
	resultFile << "}" << endl;

	// ��������� ���� � ������������
	resultFile.close();
}


// ��������� ��������
void RakeSearch::ProcessSquare()
{
	double fraction_done;	// ���� ���������� �������

	// ����������� ������� ��������� ���������
	squaresCount++;

	// ������ ����������� �����
	PermuteRows();

	// �������� �������� ��������������� ���������
	if (pairsCount > 0)
	{
		CheckMutualOrthogonality();
	}

	// �������� ���������� � ���� ���������
	if (squaresCount % CheckpointInterval == 0)
	{
		// �������� �������� ���������� ��� ������� BOINC
		fraction_done = squaresCount / 1000000000;

		if (fraction_done >= 1) fraction_done = 0.999999999;

		boinc_fraction_done(fraction_done); // �������� ������� BOINC � ���� ���������� �������

		// ��������, ����� �� ������ BOINC ������� ����������� �����,
		// � ���� �����, �� ��������� ������� � ������
		if (boinc_time_to_checkpoint())
		{
			CreateCheckpoint();
			boinc_checkpoint_completed(); // BOINC �����, ��� ����������� ����� ��������
		}

		if (isDebug)
		{
			Square squareToShow(squareA);
			
			cout << "# ------------------------" << endl;
			cout << "# Processed " << squaresCount << " squares." << endl;
			cout << "# Last processed square:" << endl;
			cout << endl;
			cout << squareToShow;
			cout << "# ------------------------" << endl;
		}
	}
}


// ����� ������ ������
void RakeSearch::ShowSearchTotals()
{
	ofstream resultFile;

	if (isDebug)
	{
		// ����� ������ � �������
		cout << "# ------------------------" << endl;
		cout << "# Total pairs found: " << totalPairsCount << endl;
		cout << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
		cout << "# ------------------------" << endl;
	}

	// ����� ������ � ����
	resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
	if (resultFile.is_open())
	{
		resultFile << "# ------------------------" << endl;
		resultFile << "# Total pairs found: " << totalPairsCount << endl;
		resultFile << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
		resultFile << "# Processed " << squaresCount << " squares" << endl;
		resultFile << "# ------------------------" << endl;
		resultFile.close();
	}
	else cerr << "Error opening file!" << endl;
}


// ������ ��������� ���������
void RakeSearch::Start()
{
	unsigned long bitIndex;      // ������� ������������� ���� � "�������" ������
	unsigned int freeValuesMask; // �������� ����� �� ����� - ������ ��������� ��������
	unsigned int isGet;          // ���� ��������� ������ �������� ��� ������
	int cellValue;               // ����� �������� ��� ������
	int oldCellValue;            // ������ ��������, �������� � ������

	int stop = 0;                // ���� ���������� ��������� �������

	if (isInitialized == Yes)
	{
		// ������ �������� ������ ��������
		do
		{
			// ������ �������� ��� ��������� ������ ��������
				// ��������� ���������� ������
				rowId = path[cellId][0];
				columnId = path[cellId][1];

				// ���������� ����� �������� ��� ������ (rowId, columnId)
					// ���������� �������� ����������
					isGet = 0;
					cellValue = Square::Empty;
					freeValuesMask = (1u << Rank) - 1;

					// ��������� �������� ��� ������
						// ��� ��� ��� 9 ����� � ������ ����� workunit-� ��� ������ ��� ���������, �� �������� �� ��� - ���������
						// // ��������� ����� ������� ���������
						// if (rowId == columnId)
						// {
						// 	freeValuesMask &= flagsPrimary;
						// }
						// 
						// // ��������� ����� �������� ���������
						// if (Rank - rowId - 1 == columnId)
						// {
						// 	freeValuesMask &= flagsSecondary;
						// }

						// ��������� ����� �����, �������� � ������� ��������
						freeValuesMask &= flagsColumns[columnId] & flagsRows[rowId] & flagsCellsHistory[rowId][columnId];

						// ���������� ���������� ��������� ��� ������������� � ������ ��������
						# ifdef _MSC_VER
						isGet = _BitScanForward(&bitIndex, freeValuesMask);
						cellValue = bitIndex;
						# else
						bitIndex = __builtin_ffs(freeValuesMask);
						if (bitIndex > 0)
						{
							isGet = 1;
							cellValue = bitIndex - 1;
						}
						else
						{
							isGet = 0;
						}
						# endif

					// ��������� ���������� ������
					if (isGet)
					{
						// ��������� ���������� ������ ��������
							// ���������� �������� ��������
							oldCellValue = squareA[rowId][columnId];
							// ������ ������ ��������
								// ���������� �������� � �������
								squareA[rowId][columnId] = cellValue;
								// �������� �������� � ��������
								flagsColumns[columnId] ^= 1u << cellValue;
								// �������� �������� � �������
								flagsRows[rowId] ^= 1u << cellValue;
								// ��� ��� ��� 9 ����� � ������ ����� workunit-� ��� ������ ��� ���������, �� �������� �� ��� - ���������
								// // �������� �������� � ����������
								// if (rowId == columnId)
								// {
								// 	flagsPrimary ^= 1u << cellValue;
								// }
								// if (rowId == Rank - 1 - columnId)
								// {
								// 	flagsSecondary ^= 1u << cellValue;
								// }
								// �������� �������� � ������� �������� ������
								flagsCellsHistory[rowId][columnId] ^= 1u << cellValue;

							// ����������� ����������� �������� ��� �������� ������� (��� ��� �� �������� � ���� �������)
							if (oldCellValue != Square::Empty)
							{
								// ���������� �������� � �������
								flagsColumns[columnId] |= 1u << oldCellValue;
								// ���������� �������� � ������
								flagsRows[rowId] |= 1u << oldCellValue;
								// ��� ��� ��� 9 ����� � ������ ����� workunit-� ��� ������ ��� ���������, �� �������� �� ��� - ���������
								// // ���������� �������� � ���������
								// if (rowId == columnId)
								// {
								// 	flagsPrimary |= 1u << oldCellValue;
								// }
								// if (rowId == Rank - 1 - columnId)
								// {
								// 	flagsSecondary |= 1u << oldCellValue;
								// }
							}

							// ��������� ��������� ������������ ��������
							if (cellId == cellsInPath - 1)
							{
								// ������������ ��������� �������
								ProcessSquare();
							}
							else
							{
								// ������ ��� �����
								cellId++;
							}
					}
					else
					{
						// ��������� ����� ������������ ������ �������� � ������ (rowId; columnId)
							// ���������� ������� �������� �� �������� � �������
								// ��������� ������� ��������
								cellValue = squareA[rowId][columnId];
								// ���������� �������� � ��������� �������
								if (cellValue != Square::Empty)
								{
									// ���������� �������� � �������
									flagsColumns[columnId] |= 1u << cellValue;
									// ���������� �������� � ������
									flagsRows[rowId] |= 1u << cellValue;
									// ��� ��� ��� 9 ����� � ������ ����� workunit-� ��� ������ ��� ���������, �� �������� �� ��� - ���������
									// // ���������� �������� � ���������
									// if (rowId == columnId)
									// {
									// 	flagsPrimary |= 1u << cellValue;
									// }
									// if (rowId == Rank - 1 - columnId)
									// {
									// 	flagsSecondary |= 1u << cellValue;
									// }
									// ���������� ������ ��������
									squareA[rowId][columnId] = Square::Empty;
									// �������� ������� ������ (rowId, columnId)
									flagsCellsHistory[rowId][columnId] = (1u << Rank) - 1;
								}

								// ������ ��� �����
								cellId--;
					}

					// ��������� ������� ��������� ������
					if (squareA[keyRowId][keyColumnId] == keyValue)
					{
						if (keyValue == Square::Empty)
						{
							if (cellId < 0)
							{
								stop = Yes;
							}
						}
						else
						{
							stop = Yes;
						}
					}
		}
		while (!stop);
	}

	// ����� ������ ������
	ShowSearchTotals();
}


// ����������� ����� ��������� ��� � ������ ���� � ����
void RakeSearch::PermuteRows()
{
	int currentRowId = 1;				// ����� �������������� ������ � ����������� ��������
	int isRowGet = 0;					// ���� ���������� ������ ��� �������� � ������ ����������
	int oldRowId = -1;					// ����� ����������� ������� �������� ������ ������ ������������ ��������, ���������� ����� ���������
	int isDifferent = 0;				// ���� ������� ���������� �������� �� ��������� (������ ���������� ���������� ����������� � �������� ���������)

	unsigned long insertedRowId = 0;	// ����� ����� ������ ��� �������� ������������� � �����������
	unsigned int isRowFree = 0;			// ���� ������� "���������" ��� �������� � ���������� ������, ������� ����� ����� ��������� �� ��������������
	unsigned int freeRowsMask = 0;		// ����� �� ���������� ������ �������������� ����� � ������� �������������� ����� ��������� �������� ��� ������� ������ ����� ����������

	// ������������� ���������� �������� ���� ����, ���������� ��� ���
	int currentSquareRows[Rank] = { 0, -1, -1, -1, -1, -1, -1, -1, -1 };					// ������ � �������� ������� �����, �������������� � ��������. �� i-� ������� - ����� ������������ ������
	unsigned int primaryDiagonalFlags = 510;												// ������� ����� ��������� �������� �� �������� ��������� 
	unsigned int secondaryDiagonalFlags = 255;												// ������� ����� ��������� �������� �� �������� ��������� 
	unsigned int rowsUsageFlags = 510;														// ������� ����� �������������� ����� � ������� ����������
	unsigned int rowsHistoryFlags[Rank] = { 510, 509, 507, 503, 495, 479, 447, 383, 255 };	// ������� ����� ������� �������������� ����� � ������ ������� ����������, ��� i-� �������� - ��� ����� ������������� ����� ������ i-� ������ �������� B
	// ������ ������! � ������ ������� ������ ������� ��������������� �����, ��� ������� ������, ����������� ����������, ���������� � ��������� ��������

	// ���� ����, ��������� �������������� ����������
	//	// ����� �������� � �������� ������������� ����� � ��������� ������������ � ������� �����, �������������� ��� �������� ��������
	//	for (int i = 0; i < Rank; i++)
	//	{
	//		currentSquareRows[i] = -1;
	//	}
	//
	//	// ����������� ������ ����������� ������������� �������� �� ����������
	//	primaryDiagonalFlags = (1u << Rank) - 1;
	//	secondaryDiagonalFlags = (1u << Rank) - 1;
	//	// ����� ������ ������������ ����� � ����� ����������
	//	rowsUsageFlags = (1u << Rank) - 1;
	//	// ����� ������� �������������� �����
	//	for (int i = 0; i < Rank; i++)
	//	{
	//		rowsHistoryFlags[i] = (1u << Rank) - 1;
	//		rowsHistoryFlags[i] ^= 1u << i;
	//	}
	//	
	//	// ���������� ����� �������������� ������ (�������������) ������ � � �������� �� ����������
	//	rowsUsageFlags ^= 1u;
	//	rowsHistoryFlags[0] ^= 1u;
	//	currentSquareRows[0] = 0;
	//	primaryDiagonalFlags ^= 1u << squareA[0][0];
	//	secondaryDiagonalFlags ^= 1u << squareA[0][Rank - 1];


	// ������� ��������� ���������� ����� ��������� ��� � ����� ������ ���� � ����
		// ����� ����� �������� ��� ��� ��������� ���
		pairsCount = 0;

		// ������������� ���������� (����������� ���� ����)

		// ������ ���� ��������� ���������� �� ����� ��������� ��������
		while (currentRowId > 0)
		{
			// ��������� ��� � ���������� ����� ����������
				// ������ ������ �� ��������� �������� �� ������� currentRowId ����������� ��������
				isRowGet = 0;

				// ����� ������, �� ��������������� ������ � ����� - ��� ���� �� ������
				do
				{
					// �������� ��������� ������ �� ����������� ���������� � ����� �����������
						// ��������� ����� ��������� �����
						freeRowsMask = rowsUsageFlags & rowsHistoryFlags[currentRowId];
						// ��������� ������ ��������� ������
						# ifdef _MSC_VER
						isRowFree = _BitScanForward(&insertedRowId, freeRowsMask);
						# else
						insertedRowId = __builtin_ffs(freeRowsMask);
						if (insertedRowId > 0)
						{
							isRowFree = 1;
							insertedRowId--;
						}
						else
						{
							isRowFree = 0;
						}
						# endif

						// ��������� ��������� ������
						if (isRowFree)
						{
							// �������� ������������ ��������� ������ �� ���������� � ��� ���������� � ���������� ��������
							if ((primaryDiagonalFlags & (1u << squareA[insertedRowId][currentRowId])) && (secondaryDiagonalFlags & (1u << squareA[insertedRowId][Rank - currentRowId - 1])))
							{
								isRowGet = 1;
							}

							// ��������� ������ � ������� bitIndex ��� ��� �����������
							rowsHistoryFlags[currentRowId] ^= 1u << insertedRowId;
						}
				}
				while (isRowFree && !isRowGet);

				// ��������� ��������� ������ 
				if (isRowGet)
				{
					// ���������� ��������� ������ � ���������� ����� ������������ ��������
						// ���������� ������ ������, ��� �������������� �� ���� �������
						oldRowId = currentSquareRows[currentRowId];

						// ��������� ������������� ����� ������ � �������� ������
							// ����� ����� � ������� ���������/�������������� �����
							rowsUsageFlags ^= 1u << insertedRowId;
							// ������ ������ ��������� ������ � �������� ����� ����� ����������
							currentSquareRows[currentRowId] = insertedRowId;
							// ��������� ������������ �������� �� ����� ������ ��� ��������������
							primaryDiagonalFlags ^= 1u << squareA[insertedRowId][currentRowId];
							secondaryDiagonalFlags ^= 1u << squareA[insertedRowId][Rank - currentRowId - 1];

							// �������� ����� ��������� ������, ��������� �� ����� ���������� � �������� � ������������ ������
							if (oldRowId != -1)
							{
								rowsUsageFlags |= 1u << oldRowId;
								primaryDiagonalFlags |= 1u << squareA[oldRowId][currentRowId];
								secondaryDiagonalFlags |= 1u << squareA[oldRowId][Rank - currentRowId - 1];
							}

							// ��������� ���������� ����������
							if (currentRowId == Rank - 1)
							{
								// ��������� ����������� ����������
									// ������������ �������� �� ��������� ����������
									for (int i = 0; i < Rank; i++)
									{
										for (int j = 0; j < Rank; j++)
										{
											squareB[i][j] = squareA[currentSquareRows[i]][j];
										}
									}

									// ��������� ����������� ��������
									ProcessOrthoSquare();
							}
							else
							{
								// ��� ����� - � ��������� ����� ������������ ��������
								currentRowId++;
							}
				}
				else
				{
					// ��� ����� � ������
						// ���������� ������ ������ ��������� �������� ��������������, ��� ����� �� ������� ������ ����������� ����������
						oldRowId = currentSquareRows[currentRowId];
						// ��������� ������� ������ ����� ���������� ��� �����������������
						currentSquareRows[currentRowId] = -1;
						// �������� ������ �������������� ������ �� ��������� �������� � �������������� � ������������ �������� � ����� ����������
						if (oldRowId != -1)
						{
							rowsUsageFlags |= 1u << oldRowId;
							primaryDiagonalFlags |= 1u << squareA[oldRowId][currentRowId];
							secondaryDiagonalFlags |= 1u << squareA[oldRowId][Rank - currentRowId - 1];
						}
						// ������� ������� ���������� ������ ����������
						rowsHistoryFlags[currentRowId] = (1u << Rank) - 1;
						// ������� � ���������� ������ ����������� ����������
						currentRowId--;
				}
		}
}
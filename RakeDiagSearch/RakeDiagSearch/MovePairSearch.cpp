// ����� ��� ������������ ��������� ��������� ������� ����������� �����

# include "MovePairSearch.h"

// ����������� �� ���������
MovePairSearch::MovePairSearch()
{
	Reset();
}


// ����� �������� ������
void MovePairSearch::Reset()
{
	// ����� �������� ���������� ���
	squareAGenerator.Reset();

	// ����� �������� ���������� ������
	ClearBeforeNextSearch();

	// ����� �������� ���������� ���������
	totalPairsCount = 0;
	totalSquaresWithPairs = 0;
	totalProcessedSquaresSmall = 0;
	totalProcessedSquaresLarge = 0;

	startParametersFileName = "start_parameters.txt";
	resultFileName = "result.txt";
	checkpointFileName = "checkpoint.txt";
	tempCheckpointFileName = "checkpoint_new.txt";

	// ������� ��������� - ��������� � ����� ���������� ��� ����������� �����
	moveSearchGlobalHeader = "# Move search of pairs OLDS status";
	moveSearchComponentHeader = "# Move search component status";

	// ����� ����� �������������
	isInitialized = 0;
}


// ������� ����������� ���������� ����� ��������� �������
void MovePairSearch::ClearBeforeNextSearch()
{
	// ����� �������� ������ ��������� A � B, � ����� ������� ������������� ����� ��� ������������ �������� B
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			squareA[i][j] = -1;
			squareB[i][j] = -1;
			rowsHistory[i][j] = 1;
		}
	}

	// ����� �������� � ������� ������������� ����� � ��������� ������������ � ������� �����, �������������� ��� �������� ��������
	for (int i = 0; i < Rank; i++)
	{
		rowsUsage[i] = 1;
		currentSquareRows[i] = -1;
	}

	// ����� �������� ��������� ��� ��������� ��� ��������� ���
	pairsCount = 0;
}


// ������������� ������
void MovePairSearch::InitializeMoveSearch(string start, string result, string checkpoint, string temp)
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
		startFile.open(startParametersFileName, std::ios_base::in);
		checkpointFile.open(checkpointFileName, std::ios_base::in);

		// ����������� ���������
		if (checkpointFile.is_open())
		{
			// ���������� ��������� �� ����� ����������� �����
			Read(checkpointFile);
			isStartFromCheckpoint = 1;
		}
		else
		{
			// ���������� ��������� �� ����� ��������� ����������
			Read(startFile);
			isStartFromCheckpoint = 0;
		}

		// �������� ������
		startFile.close();
		checkpointFile.close();
}


// ������ ��������� ������ �� ������
void MovePairSearch::Read(istream& is)
{
	string marker;

	// ����� ����� ��������������������
	isInitialized = 0;

	// ���������� ��������� ������
		// ������� ������ ������ ���������
		do
		{
			std::getline(is, marker);
		}
		while (marker != moveSearchGlobalHeader);
		
		// ��������� ��������� ���������� ���
		is >> squareAGenerator;

		// ������� ������ ���������� �����������
		do
		{
			std::getline(is, marker);
		}
		while (marker != moveSearchComponentHeader);

		// ��������� ���������� ������ ������������ (�� ����� - ���������� �� �����������)
		is >> pairsCount;
		is >> totalPairsCount;
		is >> totalSquaresWithPairs;
		is >> totalProcessedSquaresLarge;
		is >> totalProcessedSquaresSmall;

	// ����������� ����� ��������������������
	isInitialized = 1;
}


// ������ ��������� ������ � �����
void MovePairSearch::Write(ostream& os)
{
	// ������ ��������� ������
		// ������ ���������
		os << moveSearchGlobalHeader << endl;
		os << endl;

		// ������ ��������� ���������� ���
		os << squareAGenerator;
		os << endl;

		// ������ ��������� ����� �����������
		os << moveSearchComponentHeader << endl;
		os << endl;

		// ������ �������������� �����������
		os << pairsCount << " " << totalPairsCount << " " << totalSquaresWithPairs << endl;
		os << totalProcessedSquaresLarge << " " << totalProcessedSquaresSmall << endl;
		os << endl;
}


// �������� ����������� �����
void MovePairSearch::CreateCheckpoint()
{
	fstream checkpointFile;

	checkpointFile.open(tempCheckpointFileName, std::ios_base::out);
	if (checkpointFile.is_open())
	{
		Write(checkpointFile);
		checkpointFile.close();
		remove(checkpointFileName.c_str());
		rename(tempCheckpointFileName.c_str(), checkpointFileName.c_str());
	}
}


// ������ ������ ������������� ��������� ������� ������������ �����
void MovePairSearch::StartMoveSearch()
{
	// ������������� �� ������� ���������� ���������� ���
	__hook(&Generator::SquareGenerated, &squareAGenerator, &MovePairSearch::OnSquareGenerated);

	// ��������� ��������� ���
	squareAGenerator.Start();

	// ������������ �� ������� ���������� ���������� ���
	__unhook(&Generator::SquareGenerated, &squareAGenerator, &MovePairSearch::OnSquareGenerated);

	// ����� ������ ������
	ShowSearchTotals();
}


// ���������� ������� ���������� ���, ����������� ����� � ���� ����
void MovePairSearch::OnSquareGenerated(Square newSquare)
{
	// ������� ����� ������� ���������, �������������� ���������� ���
	ClearBeforeNextSearch();

	// ����������� ���������� ��������
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			squareA[i][j] = newSquare.Matrix[i][j];
		}
	}

	// ������ ����������� �����
	MoveRows();

	// �������� �������� ��������������� ���������
	if (pairsCount > 0)
	{
		CheckMutualOrthogonality();
	}

	// ��������� ���������� �� ������������ ���������
	totalProcessedSquaresSmall++;
	if (totalProcessedSquaresSmall > 0 && totalProcessedSquaresSmall % 1000000000 == 0)
	{
		totalProcessedSquaresLarge++;
		totalProcessedSquaresSmall = 0;
	}

	// �������� ���������� � ���� ���������
	if (totalProcessedSquaresSmall % CheckpointInterval == 0)
	{
		CreateCheckpoint();

		cout << "# ------------------------" << endl;
		cout << "# Processed " << totalProcessedSquaresLarge << " milliards and " << totalProcessedSquaresSmall << " squares." << endl;
		cout << "# Last processed square:" << endl;
		cout << endl;
		cout << newSquare;
		cout << "# ------------------------" << endl;
	}
}


// ����������� ����� ��������� ��� � ������ ���� � ����
void MovePairSearch::MoveRows()
{
	int currentRowId = 1;
	int isRowGet = 0;
	int gettingRowId = -1;
	int oldRowId = -1;

	int diagonalValues[Rank];
	int duplicationDetected = 0;

	// ���������� ������ ������ �������� A � B � ����� ������ ��������������� ���������
	for (int j = 0; j < Rank; j++)
	{
		squareB[0][j] = squareA[0][j];
	}

	// �������� �������������� ������ ������, �.�. ��� �������������
	rowsUsage[0] = 0;
	rowsHistory[0][0] = 0;
	currentSquareRows[0] = 0;

	while (currentRowId > 0)
	{
		// ��������� ������ �� ��������� �������� �� ������� currentRowId ������������ ��������
		isRowGet = 0;
		gettingRowId = -1;
		for (int i = 0; i < Rank; i++)
		{
			// ��������� i-� ������ ��������� ��������
			if (rowsUsage[i] && rowsHistory[currentRowId][i])
			{
				isRowGet = 1;
				gettingRowId = i;

				break;
			}
		}

		// ������������ ��������� ������
		if (isRowGet)
		{
			// ������������ ���������� ����� ������
				// ������� � ������� ����� ������
					// ��������� ����� ������, ������� ������ ����� � ��������
					oldRowId = currentSquareRows[currentRowId];
					// ���������� ����� ������ � �������, ������ ������ �������������� �����, � ������� �������������� ����� � ������ ������� ����� ��������
						// ���������� ����� ������ � �������
						for (int j = 0; j < Rank; j++)
						{
							squareB[currentRowId][j] = squareA[gettingRowId][j];
						}
						// �������� ������ � ������ ������������ �����
						rowsUsage[gettingRowId] = 0;
						// �������� ������ � ������� ������������� ������
						rowsHistory[currentRowId][gettingRowId] = 0;
						// ���������� ������ � ������ ������� ����� ��������
						currentSquareRows[currentRowId] = gettingRowId;

				// ������� ��� ���������� ������ ����� �������������
					// ������� ������� � ������� ������������ �����
					if (oldRowId != -1)
					{
						rowsUsage[oldRowId] = 1;
					}

				// ��������� �������������� ������������ ����� ��������
					// ���������� ���� ��������������� � ���������� �� ����������
					duplicationDetected = 0;
					// �������� ������� ���������
						// ���������� ����� �������������� ��������
						for (int i = 0; i < Rank; i++)
						{
							diagonalValues[i] = 1;
						}
						// �������� �������� ������� ���������
						for (int i = 0; i <= currentRowId; i++)
						{
							// �������� i-�� �������� ������� ��������� - ������ (i, i)
							if (diagonalValues[squareB[i][i]])
							{
								diagonalValues[squareB[i][i]] = 0;
							}
							else
							{
								duplicationDetected = 1;
								break;
							}
						}
					// �������� �������� ���������, ���� ��� ����� �����
					if (!duplicationDetected)
					{
						// �������� �������� ���������
							// ���������� ����� �������������� ��������
							for (int i = 0; i < Rank; i++)
							{
								diagonalValues[i] = 1;
							}
							// �������� �������� �������� ��������� ������� � "� ������"
							for (int i = 0; i <= currentRowId; i++)
							{
								// �������� i-�� �������� �������� ��������� - �������� (i, rank - 1 - i)
								if (diagonalValues[squareB[i][Rank - 1 - i]])
								{
									diagonalValues[squareB[i][Rank - 1 - i]] = 0;
								}
								else
								{
									duplicationDetected = 1;
									break;
								}
							}
					}

				// ��������� ������ �������� �������������� ��������
				if (!duplicationDetected)
				{
					// ������ ��������� ��� ����� � ����������� �� �������� ���������
					if (currentRowId == Rank - 1)
					{
						// ������������ ��������� �������
						ProcessOrthoSquare();
					}
					else
					{
						// ������ ��� �����
						currentRowId++;
					}
				}
		}
		else
		{
			// ������������ ������������ ����� ������ - ������ ��� �����, ������� ����� ��������������, 
			// ������� �������������, �������� ������� ����� �������� � ������� ��� �������
				// ��������� ����� ������� ������
				oldRowId = currentSquareRows[currentRowId];
				// �������� ������� ������ � ��������
				for (int j = 0; j < Rank; j++)
				{
					squareB[currentRowId][j] = -1;
				}
				// �������� ������� ������ ��������
				currentSquareRows[currentRowId] = -1;
				// �������� ���� ���������� ��������������
				rowsUsage[oldRowId] = 1;
				// �������� ������� ������ � ���� �������
				for (int i = 0; i < Rank; i++)
				{
					rowsHistory[currentRowId][i] = 1;
				}
				// ������ ��� �����
				currentRowId--;
		}
	}
}


// ��������� ���������� �������������� ��������
void MovePairSearch::ProcessOrthoSquare()
{
	int isDifferent = 0;			// ����� ������� � ������� �� ��������� �������� (��� ������ ������������ ��� �����)
	fstream resultFile;				// ����� ��� I/O � ���� � ������������

	Square a(squareA);				// ������� A ��� ������
	Square b(squareB);				// ������� B ��� ������

	int orthoMetric = Rank*Rank;	// �������� ������� ���������������, ��������� � ���, ��� �������� - ��������� ������������

	// ��������� ��� �� ��, ��� �� ����� ���������
	isDifferent = 0;
	
	for (int i = 0; i < Rank; i++)
	{
		if (currentSquareRows[i] != i)
		{
			isDifferent = 1;
			break;
		}
	}

	// ��������� ���������� ��������
	if (isDifferent && Square::OrthoDegree(a, b) == orthoMetric && b.IsDiagonal() && b.IsLatin() && a.IsDiagonal() && b.IsLatin())
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
				// ����� ���������� � ������ �������� ���� � ���� ���������
				cout << "{" << endl;
				cout << "# ------------------------" << endl;
				cout << "# Detected pair for the square: " << endl;
				cout << "# ------------------------" << endl;
				cout << a;
				cout << "# ------------------------" << endl;

				// ����� ���������� � ����
				resultFile.open(resultFileName, std::ios_base::app);
				resultFile << "{" << endl;
				resultFile << "# ------------------------" << endl;
				resultFile << "# Detected pair for the square: " << endl;
				resultFile << "# ------------------------" << endl;
				resultFile << a;
				resultFile << "# ------------------------" << endl;
				resultFile.close();
			}

			// ����� ���������� � ��������� ����
				// ����� ���������� � �������
				cout << b << endl;

				// ����� ���������� � ����
				resultFile.open(resultFileName, std::ios_base::app);
				resultFile << b << endl;
				resultFile.close();
	}
}


// �������� �������� ��������������� ������ ���������, ���������� � ������� ������
void MovePairSearch::CheckMutualOrthogonality()
{
	int orthoMetric = Rank*Rank;
	int maxSquareId;
	fstream resultFile;

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
	resultFile.open(resultFileName, std::ios_base::app);

	// �������� �������� ��������������� ������ ���������
	for (int i = 0; i <= maxSquareId; i++)
	{
		for (int j = i + 1; j <= maxSquareId; j++)
		{
			if (Square::OrthoDegree(orthoSquares[i], orthoSquares[j]) == orthoMetric)
			{
				cout << "# Square " << i << " # " << j << endl;
				resultFile << "# Square " << i << " # " << j << endl;
			}
		}
	}
	cout << endl;
	resultFile << endl;

	// ������� ����� ����� �������� ����
	cout << "# Pairs found: " << pairsCount << endl;
	resultFile << "# Pairs found: " << pairsCount << endl;

	// ������ ������� �� ��������� ������ �����������
	cout << "}" << endl;
	resultFile << "}" << endl;

	// ��������� ���� � ������������
	resultFile.close();
}


void MovePairSearch::ShowSearchTotals()
{
	fstream resultFile;

	// ����� ������ � �������
	cout << "# ------------------------" << endl;
	cout << "# Total pairs found: " << totalPairsCount << endl;
	cout << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
	cout << "# ------------------------" << endl;

	// ����� ������ � ����
	resultFile.open(resultFileName, std::ios_base::app);
	resultFile << "# ------------------------" << endl;
	resultFile << "# Total pairs found: " << totalPairsCount << endl;
	resultFile << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
	resultFile << "# Processes " << totalProcessedSquaresLarge << " milliards " << totalProcessedSquaresSmall << " squares" << endl;
	resultFile << "# ------------------------" << endl;
	resultFile.close();
}
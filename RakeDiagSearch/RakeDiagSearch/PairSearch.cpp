// ����� ��� ������������� ������������ ��������� ���������

# include "PairSearch.h"
# include <string>

using namespace std;

// ����������� �� ���������
PairSearch::PairSearch()
{
	// ����� ���� ��������
	Reset();

	// ������� ��������� ��������
	pairStateHeader = "# Search of pairs ODLS status";
}

// ����� ���� �������� ���������� ��������
void PairSearch::Reset()
{
	// ����� ���������� ������� �������� � ���������� �������� - ����
	squareAGenerator.Reset();
	Generator::Reset();

	// ������� ������� ��� ��������
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			pairsDictionary[i][j] = Free;
		}
	}

	// ����� ����� ��������� ��� � ������ ����� ��������� � ������
	pairsCount = 0;
	totalPairsCount = 0;
	totalSquaresWithPairs = 0;
	
	// ����� ����� ��������������������
	isInitialized = No;
}


// ������������� ������
void PairSearch::Initialize(string start, string result, string checkpoint, string temp)
{
	fstream startFile;
	fstream checkpointFile;
	string marker;

	// ���������� ���� �������������������� � "���"
	isInitialized = No;

	// ���������� �������� ���������� ��������
	Reset();

	// ���������� ����� ������ ������������, ����������� ����� � �����������
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
			isStartFromCheckpoint = Yes;
		}
		else
		{
			// ���������� ��������� �� ����� ��������� ����������
			Read(startFile);
			isStartFromCheckpoint = No;
		}

		// �������� ������
		startFile.close();
		checkpointFile.close();

		// ���������� ��������� ���������� ��� ����������� ������ ������������� ��������� � ���������� ��������
		startFile.open(startParametersFileName, std::ios_base::in);

		if (startFile.is_open())
		{
			// ��������� ����������
				// ����� ���������
				do
				{
					std::getline(startFile, marker);
				}
				while (marker != pairStateHeader);

				// ���������� ����������
				startFile >> initialState;
				initialState.SetFileNames(start, result, checkpoint, temp);
		}

		startFile.close();
}


// ���������� ��������� ������ �� ������
void PairSearch::Read(std::istream& is)
{
	int result = Yes;

	// ��������� ��������� ���������� ������������ ��������� ���������
	is >> squareAGenerator;

	// ��������� ��������� �������� ������ ������
	Generator::Read(is);

	// ��������� �������������� ���������� ������ ������ ������������� ���������
	if (isInitialized)
	{
		is >> pairsCount;
		is >> totalPairsCount;
		is >> totalSquaresWithPairs;
		
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				is >> pairsDictionary[i][j];
			}
		}
	}

	// ��������� �������, � �������� ���� ������������� ��������
	is >> squareA;
}


// ������ ��������� ������ � �����
void PairSearch::Write(std::ostream& os)
{
	// ������ ��������� ����������
	os << squareAGenerator << endl;

	// ������ ������ � ����������
	os << pairStateHeader << endl << endl;

	// ������ ��������� �������� ������ ������
	Generator::Write(os);
	os << endl;

	// ������ �������������� ����������, ��������� � ������� ������������� ���������
	os << pairsCount << " " << totalPairsCount << " " << totalSquaresWithPairs << endl;
	os << endl;

	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			os << pairsDictionary[i][j] << " ";
		}
		os << endl;
	}
	os << endl;

	// ���������� ������� � �������� ���� ������������� ��������
	os << squareA << endl;
}


// �������� ������ �������� ������
std::ostream& operator << (std::ostream& os, PairSearch& value)
{
	value.Write(os);

return os;
}


// �������� ���������� ��������� ������
std::istream& operator >> (std::istream& is, PairSearch& value)
{
	value.Read(is);

return is;
}


// ����� ��������� ������ �� ����������� ��� ��������� ���������� ��������
void PairSearch::ResetForProcessSquare()
{
	// ��������������� ��������� ������ ������ ���� � ���������� ���
	Generator::CopyState(initialState);

	// ������� ������� ��� ��������
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			pairsDictionary[i][j] = Free;
		}
	}

	// ����� ����� ��������� ���
	pairsCount = 0;

	// ����������� ����� ��������������������
	isInitialized = Yes;
}

// ���������� ���������������� ������������� ���������� ��������
void PairSearch::OnSquareGenerated(Square newSquare)
{
	// ���������� ��������� ������ �� ����������� ��� ��������� ���������� ��������
	ResetForProcessSquare();

	// ��������� ������� ���������������� �������� � ������� ���� ����
	squareA = newSquare;

	// ��������� ����� ���������, ������������� ���������
	FindPairSquare();

	// ����� ����������� ������, ���� ������� ����
	if (pairsCount > 0)
	{
		PrintSearchFooter();
	}
}

// ������ ������ ��� ���
void PairSearch::StartPairSearch()
{
	// ������������� �� ������� ��������� ������ ���
	__hook(&Generator::SquareGenerated, &squareAGenerator, &PairSearch::OnSquareGenerated);

	// ����������� ��������� �������� ��������
	if (isStartFromCheckpoint == Yes)
	{
		FindPairSquare();
	}

	// ��������� ��������� ������������ ��������� ���������
	squareAGenerator.Start();

	// ������������ �� ������� ��������� ������ ���
	__unhook(&Generator::SquareGenerated, &squareAGenerator, &PairSearch::OnSquareGenerated);

	// ����� ������ ������ ���
	PrintSearchTotals();
}


// ��������� ���������� ���� � ���������������� ���
void PairSearch::ProcessPairSquare()
{
	fstream resultFile;		// ����� ��� I/O � ���� � ������������

	// ��������� ��������� ����
	pairsCount++;
	totalPairsCount++;

	if (pairsCount == 1)
	{
		totalSquaresWithPairs++;
	}

	// ����� ���������� � ��������� ����
		// ����� ���������
		if (pairsCount == 1)
		{
			// ����� ���������� � ������ �������� ���� � ���� ���������
			cout << "# ------------------------" << endl;
			cout << "# Detected orthogonal square(s) for the square: " << endl;
			cout << endl;
			cout << squareA;
			cout << "# ------------------------" << endl;

			// ����� ���������� � ����
			resultFile.open(resultFileName, std::ios_base::app);
			resultFile << "# ------------------------" << endl;
			resultFile << "# Detected orthogonal square(s) for the square: " << endl;
			resultFile << endl;
			resultFile << squareA;
			resultFile << "# ------------------------" << endl;
			resultFile.close();
		}

		// ����� ���������� � �������
		cout << newSquare;

		// ����� ���������� � ����
		resultFile.open(resultFileName, std::ios_base::app);
		resultFile << newSquare;
		resultFile.close();

	// �������� ����������� �����
	CreateCheckpoint();
}


// ����� ���������� �� ������ ������ ���������, ������������� � ��������� ���
void PairSearch::PrintSearchFooter()
{
	fstream resultFile;

	// ����� ������ � �������
	cout << "# ------------------------" << endl;
	cout << "# Pairs found: " << pairsCount << endl;
	cout << "# ------------------------" << endl;

	// ����� ������ � ����
	resultFile.open(resultFileName, std::ios_base::app);
	resultFile << "# ------------------------" << endl;
	resultFile << "# Pairs found: " << pairsCount << endl;
	resultFile << "# ------------------------" << endl;
	resultFile.close();
}


// ����� ���������� �� ������ ����� ������ � �����
void PairSearch::PrintSearchTotals()
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
	resultFile << "# ------------------------" << endl;
	resultFile.close();
}


// �������� ����������� �����
void PairSearch::CreateCheckpoint()
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


// ����� ��������, ������� � ����������
void PairSearch::FindPairSquare()
{
	int stepCounter = 0;

	int isGet;			// ���� ��������� ������ �������� ��� ������
	int cellValue;		// ����� �������� ��� ������
	int oldCellValue;	// ������ ��������, �������� � ������

	int stop = 0;		// ���� ���������� ��������� �������

	if (isInitialized == Yes)
	{
		// ������ �������� ������ ��������
		do
		{
			/* ---------- */
			if (stepCounter % 100000000 == 0)
			{
				stepCounter = 0;
				cout << "Current state: " << endl;
				cout << newSquare << endl;
			}
			stepCounter++;
			/* ---------- */

			// ������ �������� ��� ��������� ������ ��������
				// ��������� ���������� ������
				rowId = path[cellId][0];
				columnId = path[cellId][1];

				// ���������� ����� �������� ��� ������ (rowId, columnId)
					// ���������� �������� ����������
					isGet = 0;
					cellValue = Square::Empty;

					// ��������� �������� ��� ������
					for (int i = 0; i < Rank && !isGet; i++)
					{
						// ��������� �������� i �� ����������� ������ � ������ (rowId, columnId)
						if (columns[i][columnId] && rows[rowId][i] && cellsHistory[rowId][columnId][i])
						{
							// �������� �� ������ � �������� � �������, �� ���� ��� ��������� ���������
								// ���������� ����, �������, ��������, ����� ������� ������������ ���������
								isGet = 1;
								// ��������� �������� - �� ���������� �� ��� �� ����������
									// �������� ������ ���������
									if(columnId == rowId)
									{
										if (!primary[i])
										{
											isGet = 0;
										}
									}

									// �������� ������ ���������
									if (rowId == Rank - 1 - columnId)
									{
										if (!secondary[i])
										{
											isGet = 0;
										}
									}

								// ��������� ������������ ���� �� � �������������� ������� ��������
								// ������� �������� - squareA � ������������ �������� ��� ������� - i
								if (pairsDictionary[squareA.Matrix[rowId][columnId]][i] == 0)
								{
									isGet = 0;
								}
						}

						// ����������� ��������, ���������� � �����
						if (isGet)
						{
							cellValue = i;
						}
					}

				// ��������� ���������� ������
				if (isGet)
				{
					// ��������� ���������� ������ ��������
						// ���������� �������� ��������
						oldCellValue = newSquare.Matrix[rowId][columnId];
						// ������ ������ ��������
							// ���������� �������� � �������
							newSquare.Matrix[rowId][columnId] = cellValue;
							// �������� �������� � ��������
							columns[cellValue][columnId] = Used;
							// �������� �������� � �������
							rows[rowId][cellValue] = Used;
							// �������� �������� � ����������
							if (rowId == columnId)
							{
								primary[cellValue] = Used;
							}
							if (rowId == Rank - 1 - columnId)
							{
								secondary[cellValue] = Used;
							}
							// �������� �������� � ������� �������� ������
							cellsHistory[rowId][columnId][cellValue] = Used;

						// ����������� ����������� �������� ��� �������� ������� (��� ��� �� �������� � ���� �������)
						if (oldCellValue != Square::Empty)
						{
							// ���������� �������� � �������
							columns[oldCellValue][columnId] = Free;
							// ���������� �������� � ������
							rows[rowId][oldCellValue] = Free;
							// ���������� �������� � ���������
							if (rowId == columnId)
							{
								primary[oldCellValue] = Free;
							}
							if (rowId == Rank - 1 - columnId)
							{
								secondary[oldCellValue] = Free;
							}
						}

						// �������� �������� ��������� ���� � ��������������
							// �������� �������������� ����� ����
							pairsDictionary[squareA.Matrix[rowId][columnId]][cellValue] = 0;

							// ������� ������� � ������������� ���������� ����
							if (oldCellValue != -1)
							{
								pairsDictionary[squareA.Matrix[rowId][columnId]][oldCellValue] = 1;
							}

						// ��������� ��������� ������������ ��������
						if (cellId == cellsInPath - 1)
						{
							// ������������ ��������� �������
							ProcessPairSquare();
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
							cellValue = newSquare.Matrix[rowId][columnId];
							// ���������� �������� � ��������� �������
							if (cellValue != Square::Empty)
							{
								// ���������� �������� � �������
								columns[cellValue][columnId] = Free;
								// ���������� �������� � ������
								rows[rowId][cellValue] = Free;
								// ���������� �������� � ���������
								if (rowId == columnId)
								{
									primary[cellValue] = Free;
								}
								if (rowId == Rank - 1 - columnId)
								{
									secondary[cellValue] = Free;
								}
								// ���������� ������ ��������
								newSquare.Matrix[rowId][columnId] = Square::Empty;
								// �������� ������� ������ (rowId, columnId)
								for (int i = 0; i < Rank; i++)
								{
									cellsHistory[rowId][columnId][i] = 1;
								}

								// �������� ������������ �������� � �������������� �����
								pairsDictionary[squareA.Matrix[rowId][columnId]][cellValue] = 1;
							}

						// ������ ��� �����
						cellId--;
				}

				// ��������� ������� ��������� ������
				if (keyValue == Square::Empty)
				{
					// ����������� ����� ��� ������������ �������� "-1" ��� ������� ������������ ���� �� ������
					if (newSquare.Matrix[keyRowId][keyColumnId] == keyValue && cellId < 0)
					{
						stop = Yes;
					}
				}
				else
				{
					// ����������� ����� ��� ������� ������������ ��������
					if (newSquare.Matrix[keyRowId][keyColumnId] == keyValue)
					{
						stop = Yes;
					}
				}
		}
		while (!stop);
	}
}
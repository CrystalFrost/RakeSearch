// ��������� ������������ ��������� ���������

# include <iostream>
# include <fstream>
# include <iomanip>
# include <string>
# include <sstream>

# include "Generator.h"

using namespace std;

// ����������� �� ���������
Generator::Generator()
{
	// ����� ��������
	Reset();

	// ������� ��������� ��������
	generatorStateHeader = "# Workunits generator for Rake-search status";
}


// ����������� �����������
Generator::Generator(Generator& source)
{
	CopyState(source);
}


// ��������� ������ �������
void Generator::GenerateWorkunits(string start, string result, string checkpoint, string temp, int workunits)
{
	// ����� ��������� ����������
	Reset();

	// ������������� ����������
	Initialize(start, result, checkpoint, temp, workunits);

	// ��������� ������ �������
	Start();

	// �������� ����������� ����� ����� ���������� ���������
	CreateCheckpoint();
}


// ����� ���� �������� ���������� ��������
void Generator::Reset()
{
	// ����� ���������� �������� ��������
	newSquare.Reset();

	// ����� �������� �������� ��������� ���������
		// ����� ��������, ��������������� �������� ������
		keyRowId = Square::Empty;
		keyColumnId = Square::Empty;
		keyValue = Square::Empty;

		// ����� ��������, ��������� � ���� ���������� ������
		for (int i = 0; i < CellsInPath; i++)
		{
			path[i][0] = Square::Empty;
			path[i][1] = Square::Empty;
		}

		// ����� �������� � �������� ������������� ��������� �� ���������
		for (int i = 0; i < Rank; i++)
		{
			primary[i] = Free;
			secondary[i] = Free;
		}

		// ����� �������� � �������� ������������� ��������� � �������� � �������
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				columns[i][j] = Free;
				rows[i][j] = Free;
			}
		}

		// ����� �������� � ���� ������� ������������� �������� � �������
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				for (int h = 0; h < Rank; h++)
				{
					cellsHistory[i][j][h] = Free;
				}
			}
		}

		// ����� ��������� �������������� ������
		rowId = Square::Empty;
		columnId = Square::Empty;

		// ����� �������� ������
		checkpointFileName.clear();
		tempCheckpointFileName.clear();
		resultFileName.clear();

		// ����� ������ ���������������� ������ � ��������� �������
		snapshotNumber = 0;
		workunitsCount = 0;
		workunitsToGenerate = 0;

		// ����� ������ �������� ������
		workunitKeyRowId = 0;
		workunitKeyColumnId = 0;

		// ����� ����� ��������������������
		isInitialized = No;
}


// ������������� ����������
void Generator::Initialize(string start, string result, string checkpoint, string temp, int workunits)
{
	fstream startFile;
	fstream checkpointFile;

	// ���������� �������� ���������� ��������
	Reset();

	// ���������� ����� ������ ������������, ����������� ����� � �����������
	startParametersFileName = start;
	checkpointFileName = checkpoint;
	resultFileName = result;
	tempCheckpointFileName = temp;
	workunitsToGenerate = workunits;

	// ��������� ���������
	startFile.open(startParametersFileName, std::ios_base::in);
	checkpointFile.open(checkpointFileName, std::ios_base::in);

	if (checkpointFile.is_open())
	{
		// ��������� ������ �� ����� ����������� �����
		Read(checkpointFile);
	}
	else
	{
		// ��������� ������ �� ����� ���������� �������
		if (startFile.is_open())
		{
			Read(startFile);
		}
	}

	startFile.close();
	checkpointFile.close();
}

// �������� ������ ��������� ����������
std::ostream& operator << (std::ostream& os, Generator& value)
{
	value.Write(os);

return os;
}


// �������� ���������� ��������� ����������
std::istream& operator >> (std::istream& is, Generator& value)
{
	value.Read(is);

return is;
}


// ���������� ��������� ���������� �� ������
void Generator::Read(std::istream& is)
{
	int rankToVerify;
	int result = Yes;
	string marker;

	// ���������� ���� ��������������������
	isInitialized = No;

	// ����� ���������
	do
	{
		std::getline(is, marker);
	}
	while (marker != generatorStateHeader);

	// ���������� �� ������ ����� ��������
	is >> rankToVerify;

	// ���������� ������ ������ ������� ��� �����
	if (rankToVerify == Square::Rank)
	{
		// ���������� �� ������ ��������
		is >> newSquare;

		// ���������� �� ������ ���� ������ ������
		for (int i = 0; i < CellsInPath; i++)
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
				is >> primary[i];
			}

			// ���������� ���������� � ��������� �� �������� ���������
			for (int i = 0; i < Rank; i++)
			{
				is >> secondary[i];
			}

			// ���������� ���������� � ��������� � �������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					is >> rows[i][j];
				}
			}

			// ���������� ���������� � ��������� � ��������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					is >> columns[i][j];
				}
			}

			// ���������� ���������� �� ������� �������� � ������� ��������
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						is >> cellsHistory[i][j][h];
					}
				}	
			}	

		// ��������� ����� ���������� ���������������� ������
		is >> snapshotNumber;

		// ���������� ������ � ������� �������� ������
		is >> workunitKeyRowId;
		is >> workunitKeyColumnId;

		// ���������� ���� ��������������������
		isInitialized = Yes;
	}
}


// ������ ��������� ���������� � �����
void Generator::Write(std::ostream& os)
{
	// ������ ���������
	os << generatorStateHeader << endl << endl;

	// ������ � ����� ����� ��������
	os << Square::Rank << endl;

	// ������ � ����� ��������
	os << newSquare;
	
	// ������ � ����� ���� ������ ������
	for (int i = 0; i < CellsInPath; i++)
	{
		os << path[i][0] << " ";
		os << path[i][1] << " ";
		os << endl;
	}
	os << endl;

	// ������ � ����� ���������� � �������� ������
	os << keyRowId << " " << keyColumnId << " " << keyValue << endl;

	// ������ ���������� � ������� ������
	os << rowId << " " << columnId << " " << cellId  << endl;

	// ���������� ������ ������ ��� ��������
	os << endl;

	// ������ ���������� � ��������������� ��������� � ������� ��������
		// ������ ���������� � ��������� �� ������� ���������
		for (int i = 0; i < Rank; i++)
		{
			os << primary[i] << " ";
		}
		os << endl;

		// ������ ���������� � ��������� �� �������� ���������
		for (int i = 0; i < Rank; i++)
		{
			os << secondary[i] << " ";
		}
		os << endl;

		// �������������� ������ ������
		os << endl;

		// ������ ���������� � ��������� � �������
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				os << rows[i][j] << " ";
			}
			os << endl;
		}
		os << endl;

		// ������ ���������� � ��������� � ��������
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				os << columns[i][j] << " ";
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
					os << cellsHistory[i][j][h] << " ";
				}
				os << endl;
			}
			os << endl;
		}
		os << endl;

	// ������ � ����� ���������� � ������ ���������� ���������������� ������
	os << snapshotNumber << endl;
	os << endl;

	// ����� ������ � ������� �������� ������ ��� �������
	os << workunitKeyRowId << " " << workunitKeyColumnId << endl;
	os << endl;
}


// �������� ������������
Generator& Generator::operator = (Generator& value)
{
	CopyState(value);

return *this;
}


// ����������� ��������� � ��������� �������
void Generator::CopyState(Generator& source)
{
	// �������� ���������� ��������� � ���� ���������� �����
	for (int i = 0; i < CellsInPath; i++)
	{
		path[i][0] = source.path[i][0];
		path[i][1] = source.path[i][1];
	}

	keyRowId = source.keyRowId;
	keyColumnId = source.keyColumnId;
	keyValue = source.keyValue;

	// �������� ������� ������ ������������� ��������
	for (int i = 0; i < Rank; i++)
	{
		primary[i] = source.primary[i];
		secondary[i] = source.secondary[i];
	}

	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			columns[i][j] = source.columns[i][j];
			rows[i][j] = source.rows[i][j];
		}
	}

	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			for (int h = 0; h < Rank; h++)
			{
				cellsHistory[i][j][h] = source.cellsHistory[i][j][h];
			}
		}
	}

	// ����������� �������� ������
	startParametersFileName = source.startParametersFileName;
	resultFileName = source.resultFileName;
	checkpointFileName = source.checkpointFileName;
	tempCheckpointFileName = source.tempCheckpointFileName;

	// ����������� ���������� �������� ���������
	isInitialized = source.isInitialized;
	snapshotNumber = source.snapshotNumber;
	rowId = source.rowId;
	columnId = source.columnId;
	cellId = source.cellId;

	// ����������� ������� ��������� ��������
	generatorStateHeader = source.generatorStateHeader;
}


// �������� ����������� �����
void Generator::CreateCheckpoint()
{
	fstream newCheckpointFile;

	// ���������� ��������� � ����� ���� ����������� �����
	newCheckpointFile.open(tempCheckpointFileName, std::ios_base::out);

	if (newCheckpointFile.is_open())
	{
		Write(newCheckpointFile);
		newCheckpointFile.close();
		remove(checkpointFileName.c_str());
		rename(tempCheckpointFileName.c_str(), checkpointFileName.c_str());
	}
}


// ������ ��������� �������
void Generator::Start()
{
	int isGet;			// ���� ��������� ������ �������� ��� ������
	int cellValue;		// ����� �������� ��� ������
	int oldCellValue;	// ������ ��������, �������� � ������

	int stop = 0;		// ���� ���������� ��������� �������

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

						// ��������� ��������� ������������ ��������
						if (cellId == CellsInPath - 1)
						{
							// ���������� �������
							ProcessWorkunit();
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

				// ��������� ������� ��������� ��������� �������
				if (workunitsCount == workunitsToGenerate)
				{
					stop = Yes;
				}
		}
		while (!stop);
	}
}


// ��������� ��������
void Generator::ProcessWorkunit()
{
	switch (Rank)
	{
		case 8:
			ProcessWorkunitR8();
		break;

		case 9:
			ProcessWorkunitR9();
		break;
	}
}


// ������������ ���������� ������� ��� ��������� 8-�� �����
void Generator::ProcessWorkunitR8()
{
	fstream workunitFile;
	string workunitFileName;
	std::stringstream wuNameBuilder;
	int workunitNumber;
	int workunitKeyCellValue;

	// ����������� ������� ������ ������
	snapshotNumber++;

	// ��������� �������
		// ����������� ������ ������� � ���������� �������� �������� ������
		workunitNumber = snapshotNumber;
		workunitKeyCellValue = newSquare.Matrix[workunitKeyRowId][workunitKeyColumnId];

		// ��������� �������� �����
		wuNameBuilder << "wu_" << std::setw(6) << std::setfill('0') << workunitNumber << ".txt";
		workunitFileName = wuNameBuilder.str();

		// ��������� ���� � ��������
		cout << "Workunit " << workunitFileName << ":" << endl;
		cout << newSquare << endl;

		// ��������� ����
		workunitFile.open(workunitFileName, std::ios_base::out);
	
		// ���������� ��������� � �������, � �������� ���� �������� ���������
		workunitFile << "# Move search of pairs OLDS status" << endl
					<< endl
					<< "# Generation of DLS status" << endl
					<< endl
					<< Rank << endl
					<< endl
					<< newSquare
					<< endl;
	
		// ���������� ����� ������ � ����
		workunitFile << (Rank - 1)*Rank - CellsInPath << endl
					 << endl;

		// ���������� ���� ��������� �� �������
		workunitFile << "7 0" << endl << "6 1" << endl << "5 2" << endl << "4 3" << endl << "3 4" << endl << "2 5" << endl << "1 6" << endl
					 << "1 0" << endl << "1 2" << endl << "1 3" << endl << "1 4" << endl << "1 5" << endl << "1 7" << endl
					 << "2 0" << endl << "2 1" << endl << "2 3" << endl << "2 4" << endl << "2 6" << endl << "2 7" << endl
					 << "3 0" << endl << "3 1" << endl << "3 2" << endl << "3 5" << endl << "3 6" << endl << "3 7" << endl
					 << "4 0" << endl << "4 1" << endl << "4 2" << endl << "4 5" << endl << "4 6" << endl << "4 7" << endl
					 << "5 0" << endl << "5 1" << endl << "5 3" << endl << "5 4" << endl << "5 6" << endl << "5 7" << endl
					 << "6 0" << endl << "6 2" << endl << "6 3" << endl << "6 4" << endl << "6 5" << endl << "6 7" << endl
					 << "7 1" << endl << "7 2" << endl << "7 3" << endl << "7 4" << endl << "7 5" << endl << "7 6" << endl << endl;

		// ������ ���������� �������� ������ � � ��������
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << workunitKeyCellValue << endl;
		// ������ ��������� ������� ������ � �������������� ������� ������ � ����
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << 0 << endl;
		workunitFile << endl;

		// ������ ��������� ����������, �����, �������� � ������� ������������� �������� (����� �� Write!)
			// ������ ���������� � ��������� �� ������� ���������
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << primary[i] << " ";
			}
			workunitFile << endl;

			// ������ ���������� � ��������� �� �������� ���������
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << secondary[i] << " ";
			}
			workunitFile << endl;

			// �������������� ������ ������
			workunitFile << endl;

			// ������ ���������� � ��������� � �������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << rows[i][j] << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� � ��������� � ��������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << columns[i][j] << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� �� ������� �������� � ������� ��������
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						workunitFile << cellsHistory[i][j][h] << " ";
					}
					workunitFile << endl;
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� � ����� ��������������� WU
			workunitFile << 0 << endl
						 << endl;

		// ������ ������ ���������� ����������� �����
		workunitFile << "# Move search component status" << endl
					 << endl
					 << "0 0 0" << endl
					 << "0 0" << endl;

		// ��������� ����
		workunitFile.close();

	// ����������� ������� ��������������� ������� 
	workunitsCount++;
}


// ������������ ���������� ������� ��� ��������� 9-�� �����
void Generator::ProcessWorkunitR9()
{
	fstream workunitFile;
	string workunitFileName;
	std::stringstream wuNameBuilder;
	int workunitNumber;
	int workunitKeyCellValue;

	// ����������� ������� ������ ������
	snapshotNumber++;

	// ��������� �������
		// ����������� ������ ������� � ���������� �������� �������� ������
		workunitNumber = snapshotNumber;
		workunitKeyCellValue = newSquare.Matrix[workunitKeyRowId][workunitKeyColumnId];

		// ��������� �������� �����
		wuNameBuilder << "wu_" << std::setw(6) << std::setfill('0') << workunitNumber << ".txt";
		workunitFileName = wuNameBuilder.str();

		// ��������� ���� � ��������
		cout << "Workunit " << workunitFileName << ":" << endl;
		cout << newSquare << endl;

		// ��������� ����
		workunitFile.open(workunitFileName, std::ios_base::out);
	
		// ���������� ��������� � �������, � �������� ���� �������� ���������
		workunitFile << "# Move search of pairs OLDS status" << endl
					<< endl
					<< "# Generation of DLS status" << endl
					<< endl
					<< Rank << endl
					<< endl
					<< newSquare
					<< endl;
	
		// ���������� ����� ������ � ����
		workunitFile << (Rank - 1)*Rank - CellsInPath << endl
					 << endl;

		// ���������� ���� ��������� �� �������
		workunitFile << "1 2" << endl << "1 3" << endl << "1 4" << endl << "1 5" << endl << "1 6" << endl << "1 8" << endl
					 << "2 0" << endl << "2 1" << endl << "2 3" << endl << "2 4" << endl << "2 5" << endl << "2 7" << endl << "2 8" << endl
					 << "3 0" << endl << "3 1" << endl << "3 2" << endl << "3 4" << endl << "3 6" << endl << "3 7" << endl << "3 8" << endl
					 << "4 0" << endl << "4 1" << endl << "4 2" << endl << "4 3" << endl << "4 5" << endl << "4 6" << endl << "4 7" << endl << "4 8" << endl
					 << "5 0" << endl << "5 1" << endl << "5 2" << endl << "5 4" << endl << "5 6" << endl << "5 7" << endl << "5 8" << endl
					 << "6 0" << endl << "6 1" << endl << "6 3" << endl << "6 4" << endl << "6 5" << endl << "6 7" << endl << "6 8" << endl
					 << "7 0" << endl << "7 2" << endl << "7 3" << endl << "7 4" << endl << "7 5" << endl << "7 6" << endl << "7 8" << endl
					 << "8 1" << endl << "8 2" << endl << "8 3" << endl << "8 4" << endl << "8 5" << endl << "8 6" << endl << "8 7" << endl << endl;

		// ������ ���������� �������� ������ � � ��������
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << workunitKeyCellValue << endl;
		// ������ ��������� ������� ������ � �������������� ������� ������ � ����
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << 0 << endl;
		workunitFile << endl;

		// ������ ��������� ����������, �����, �������� � ������� ������������� �������� (����� �� Write!)
			// ������ ���������� � ��������� �� ������� ���������
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << primary[i] << " ";
			}
			workunitFile << endl;

			// ������ ���������� � ��������� �� �������� ���������
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << secondary[i] << " ";
			}
			workunitFile << endl;

			// �������������� ������ ������
			workunitFile << endl;

			// ������ ���������� � ��������� � �������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << rows[i][j] << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� � ��������� � ��������
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << columns[i][j] << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� �� ������� �������� � ������� ��������
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						workunitFile << cellsHistory[i][j][h] << " ";
					}
					workunitFile << endl;
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// ������ ���������� � ����� ��������������� WU
			workunitFile << 0 << endl
						 << endl;

		// ������ ������ ���������� ����������� �����
		workunitFile << "# Move search component status" << endl
					 << endl
					 << "0 0 0" << endl
					 << "0 0" << endl;

		// ��������� ����
		workunitFile.close();

	// ����������� ������� ��������������� ������� 
	workunitsCount++;
}
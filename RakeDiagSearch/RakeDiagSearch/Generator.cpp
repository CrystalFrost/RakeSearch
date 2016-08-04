// ��������� ������������ ��������� ���������

# include "Generator.h"

using namespace std;

// ����������� �� ���������
Generator::Generator()
{
	// ����� ��������
	Reset();

	// ������� ��������� ��������
	generatorStateHeader = "# Generation of DLS status";
}


// ����������� �����������
Generator::Generator(Generator& source)
{
	CopyState(source);
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
		for (int i = 0; i < MaxCellsInPath; i++)
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

		// ����� ����� ��������������� ���������
		squaresCount = 0;

		// ����� ����� ��������������������
		isInitialized = No;
}


// ������������� ����������
void Generator::Initialize(string start, string result, string checkpoint, string temp)
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

		// ��������� ����� ��������������� ���������
		is >> squaresCount;

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

	// ������ � ����� ���������� � ����� ��������������� ���������
	os << squaresCount << endl;
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
	for (int i = 0; i < cellsInPath; i++)
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
	squaresCount = source.squaresCount;
	rowId = source.rowId;
	columnId = source.columnId;
	cellId = source.cellId;

	// ����������� ������� ��������� ��������
	generatorStateHeader = source.generatorStateHeader;
}


// �������� ���� ������ ���������� � ����������� �����
void Generator::SetFileNames(string start, string result, string checkpoint, string temp)
{
	startParametersFileName = start;
	resultFileName = result;
	checkpointFileName = checkpoint;
	tempCheckpointFileName = temp;
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


// ������ ��������� ���������
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
		}
		while (!stop);
	}
}


// ��������� ��������
void Generator::ProcessSquare()
{
	// ����������� ������� ��������� ���������
	squaresCount++;

	// ���������� �������
	__raise SquareGenerated(newSquare);
}
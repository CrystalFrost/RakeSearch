// ������������ ��������� �������

# include "Square.h"

using namespace std;

// ����������� �� ���������. "���������" ���� ��������
Square::Square()
{
	Reset();
}


// �������� �������� �� �������� �������
Square::Square(int source[Rank][Rank])
{
	Initialize(source);
}


// ����������� �����������
Square::Square(Square& source)
{
	Initialize(source.Matrix);
}


// ������������� ���������� ��������
void Square::Initialize(int source[Rank][Rank])
{
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			Matrix[rowId][columnId] = source[rowId][columnId];
		}
	}
}


// ����� �������� ���������� ����������
void Square::Reset()
{
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			Matrix[rowId][columnId] = Empty;
		}
	}
}


// �������� ���������
int Square::operator == (Square& value)
{
	int isEqual = 1;

	for (int rowId = 0; rowId < Rank && isEqual; rowId++)
	{
		for (int columnId = 0; columnId < Rank && isEqual; columnId++)
		{
			if (Matrix[rowId][columnId] != value.Matrix[rowId][columnId])
			{
				isEqual = 0;
			}
		}
	}

return isEqual;
}


// �������� ����������
Square& Square::operator = (Square& value)
{
	Initialize(value.Matrix);

return *this;
}


// �������� ������ ������ ��������
std::ostream& operator << (std::ostream& os, Square& value)
{
	value.Write(os);

return os;
}


// �������� ���������� ������ ��������
std::istream& operator >> (std::istream& is, Square& value)
{
	value.Read(is);

return is;
}


// ������ �������� �� ������
void Square::Read(std::istream& is)
{
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			is >> Matrix[rowId][columnId];
		}
	}
}


// ������ �������� � �����
void Square::Write(std::ostream& os)
{
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			os << Matrix[rowId][columnId] << " ";
		}
		os << endl;
	}

	os << endl;
}

// �������� �������� �� ��, ��� �� �������� ������������ ��������� ���������
int Square::IsDiagonal()
{
	int isDiagonal = 1;

	// �������� ������ ��������� - ��������� [0;0] - [rank;rank]
	for (int itemId = 0; itemId < Rank && isDiagonal; itemId++)
	{
		// �������� �� ���������� �������� [itemId; itemId] �� ����� ���������� ���������� ���������
		for (int comparedId = itemId + 1; comparedId < Rank && isDiagonal; comparedId++)
		{
			if (Matrix[itemId][itemId] == Matrix[comparedId][comparedId])
			{
				isDiagonal = 0;
			}
		}
	}

	// �������� ������ ��������� - ��������� [rank - itemId - 1; itemId]
	for (int itemId = 0; itemId < Rank && isDiagonal; itemId ++)
	{
		// �������� �� ���������� �������� [rank - itemId - 1; itemId] �� ����� ���������� ���������� ���������
		for (int comparedId = itemId + 1; comparedId < Rank && isDiagonal; comparedId++)
		{
			if (Matrix[(Rank - itemId - 1)][itemId] == Matrix[(Rank - comparedId - 1)][comparedId])
			{
				isDiagonal = 0;
			}
		}
	}

	return isDiagonal;
}


// �������� �������� �� ��, ��� �� �������� ��������� ���������
int Square::IsLatin()
{
	int isLatin = 1;

	// �������� �������� ��������
	for (int columnId = 0; columnId < Rank && isLatin; columnId++)
	{
		// �������� ������������ ������� columnId
		for (int rowId = 0; rowId < Rank && isLatin; rowId++)
		{
			// �������� �� ���������� �������� [rowId; columnId] �� ����� ���������� ���������� ������� columndId
			for (int comparedRowId = rowId + 1; comparedRowId < Rank && isLatin; comparedRowId++)
			{
				if (Matrix[comparedRowId][columnId] == Matrix[rowId][columnId])
				{
					isLatin = 0;
				}
			}
		}
	}

	// �������� ����� ��������
	for (int rowId = 0; rowId < Rank && isLatin; rowId++)
	{
		// �������� ������������ ������ rowId
		for (int columnId = 0; columnId < Rank && isLatin; columnId++)
		{
			// �������� �� ���������� �������� [rowId; columnId] �� ����� ���������� ���������� ������ rowId
			for (int comparedColumnId = columnId + 1; comparedColumnId < Rank && isLatin; comparedColumnId++)
			{
				if (Matrix[rowId][columnId] == Matrix[rowId][comparedColumnId])
				{
					isLatin = 0;
				}
			}
		}
	}

	return isLatin;
}


// �������� ��������������� ��������� a � b
int Square::OrthoDegree(Square a, Square b)
{
	int degree = 0;				// ������� ���������������
	int freePair[Rank][Rank];	// ������ ������������� ��� �������� � ������������ �����-��������� ��������

	// ������������� ��� ���� ��� ���������
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			freePair[i][j] = 1;
		}
	}

	// �������� ����, �������������� � �����-��������� ��������
	for (int rowId = 0; rowId < Rank; rowId++)
	{
		for (int columnId = 0; columnId < Rank; columnId++)
		{
			if (freePair[a.Matrix[rowId][columnId]][b.Matrix[rowId][columnId]])
			{
				freePair[a.Matrix[rowId][columnId]][b.Matrix[rowId][columnId]] = 0;
				degree++;
			}
		}
	}

return degree;
}
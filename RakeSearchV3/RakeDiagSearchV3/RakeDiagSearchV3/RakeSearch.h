#pragma once

// ����� ��� ������������ ��������� ��������� ������� "�����������" �����

# include <iostream>
# include <fstream>
# include <string>
# include <immintrin.h>

# include "boinc_api.h"
# include "Square.h"

using namespace std;

class RakeSearch
{
public:
	static const int Rank = Square::Rank;       // ���� �������������� ���������

	RakeSearch();								// ����������� �� ���������
	void Start();								// ������ ��������� ���������
	void Reset();								// ����� ���� �������� ���������� ��������
	void SetFileNames(string start, string result, string checkpoint, string temp);	// ������� ���� ������ ���������� � ����������� �����
	void Initialize(string start, string result, string checkpoint, string temp);	// ������������� ������

private:
	static const int Yes = 1;						// ���� "��"
	static const int No = 0;						// ���� "���"
	static const int MaxCellsInPath = Rank * Rank;	// ������������ ����� �������������� ������
	static const bool isDebug = true;				// ���� ������ ���������� ����������
	static const int CheckpointInterval = 1000000;	// �������� �������� ����������� �����
	static const int OrhoSquaresCacheSize = 128;	// ������ ���� ��� �������� ���������, ������������� ���������������

	string startParametersFileName;			// �������� ����� � ����������� ������� �������
	string resultFileName;					// �������� ����� � ������������
	string checkpointFileName;				// �������� ����� ����������� �����
	string tempCheckpointFileName;			// ��������� �������� ����� ����� ����������� �����
	string workunitHeader;					// ��������� ������ ����� � ��������� ��� ����� ����������� �����

	int isInitialized;						// ���� �������� ������������� ������
	int isStartFromCheckpoint;				// ���� ������� � ����������� �����
	int cellsInPath;						// ����� �������������� ������
	int path[MaxCellsInPath][2];			// ���� ���������� ������� �������� - path[i][0] - ������ �� ���� i, path[i][1] - �������
	int keyRowId;							// ������������� ������ �������� ������ - �� �������� ������� ������ ����� ���������������
	int keyColumnId;						// ������������� ������� �������� ������
	int keyValue;							// �������� �������� ������, �� ���������� �������� ������ ����� ���������������
	int rowId;								// ������������� ������ ������������ ������
	int columnId;							// ������������� ������� �������������� ������
	int cellId;								// ������������� ������ � ������� ����� ������ ��������
	unsigned long long squaresCount;		// ����� ��������������� ���

	unsigned int flagsPrimary;					// "������" ������-����� �������������� �������� �� ������� ���������
	unsigned int flagsSecondary;				// "������" ������-����� �������������� �������� �� �������� ���������
	unsigned int flagsColumns[Rank];			// "�������" ��������, ���������������� � �������� - columns[��������][�������] = 0|1. 0 - �������� ������. 1 - ��������.
	unsigned int flagsRows[Rank];				// "�������" ��������, ���������������� � ������� - rows[������][��������] = 0|1
	unsigned int flagsCellsHistory[Rank][Rank];	// "���" ��������, ������� �������������� ��� ������������ ����������� ����� �������� - cellsHistory[������][�������][��������]

	int pairsCount;					// ����� ������������ ������������ ��������� � ������������� ����� �� ���������� squareA
	int totalPairsCount;			// ����� ����� ������������ ������������ ��������� - � ������ ����� ������
	int totalSquaresWithPairs;		// ����� ����� ���������, � ������� ������ ���� �� ���� �������������

	int squareA[Rank][Rank];		// ������ ��� ��������� ����, ������ � ������� ����� ��������������
	int squareB[Rank][Rank];		// ������ ��������� ��� ����, ���������� ������������� �����
	Square orthoSquares[OrhoSquaresCacheSize];	// ��� ��� �������� ���������, ������������� ���������������

	void PermuteRows();				// ����������� ����� ��������� ��� � ������ ���� � ����
	void ProcessSquare();           // ��������� ������������ ������� �������� ��������� ����
	void ProcessOrthoSquare();		// ��������� ���������� �������������� ��������
	void CheckMutualOrthogonality();// �������� �������� ��������������� ���������
	void CreateCheckpoint();		// �������� ����������� �����
	void Read(std::istream& is);	// ������ ��������� ������ �� ������
	void Write(std::ostream& os);	// ������ ��������� ������ � �����
	void ShowSearchTotals();		// ����������� ����� ������ ������
};
// ��������� ������������ ��������� ���������

# if !defined Generator_h
# define Generator_h

# include <iostream>
# include <fstream>
# include "Square.h"

using namespace std;

class Generator
{
public:
	Generator();							// ����������� �� ���������
	Generator(Generator& source);			// ����������� ����������
	void GenerateWorkunits(string start, string result, string checkpoint, string temp, int workunits);	// ���������� ��������� �������

	Generator& operator = (Generator&  value);									// �������� �����������
	friend std::ostream& operator << (std::ostream& os, Generator& value);		// �������� ������ ��������� ����������
	friend std::istream& operator >> (std::istream& is, Generator& value);		// �������� ���������� ��������� ����������

private:
	static const int Rank = Square::Rank;	// ���� �������� (��� ��������)
	static const int Free = 1;				// ���� ���������� ��� ������������� ��������
	static const int Used = 0;				// ���� �������������� �������� � �����-���� ������ ��������� | ������ | �������
	static const int CellsInPath = 16;		// ����� �������������� ������. ��� ���������� 9 ����� + 1 ������ � ������ ������ - 16 ������
	/*static const int CellsInPath = 7;		// ����� �������������� ������. ��� ������� ��������� 8 ����� - 7 ������*/
	static const int Yes = 1;				// ���� "��"
	static const int No = 0;				// ���� "���"

	void Initialize(string start, string result, string checkpoint, string temp, int workunits);	// ������������� ������
	void Start();							// ������� ��������� ������� ������� ������
	void Reset();							// ����� ���� �������� ���������� ��������

	void CopyState(Generator& source);		// ����������� ���������
	void Read(std::istream& is);			// ���������� ��������� ���������� �� ������
	void Write(std::ostream& os);			// ������ ��������� ���������� � �����

	Square newSquare;						// ������������ �������

	int path[CellsInPath][2];				// ���� ���������� ������� �������� - path[i][0] - ������ �� ���� i, path[i][1] - �������
	int keyRowId;							// ������������� ������ �������� ������ - �� �������� ������� ������ ����� ���������������
	int keyColumnId;						// ������������� ������� �������� ������
	int keyValue;							// �������� �������� ������, �� ���������� �������� ������ ����� ���������������

	int primary[Rank];						// ���������� ������� ���������
	int secondary[Rank];					// ���������� �������� ���������
	int columns[Rank][Rank];				// ������� ��������, ���������������� � �������� - columns[��������][�������] = 0|1. 0 - �������� ������. 1 - ��������.
	int rows[Rank][Rank];					// ������� ��������, ���������������� � ������� - rows[������][��������] = 0|1
	int cellsHistory[Rank][Rank][Rank];		// ��� ��������, ������� �������������� ��� ������������ ����������� ����� �������� - cellsHistory[������][�������][��������]

	string startParametersFileName;			// �������� ����� � ����������� ������� �������
	string resultFileName;					// �������� ����� � ������������
	string checkpointFileName;				// �������� ����� ����������� �����
	string tempCheckpointFileName;			// ��������� �������� ����� ����� ����������� �����

	int isInitialized;						// ���� �������� ������������� ������
	int rowId;								// ������������� ������ ������������ ������
	int columnId;							// ������������� ������� �������������� ������
	int cellId;								// ������������� ������ � ������� ����� ������ ��������
	
	int snapshotNumber;						// ����� ������ �� ������� ������������ ������� (�� i-1 �� i)
	int workunitKeyRowId;					// ������ �������� ������, ������������� � �������
	int workunitKeyColumnId;				// ������� �������� ������, ������������� � �������

	int workunitsCount;						// ����� �������, ��������� � ������ ����� ������� (�� ������ � ����������� �����!)
	int workunitsToGenerate;				// ����� �������, ������� ���������� ������������� (�� ������ � ����������� �����!)

	void CreateCheckpoint();				// �������� ����������� �����
	void ProcessWorkunit();					// ������������ ���������� �������
	void ProcessWorkunitR8();				// ������������ ���������� ������� ��� ��������� 8-�� �����
	void ProcessWorkunitR9();				// ������������ ���������� ������� ��� ��������� 9-�� �����

	string generatorStateHeader;			// ���������, ����� �������� � ����� ���������� ��� ����������� ����� ��� ��������� ���������� ������������ ���������
};

# endif
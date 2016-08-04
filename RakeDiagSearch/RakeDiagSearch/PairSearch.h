// ����� ��� ������������� ������������ ��������� ���������

# if !defined PairSearch_h
# define PairSearch_h

# include <iostream>
# include <string>

# include "Generator.h"

using namespace std;

class PairSearch : protected Generator
{
public:
	PairSearch();							// ����������� �� ���������
	void StartPairSearch();					// ������ ������ ��� ���
	void Reset();							// ����� ���� �������� ���������� ��������
	void Initialize(string start, string result, string checkpoint, string temp);	// ������������� ������

	friend std::ostream& operator << (std::ostream& os, PairSearch& value);		// �������� ������ ��������� ����������
	friend std::istream& operator >> (std::istream& is, PairSearch& value);		// �������� ���������� ��������� ����������

	void OnSquareGenerated(Square newSquare);	// ���������� ���������������� ������������� ���������� ��������

protected:
	Generator squareAGenerator;				// ��������� ���, � ���� � ������� ������ ������������� ��������
	Generator initialState;					// �������������� ��������� ������, � �������� ������������ ����� ������� ���� � ���������� ���
	Square squareA;							// ������� ������� �������� ����
	int pairsCount;							// ����� ������������ ��� � ���������������� ���
	int totalPairsCount;					// ����� ����� ������������ ��� � ������������ ���������
	int totalSquaresWithPairs;				// ����� ����� ���������, ������� ���� ���� �� ���� �������������
	int pairsDictionary[Rank][Rank];		// ������� �������������� ��� � ����������� ���� ���������

	void Read(std::istream& is);				// ���������� ��������� ������ �� ������
	void Write(std::ostream& is);			// ������ ��������� ������ � �����
	void FindPairSquare();					// ����� ��������, ������� � ����������
	void ResetForProcessSquare();			// ����� ��������� ������ �� ����������� ��� ��������� ���������� ��������
	void ProcessPairSquare();				// ��������� ���������� ���� � ���������������� ���
	void PrintSearchFooter();				// ����� ���������� �� ������ ������ ���������, ������������� � ��������� ���
	void PrintSearchTotals();				// ����� ���������� �� ������ ����� ������ � �����

private:
	void CreateCheckpoint();				// �������� ����������� ����� ������

	string pairStateHeader;					// ���������, ����� �������� � ����� ���������� ��� ����������� ����� ��� ��������� ������ ������������� ���������
	int isStartFromCheckpoint;				// ���� ������� � ����������� �����
};

# endif
// ������������ ��������� �������

# if !defined Square_h
# define Square_h

# include <iostream>

using namespace std;

class Square
{
public:
	static const int Rank = 9;					// ���� ��������
	static const int Empty = -1;				// ������, �� �������� ��������

	static int OrthoDegree(Square a, Square b);	// ������� ��������������� ��������� a � b

	Square();									// ����������� �� ���������
	Square(int source[Rank][Rank]);				// ����������� �������� �������� �� �������
	Square(Square& source);						// ����������� �����������

	int operator == (Square& value);										// ���������� ��������� ��������� - ������������ ���������� �������
	Square& operator = (Square& value);										// ���������� ��������� ����������
	friend std::ostream& operator << (std::ostream& os, Square& value);		// ���������� ��������� ������ ������ ��������
	friend std::istream& operator >> (std::istream& is, Square& value);		// ���������� ��������� ���������� ������ ��������

	int IsDiagonal();								// �������� �������� �� ��, ��� �� �������� ������������ ��������� ���������
	int IsLatin();									// �������� �������� �� ��, ��� �� �������� ��������� ���������
	void Initialize(int source[Rank][Rank]);		// ������������� ����������� ��������
	void Reset();									// ����� ���� ������� ������������
	void Read(std::istream& is);					// ������ �������� �� ������
	void Write(std::ostream& os);					// ������ �������� � �����

	int Matrix[Rank][Rank];							// ������� ��������

protected:
private:
};

# endif
// ����� "��������� �������"

# if !defined Result_h
# define Result_h

# include <fstream>

using namespace std;

class Result
{
public:
	Result();					// ����������� �� ���������
	void Read(istream& is);		// ������ ������ ���������� �� �����

	int TotalPairs;				// ����� ����� ��������� ���
	int TotalSquaresWithPairs;	// ����� ����� ��������� � �����
	int SquaresProcessed;		// ����� ������������ ���������
};

# endif
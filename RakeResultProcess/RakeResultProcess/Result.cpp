# include <fstream>
# include <string>

# include "Result.h"

using namespace std;

// ����������� �� ���������
Result::Result()
{
	TotalPairs = 0;
	TotalSquaresWithPairs = 0;
	SquaresProcessed = 0;
}


// ������ ������ ���������� �� �����
void Result::Read(istream& is)
{
	string markerTotalPairs = "# Total pairs found: ";						// ��������� - ������ ����� ��������� ���
	string markerTotalSquaresWithPairs = "# Total squares with pairs: ";	// ��������� - ������ ����� ��������� � �����
	string markerSquaresProcessed_A = "# Processes ";						// ��������� - ������ ������ � ������ ������������ ���������
	string markerSquaresProcessed_B = " milliards ";						// ��������� - ������� � ������ ����������
	string markerSquaresProcessed_C = " squares";							// ��������� - ���������� ��������� ����� ����� ���������

	string line;									// �������� ������
	string lineTotalPairs;							// ������ � ������ ��������� ���
	string lineTotalSquaresWithPairs;				// ������ � ������ ���������, �������� � ����
	string lineSquaresProcessed;					// ������ � ������ ������������ ���������

	int markerTotalPairsLength = markerTotalPairs.length();

	int milliards;			// ����� ���������� ���������
	int squares;			// ����� ��������� � ������ ���������
	size_t milliardsStart;
	size_t milliardsEnds;
	size_t squaresStart;
	size_t squaresEnds;

	// ����� ������� ������
	do
	{
		std::getline(is, line);
	}
	while (line.substr(0, markerTotalPairsLength) != markerTotalPairs);

	// ������ ������
	lineTotalPairs = line;
	std::getline(is, lineTotalSquaresWithPairs);
	std::getline(is, lineSquaresProcessed);

	// ���������� ������ �� �����
	TotalPairs = std::stoi(lineTotalPairs.substr(markerTotalPairs.length()));
	TotalSquaresWithPairs = std::stoi(lineTotalSquaresWithPairs.substr(markerTotalSquaresWithPairs.length()));

	milliardsStart = markerSquaresProcessed_A.length();
	milliardsEnds = lineSquaresProcessed.find(markerSquaresProcessed_B);
	squaresStart = milliardsEnds + markerSquaresProcessed_B.length();
	squaresEnds = lineSquaresProcessed.find(markerSquaresProcessed_C);

	milliards = std::stoi(lineSquaresProcessed.substr(milliardsStart, milliardsEnds - milliardsStart));
	squares = std::stoi(lineSquaresProcessed.substr(squaresStart, squaresEnds - squaresStart));

	SquaresProcessed = milliards*1000000000 + squares;
}
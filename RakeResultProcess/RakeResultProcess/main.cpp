# include <iostream>
# include <fstream>
# include <string>
# include <Windows.h>

# include "Result.h"

using namespace std;

int main()
{
	string directory = "D:\\WorkFiles\\RakeResults\\";		// ����� � �������� � �������
	string mask = "rs*.txt";								// ����� ������ ������
	string maskWithPath = directory + mask;					// ����� ������ ������ �� ��������� ����
	string fileName;										// ��� ���������� �����
	string fullFileName;									// ��� ����� � ����
	WIN32_FIND_DATA findData;								// ������ ������
	HANDLE findHandle;										// ��������� �������� ������

	fstream resultFile;
	Result taskResult;
	long long int totalSquares = 0;

	// ��������� ������ 
		// ������������� ������
		findHandle = FindFirstFile(maskWithPath.c_str(), &findData);
	
		// ������� ������
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				fileName = findData.cFileName;
				fullFileName = directory + fileName;

				resultFile.open(fullFileName, std::ios_base::in);
				if (resultFile.is_open())
				{
					taskResult.Read(resultFile);
				}
				resultFile.close();

				cout << fileName << " " << taskResult.SquaresProcessed << endl;
				totalSquares += taskResult.SquaresProcessed;
			}
			while (FindNextFile(findHandle, &findData));
		}

		// �������� �������� ������
		FindClose(findHandle);

	// ����� ����������
	cout << endl;
	cout << "Squares in results: " << totalSquares << endl;
	cout << endl << "Press any key to continue ..." << endl;

	cin.get();

	return 0;
}
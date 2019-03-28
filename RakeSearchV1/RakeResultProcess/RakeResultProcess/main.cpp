# include <iostream>
# include <fstream>
# include <string>
# include <Windows.h>

# include "Result.h"

using namespace std;

int main()
{
	string directory = "D:\\WorkFiles\\RakeResults\\";		// Пусть к каталогу с файлами
	string mask = "rs*.txt";								// Маска поиска файлов
	string maskWithPath = directory + mask;					// Маска поиска файлов по заданному пути
	string fileName;										// Имя найденного файла
	string fullFileName;									// Имя файла с путём
	WIN32_FIND_DATA findData;								// Данные поиска
	HANDLE findHandle;										// Заголовок структур поиска

	fstream resultFile;
	Result taskResult;
	long long int totalSquares = 0;

	// Обработка файлов 
		// Инициализация поиска
		findHandle = FindFirstFile(maskWithPath.c_str(), &findData);
	
		// Перебор файлов
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

		// Закрытие структур поиска
		FindClose(findHandle);

	// Вывод результата
	cout << endl;
	cout << "Squares in results: " << totalSquares << endl;
	cout << endl << "Press any key to continue ..." << endl;

	cin.get();

	return 0;
}
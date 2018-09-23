# include <iostream>
# include <fstream>
# include "Square.h"
# include "DLX_DLS.h"

using namespace std;

int main(int argsCount, char* argsValues[])
{
	// Переменные построения графа из ОДЛК
	Square startSquare;								// Квадрат, с которого начинается поиск
	string startFileName = "start_square.txt";		// Имя файла с начальным квадратов
	string setFileName = "squares_set.txt";			// Имя файла с набором найденных квадратов
	string edgesFileName = "squares_edges.txt";		// Имя файла с перечнем рёбер графа из ОДЛК
	string totalsFileName = "graph_totals.txt";		// Имя файла с характеристиками графа из ОДЛК
	string checkListFileName = "check_list.txt";	// Имя файла с проверочным списком квадратов
	int checkListSize = 0;							// Размер проверочного списка квадратов

	fstream startFile;								// Файл с начальным квадратом
	fstream setFile;								// Файл с набором найденных квадратов
	fstream edgesFile;								// Файл с перечнем рёбер графа из ОДЛК
	fstream totalsFile;								// Файл с характеристиками графа из ОДЛК
	fstream checkListFile;							// Файл с проверочным списком квадратов

	orth_mate_search finder;				// Искатель ОДЛК

	const int FullOrthoDegree = Square::Rank*Square::Rank;	// Степерь ортогональности, соответствующая ортогональным квадратам

	vector<Square> checkList;				// Список квадратов на соответствие с которым проверяются найденные
	vector<vector<int>> startSquareVector;	// Квадрат, с которого начинается поиск
	vector<vector<vector<int>>> result;		// Результат поиска
	vector<Square> squaresSet;				// Множество найденных квадратов
	vector<Square> iterationSet;			// Множество квадратов, найденных в рамках итерации
	vector<Square> currentSet;				// Множество квадратов, обрабатываемых в рамках текущей итерации
	Square newSquare;						// Квадрат, добавляемый в множество
	Square checkSquare;						// Квадрат, добавляемый в список проверки
	int isNewSquare = 0;					// Флаг нового квадрата
	int isTheGraph = 0;						// Флаг нахождения квадрата из The Graph
	int newSquaresInIteration = 0;			// Число новых квадратов, найденных в рамках очередной итерации
	int edgesCount = 0;						// Число рёбер в графе из ОДЛК

	// Переменные для разбора параметров
	string argument;							// Название обрабатываемого аргумента
	string paramName;							// Имя параметра, полученное из аргумента
	string paramValue;							// Значение параметра, полученное из аргумента
	string startFileParamName = "--start";		// Название параметра "Файл с начальным квадратом"
	string setFileParamName = "--set";			// Название параметра "Файл с набором найденных квадратов"
	string edgesFileParamName = "--edges";		// Название параметра "Файл с рёбрами графа"
	string totalsFileParamName = "--totals";	// Название параметра "Файл с характеристиками графа"
	string checkListParamName = "--check_list";	// Название параметра "Проверочный список квадратов" (для определения того, что столкнулись с The Graph)
	string checkListSizeParamName = "--check_list_size";	// Название параметра "Длина проверочного списка"
	string delimiter = "=";						// Разделитель имени параметра и его значения в аргументе программы
	size_t delimiterPosition = -1;              // Позиция разделителя имени параметра и его значения в аргументе
	std::string::size_type size;

	// Чтение входных параметров
	if (argsCount > 0)
	{
		for (int i = 0; i < argsCount; i++)
		{
			// Обработка i-го аргумента
				// Чтение из i-го аргумента названия параметра и его значение
				argument = argsValues[i];
				delimiterPosition = argument.find(delimiter, 0);

				if (delimiterPosition != string::npos)
				{
					paramName = argument.substr(0, delimiterPosition);
					paramValue = argument.substr(delimiterPosition + 1);
				}

				// Обработка прочитанного параметра
				if (paramName == startFileParamName)
				{
					startFileName = paramValue;
				}

				if (paramName == setFileParamName)
				{
					setFileName = paramValue;
				}

				if (paramName == edgesFileParamName)
				{
					edgesFileName = paramValue;
				}

				if (paramName == totalsFileParamName)
				{
					totalsFileName = paramValue;
				}

				if (paramName == checkListParamName)
				{
					checkListFileName = paramValue;
				}

				if (paramName == checkListSizeParamName)
				{
					checkListSize =std::stoi(paramValue, nullptr);
				}
		}
	}

	// Вывод прочитанных параметров
	cout << "Start square file name: " << startFileName << endl;
	cout << "Squares set file name: " << setFileName << endl;
	cout << "Graph edges file name: " << edgesFileName << endl;
	cout << "Totals file name: " << totalsFileName << endl;
	cout << "Check list file name: " << checkListFileName << endl;

	// Считывание квадратов из списка проверки
	checkListFile.open(checkListFileName, std::ios_base::in);

	if (checkListFile.is_open())
	{
		for (int i = 0; i < checkListSize; i++)
		{
			checkListFile >> checkSquare;
			checkList.push_back(checkSquare);
		}
	}

	// Считывание списка ДЛК
	startFile.open(startFileName, std::ios_base::in);

	if (startFile.is_open())
	{
		startFile >> startSquare;
	}

	startFile.close();

	// Поиск структуры из ортогональных квадратов, начинающейся с заданного

		// Добавляем первый квадрат в множество
		squaresSet.clear();
		squaresSet.push_back(startSquare);
		currentSet.clear();
		currentSet.push_back(startSquare);

		// Поиск новых квадратов, ортогональных к известному и ортогональных ему и т.д.
		cout << "Starting to search an orthogonal squares ..." << endl;

		do
		{
			iterationSet.clear();
			newSquaresInIteration = 0;

			// Поиск квадратов, ортогональных квадратам, обнаруженным ранее множества
			for (int squareId = 0; squareId < currentSet.size(); squareId++)
			{
				startSquareVector << currentSet[squareId];
				result.clear();
				finder.check_dlx_rc1(startSquareVector, result);
	
				for (int resultItemId = 0; resultItemId < result.size(); resultItemId++)
				{
					result[resultItemId] >> newSquare;
					iterationSet.push_back(newSquare);
				}
			}

			// Очистка рабочего множества
			currentSet.clear();

			// Проверка найденных квадратов на нахождение в проверочном списке
			for (int i = 0; i < iterationSet.size() && !isTheGraph; i++)
			{
				for (int j = 0; j < checkListSize && !isTheGraph; j++)
				{
					if (iterationSet[i] == checkList[j])
					{
						isTheGraph = 1;
					}
				}
			}

			// Проверка найденных квадратов на уникальность и добавление новых квадратов в основное и рабочее множество
			if (!isTheGraph)
			{
				for (int i = 0; i < iterationSet.size(); i++)
				{
					isNewSquare = 1;

					for (int j = 0; j < squaresSet.size() && isNewSquare; j++)
					{
						if (squaresSet[j] == iterationSet[i])
						{
							isNewSquare = 0;
						}
					}

					if (isNewSquare)
					{
						squaresSet.push_back(iterationSet[i]);
						currentSet.push_back(iterationSet[i]);
						newSquaresInIteration++;
					}
				}

				cout << "Found " << newSquaresInIteration << " new squares " << endl;
				cout << "Total squares: " << squaresSet.size() << endl;
			}
			else
			{
				cout << "Found The Graph! Exit." << endl;
			}
		}
		while (newSquaresInIteration > 0 && !isTheGraph);

	// Обработка найденного графа
	if (!isTheGraph)
	{
		// Запись найденных квадратов в файл
		cout << "Save squares set ..." << endl;

		setFile.open(setFileName, std::ios_base::out);

		if (setFile.is_open())
		{
			for (int i = 0; i < squaresSet.size(); i++)
			{
				setFile << squaresSet[i];
			}
		}

		setFile.close();

		// Построение графа из найденных ОДЛК
			// Вывод сообщения
			cout << "Start to count graph edges..." << endl;

			// Открытие файла для записи рёбер графа
			edgesFile.open(edgesFileName, std::ios_base::out);
			edgesFile << "IdA;IdB" << endl;

			// Проверка ортогональности ДЛК
			for (int i = 0; i < squaresSet.size(); i++)
			{
				// Поиск квадратов ортогональных i-му квадрату
				for (int j = i + 1; j < squaresSet.size(); j++)
				{
					// Проверка ортогональности i-го и j-го квадрата
					if (Square::OrthoDegree(squaresSet[i], squaresSet[j]) == FullOrthoDegree)
					{
						edgesCount++;
						edgesFile << i << ";" << j << endl;

						if (edgesCount % 1000 == 0)
						{
							cout << "Reach " << edgesCount << " edges" << endl;
						}
					}
				}
			}

		// Закрытие файла, хранящего рёбра графа
		edgesFile.close();

		// Завершение работы
			// Вывод итогов на экран
			cout << "--------------------------------" << endl;
			cout << "Total squares in graph: " << squaresSet.size() << endl;
			cout << "Total edges count: " << edgesCount << endl;

			// Сохранение итогов в файл
			totalsFile.open(totalsFileName, std::ios_base::out);

			if (totalsFile.is_open())
			{
				totalsFile << "Total squares in graph: " << squaresSet.size() << endl;
				totalsFile << "Total edges count: " << edgesCount << endl;
			}

			totalsFile.close();
	}

	return 0;
}

/* Фрагменты старого кода, которые могут быть полезны
	int matrix[Square::Rank][Square::Rank] =
	{
		{0, 1, 2, 3, 4, 5, 6, 7, 8},
		{8, 2, 6, 0, 5, 1, 7, 4, 3},
		{2, 4, 1, 6, 0, 8, 5, 3, 7},
		{1, 7, 8, 4, 6, 2, 3, 0, 5},
		{6, 5, 4, 7, 3, 0, 1, 8, 2},
		{5, 8, 3, 1, 2, 7, 0, 6, 4},
		{4, 3, 0, 5, 7, 6, 8, 2, 1},
		{3, 6, 7, 8, 1, 4, 2, 5, 0},
		{7, 0, 5, 2, 8, 3, 4, 1, 6}
	};

	Square simpleSquare(matrix);

	fstream outfile;
	outfile.open("square.txt", std::ios_base::out);

	if (outfile.is_open())
	{
		outfile << simpleSquare;
		outfile.close();
	}

	Square simpleSquare;
	fstream infile;
	infile.open("square.txt", std::ios_base::in);

	if (infile.is_open())
	{
		infile >> simpleSquare;
		infile.close();
	}

	
	// Конверсия из формата Эдуарда
	const int squaresCount = 61824;
	Square item;
	fstream infile;
	fstream outfile;
	string infileName = "dls9_61824_ODLS.txt";
	string outfileName = "DLS_61284_ODLS_Bracket.txt";

	infile.open(infileName, std::ios_base::in);
	outfile.open(outfileName, std::ios_base::out);

	if (infile.is_open() && outfile.is_open())
	{
		for (int i = 0; i < squaresCount; i++)
		{
			infile >> item;
			outfile << item;
		}
	}

	outfile.close();
	infile.close();

	// 374064 edges

	const int FullOrthoDegree = Square::Rank*Square::Rank;
	const int SquaresCount = 61824;
	Square* squareList = new Square[SquaresCount];
	string infileName = "Graph-1.txt";
	string edgesFileName = "Edges-1.txt";
	fstream infile;
	fstream edgesFile;
	int edgesCount = 0;

	// Считывание списка ДЛК
	infile.open(infileName, std::ios_base::in);

	if (infile.is_open())
	{
		for (int i = 0; i < SquaresCount; i++)
		{
			infile >> squareList[i];

			if (i % 1000 == 0)
			{
				cout << i << " squares readed" << endl;
			}
		}
	}

	infile.close();

	cout << "Start to count graph edges..." << endl;

	// Открытие файла для записи рёбер графа
	edgesFile.open(edgesFileName, std::ios_base::out);
	edgesFile << "IdA;IdB" << endl;

	// Проверка ортогональности ДЛК
	for (int i = 0; i < SquaresCount; i++)
	{
		// Поиск квадратов ортогональных i-му квадрату
		for (int j = i + 1; j < SquaresCount; j++)
		{
			// Проверка ортогональности i-го и j-го квадрата
			if (Square::OrthoDegree(squareList[i], squareList[j]) == FullOrthoDegree)
			{
				edgesCount++;
				edgesFile << i << ";" << j << endl;

				if (edgesCount % 1000 == 0)
				{
					cout << "Reach " << edgesCount << " edges" << endl;
				}
			}
		}
	}

	// Завершение работы
	edgesFile.close();
	delete[] squareList;

	cout << "Total edges count: " << edgesCount << endl;
	cout << "Press any key to continue ..." << endl;
	cin.get();
*/

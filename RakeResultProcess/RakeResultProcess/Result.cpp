# include <fstream>
# include <string>

# include "Result.h"

using namespace std;

// Конструктор по умолчанию
Result::Result()
{
	TotalPairs = 0;
	TotalSquaresWithPairs = 0;
	SquaresProcessed = 0;
}


// Чтение данных результата из файла
void Result::Read(istream& is)
{
	string markerTotalPairs = "# Total pairs found: ";						// Подстрока - маркер числа найденных пар
	string markerTotalSquaresWithPairs = "# Total squares with pairs: ";	// Подстрока - маркер числа квадратов в парах
	string markerSquaresProcessed_A = "# Processes ";						// Подстрока - начало строки с числом обработанных квадратов
	string markerSquaresProcessed_B = " milliards ";						// Подстрока - вставка с числом миллиардов
	string markerSquaresProcessed_C = " squares";							// Подстрока - отрезаемое окончание после числа квадратов

	string line;									// Читаемая строка
	string lineTotalPairs;							// Строка с числом найденных пар
	string lineTotalSquaresWithPairs;				// Строка с числом квадратов, входящих в пары
	string lineSquaresProcessed;					// Строка с числом обработанных квадратов

	int markerTotalPairsLength = markerTotalPairs.length();

	int milliards;			// Число миллиардов квадратов
	int squares;			// Число квадратов в рамках миллиарда
	size_t milliardsStart;
	size_t milliardsEnds;
	size_t squaresStart;
	size_t squaresEnds;

	// Поиск маркера итогов
	do
	{
		std::getline(is, line);
	}
	while (line.substr(0, markerTotalPairsLength) != markerTotalPairs);

	// Чтение итогов
	lineTotalPairs = line;
	std::getline(is, lineTotalSquaresWithPairs);
	std::getline(is, lineSquaresProcessed);

	// Извлечение итогов из строк
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

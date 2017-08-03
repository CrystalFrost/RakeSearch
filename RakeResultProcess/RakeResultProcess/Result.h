// Класс "Результат расчёта"

# if !defined Result_h
# define Result_h

# include <fstream>

using namespace std;

class Result
{
public:
	Result();					// Конструктор по умолчанию
	void Read(istream& is);		// Чтение данных результата из файла

	int TotalPairs;				// Общее число найденных пар
	int TotalSquaresWithPairs;	// Общее число квадратов в парах
	int SquaresProcessed;		// Число обработанных квадратов
};

# endif

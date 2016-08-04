// Поиск пар ортогональных диагональных латинских квадратов

# if !defined PairSearch_h
# define PairSearch_h

# include <iostream>
# include <string>

# include "Generator.h"

using namespace std;

class PairSearch : protected Generator
{
public:
	PairSearch();							// Конструктор по умолчанию
	void StartPairSearch();					// Запуск поиска пар ДЛК
	void Reset();							// Сброс всех значений внутренних структур
	void Initialize(string start, string result, string checkpoint, string temp);	// Инициализация поиска

	friend std::ostream& operator << (std::ostream& os, PairSearch& value);		// Оператор записи состояние генератора
	friend std::istream& operator >> (std::istream& is, PairSearch& value);		// Оператор считывания состояния генератора

	void OnSquareGenerated(Square newSquare);	// Обработчик сгенерированного диагонального латинского квадрата

protected:
	Generator squareAGenerator;				// Генератор ДЛК, в пару к которым ищутся ортогональные квадраты
	Generator initialState;					// Первоначальное состояние поиска, к которому возвращаемся перед поиском пары к очередному ДЛК
	Square squareA;							// Матрица первого квадрата пары
	int pairsCount;							// Число обнаруженных пар к сгенерированному ДЛК
	int totalPairsCount;					// Общее число обнаруженных пар к обработанным квадратам
	int totalSquaresWithPairs;				// Общее число квадратов, которым есть хотя бы один ортогональный
	int pairsDictionary[Rank][Rank];		// Матрица задействования пар в формируемой паре квадратов

	void Read(std::istream& is);				// Считывание состояние поиска из потока
	void Write(std::ostream& is);			// Запись состояния поиска в поток
	void FindPairSquare();					// Поиск квадрата, парного к найденному
	void ResetForProcessSquare();			// Сброс состояния поиска на необходимое для обработки очередного квадрата
	void ProcessPairSquare();				// Обработка нахождения пары к сгенерированному ДЛК
	void PrintSearchFooter();				// Вывод информации об итогах поиска квадратов, ортогональных к заданному ДЛК
	void PrintSearchTotals();				// Вывод информации об итогах всего поиска в целом

private:
	void CreateCheckpoint();				// Создание контрольной точки поиска

	string pairStateHeader;					// Заголовок, после которого в файле параметров или контрольной точки идёт состояние поиска ортогональных квадратов
	int isStartFromCheckpoint;				// Флаг запуска с контрольной точки
};

# endif
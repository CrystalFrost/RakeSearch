#pragma once

// Поиск пар диагональных латинских квадратов методом "перетасовки" строк

# include <iostream>
# include <fstream>
# include <string>
# include <immintrin.h>

# include "boinc_api.h"
# include "Square.h"

using namespace std;

class RakeSearch
{
public:
	static const int Rank = Square::Rank;       // Ранг обрабатываемых квадратов

	RakeSearch();								// Конструктор по умолчанию
	void Start();								// Запуск генерации квадратов
	void Reset();								// Сброс всех значений внутренних структур
	void SetFileNames(string start, string result, string checkpoint, string temp);	// Задание имен файлов параметров и контрольной точки
	void Initialize(string start, string result, string checkpoint, string temp);	// Инициализация поиска

private:
	static const int Yes = 1;						// Флаг "Да"
	static const int No = 0;						// Флаг "Нет"
	static const int MaxCellsInPath = Rank * Rank;	// Максимальное число обрабатываемых клеток
	static const bool isDebug = true;				// Флаг вывода отладочной информации
	static const int CheckpointInterval = 1000000;	// Интервал создания контрольных точек
	static const int OrhoSquaresCacheSize = 128;	// Размер кэша для хранения квадратов, ортогональных обрабатываемому
	static const int MinOrthoMetric = 81;			// Минимальное значение характеристики ортогональности при котором пара записывается в результат

	string startParametersFileName;			// Название файла с параметрами запуска расчёта
	string resultFileName;					// Название файла с результатами
	string checkpointFileName;				// Название файла контрольной точки
	string tempCheckpointFileName;			// Временное название файла новой контрольной точки
	string workunitHeader;					// Заголовок данных файла с заданиями или файла контрольной точки

	int isInitialized;						// Флаг успешной инициализации поиска
	int isStartFromCheckpoint;				// Флаг запуска с контрольной точки
	int cellsInPath;						// Число обрабатываемых клеток
	int path[MaxCellsInPath][2];			// Путь заполнения матрицы квадрата - path[i][0] - строка на шаге i, path[i][1] - столбец
	int keyRowId;							// Идентификатор строки ключевой клетки - по значению которой расчёт будет останавливаться
	int keyColumnId;						// Идентификатор столбца ключевой клетки
	int keyValue;							// Значение ключевой клетки, по достижению которого расчёт будет останавливаться
	int rowId;								// Идентификатор строки обратываемой клетки
	int columnId;							// Идентификатор столбца обрабатываемой клетки
	int cellId;								// Идентификатор клетки в перечне шагов обхода квадрата
	unsigned long long squaresCount;		// Число сгенерированных ДЛК

	unsigned int flagsPrimary;					// "Массив" флагов-битов задействования значений на главной диагонали
	unsigned int flagsSecondary;				// "Массив" флагов-битов задействования значений на побочной диагонали
	unsigned int flagsColumns[Rank];			// "Матрица" значений, использовавшихся в столбцах - columns[значение][столбец] = 0|1. 0 - значение занято. 1 - свободно.
	unsigned int flagsRows[Rank];				// "Матрица" значений, использовавшихся в строках - rows[строка][значение] = 0|1
	unsigned int flagsCellsHistory[Rank][Rank];	// "Куб" значений, которые использовались для формирования построенной части квадрата - cellsHistory[строка][столбец][значение]

	int pairsCount;					// Число обнаруженных диагональных квадратов в перестановках строк из найденного squareA
	int totalPairsCount;			// Общее число обнаруженных диагональных квадратов - в рамках всего поиска
	int totalSquaresWithPairs;		// Общее число квадратов, к которым найден хотя бы один ортогональный

	int squareA[Rank][Rank];		// Первый ДЛК возможной пары, строки в котором будут переставляться
	int squareB[Rank][Rank];		// Второй возможный ДЛК пары, получаемый перестановкой строк
	Square orthoSquares[OrhoSquaresCacheSize];	// Кэш для хранения квадратов, ортогональных обрабатываемому

	void PermuteRows();				// Перетасовка строк заданного ДЛК в поиске ОДЛК к нему
	void ProcessSquare();           // Обработка построенного первого квадрата возможной пары
	void ProcessOrthoSquare();		// Обработка найденного ортогонального квадрата
	void CheckMutualOrthogonality();// Проверка взаимной ортогональности квадратов
	void CreateCheckpoint();		// Создание контрольной точки
	void Read(std::istream& is);	// Чтение состояния поиска из потока
	void Write(std::ostream& os);	// Запись состояния поиска в поток
	void ShowSearchTotals();		// Отображение общих итогов поиска
};

// Генератор Диагональных латинских квадратов

# if !defined Generator_h
# define Generator_h

# include <iostream>
# include <fstream>
# include <iomanip>
# include <string>
# include <sstream>
# include <immintrin.h>

# include "Square.h"

using namespace std;

class Generator
{
public:
	Generator();							// Конструктор по умолчанию
	Generator(Generator& source);			// Конструктор копировния
	void GenerateWorkunits(string start, string directory, string checkpoint, string temp, int workunits);  // Выполнение генерации заданий

	Generator& operator = (Generator&  value);								// Оператор копирования
	friend std::ostream& operator << (std::ostream& os, Generator& value);	// Оператор записи состояние генератора
	friend std::istream& operator >> (std::istream& is, Generator& value);	// Оператор считывания состояния генератора

private:
	static const int Rank = Square::Rank;	// Ранг квадрата (для удобства)
	static const int Free = 1;				// Флаг свободного для использования значения
	static const int Used = 0;				// Флаг задействования значения в какой-либо клетке диагонали | строки | столбца
	static const int CellsInPath = 28;		// Число обрабатываемых клеток. Для диагоналей квадрата 10 ранга + 1 клетка в первой строке - 18 клеток
	static const int Yes = 1;				// Флаг "Да"
	static const int No = 0;				// Флаг "Нет"

	void Initialize(string start, string directory, string checkpoint, string temp, int workunits);	// Инициализация поиска
	void Start();							// Функция генерации заданий обходом клеток
	void Reset();							// Сброс всех значений внутренних структур

	void CopyState(Generator& source);		// Копирование состояния
	void Read(std::istream& is);			// Считывание состояние генератора из потока
	void Write(std::ostream& os);			// Запись состояния генератора в поток

	Square newSquare;						// Генерируемый квадрат

	int path[CellsInPath][2];	// Путь заполнения матрицы квадрата - path[i][0] - строка на шаге i, path[i][1] - столбец
	int keyRowId;				// Идентификатор строки ключевой клетки - по значению которой расчёт будет останавливаться
	int keyColumnId;			// Идентификатор столбца ключевой клетки
	int keyValue;				// Значение ключевой клетки, по достижению которого расчёт будет останавливаться

	unsigned int flagsPrimary;					// "Массив" флагов-битов задействования значений на главной диагонали
	unsigned int flagsSecondary;				// "Массив" флагов-битов задействования значений на побочной диагонали
	unsigned int flagsColumns[Rank];			// "Матрица" значений, использовавшихся в столбцах - columns[значение][столбец] = 0|1. 0 - значение занято. 1 - свободно.
	unsigned int flagsRows[Rank];				// "Матрица" значений, использовавшихся в строках - rows[строка][значение] = 0|1
	unsigned int flagsCellsHistory[Rank][Rank];	// "Куб" значений, которые использовались для формирования построенной части квадрата - cellsHistory[строка][столбец][значение]

	string startParametersFileName;	// Название файла с параметрами запуска расчёта
	string checkpointFileName;		// Название файла контрольной точки
	string tempCheckpointFileName;	// Временное название файла новой контрольной точки
	string workunitsDirectory;		// Название каталога, в который необходимо записать файлы заданий

	int isInitialized;				// Флаг успешной инициализации поиска
	int rowId;						// Идентификатор строки обратываемой клетки
	int columnId;					// Идентификатор столбца обрабатываемой клетки
	int cellId;						// Идентификатор клетки в перечне шагов обхода квадрата

	int snapshotNumber;				// Номер снимка по которым генерируются задания (от i-1 до i)
	int workunitKeyRowId;			// Строка ключевой клетки, прописываемая в задании
	int workunitKeyColumnId;		// Столбец ключевой клетки, приписываемый в задании

	long long workunitsCount;		// Число заданий, созданных в рамках этого прогона (не входит в контрольную точку!)
	long long workunitsToGenerate;	// Число заданий, которые необходимо сгенерировать (не входит в контрольную точку!)

	void CreateCheckpoint();		// Создание контрольной точки
	void ProcessWorkunit();			// Формирование очередного задания
	void ProcessWorkunitR8();		// Формирование очередного задания для квадратов 8-го ранга
	void ProcessWorkunitR9();		// Формирование очередного задания для квадратов 9-го ранга
	void ProcessWorkunitR10();		// Формирование очередного задания для квадратов 10-го ранга

	string generatorStateHeader;	// Заголовок, после которого в файле параметров или контрольной точки идёт состояние генератора диагональных квадратов
};

# endif

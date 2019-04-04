// Генератор Диагональных латинских квадратов

# include "Generator.h"

using namespace std;

// Конструктор по умолчанию
Generator::Generator()
{
	// Сброс настроек
	Reset();

	// Задание текстовых констант
	generatorStateHeader = "# Workunits generator for RakeSearch status";
}


// Конструктор копирования
Generator::Generator(Generator& source)
{
	CopyState(source);
}


// Генерация пакета заданий
void Generator::GenerateWorkunits(string start, string directory, string checkpoint, string temp, int workunits)
{
	// Сброс состояния генератора
	Reset();

	// Инициализация генератора
	Initialize(start, directory, checkpoint, temp, workunits);

	// Генерация пакета заданий
	Start();

	// Создание контрольной точки после завершения генерации
	CreateCheckpoint();
}


// Сброс всех значений внутренних структур
void Generator::Reset()
{
	// Сброс внутренних значений квадрата
	newSquare.Reset();

	// Сброс значений структур генерации квадратов
		// Сброс значений, соответствующих ключевой клетке
		keyRowId = Square::Empty;
		keyColumnId = Square::Empty;
		keyValue = Square::Empty;

		// Сброс значений, связанных с путём заполнения клеток
		for (int i = 0; i < CellsInPath; i++)
		{
			path[i][0] = Square::Empty;
			path[i][1] = Square::Empty;
		}

		// Сброс значений в векторах использования элементов на диагонали
		flagsPrimary = (1u << Rank) - 1;
		flagsSecondary = (1u << Rank) - 1;

		// Сброс значений в матрицах использования элементов в столбцах и строках
		for (int i = 0; i < Rank; i++)
		{
			flagsColumns[i] = (1u << Rank) - 1;
			flagsRows[i] = (1u << Rank) - 1;
		}

		// Сброс значений в кубе истории использования значений в клетках
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				flagsCellsHistory[i][j] = (1u << Rank) - 1;
			}
		}

		// Сброс координат обрабатываемой клетки
		rowId = Square::Empty;
		columnId = Square::Empty;

		// Сброс названий файлов
		checkpointFileName.clear();
		tempCheckpointFileName.clear();
		workunitsDirectory.clear();

		// Сброс номера сгенерированного снимка и счётчиков заданий
		snapshotNumber = 0;
		workunitsCount = 0;
		workunitsToGenerate = 0;

		// Сброс адреса ключевой клетки
		workunitKeyRowId = 0;
		workunitKeyColumnId = 0;

		// Сброс флага инициализированности
		isInitialized = No;
}


// Инициализация генератора
void Generator::Initialize(string start, string directory, string checkpoint, string temp, int workunits)
{
	fstream startFile;
	fstream checkpointFile;

	// Сбрасываем значения внутренних структур
	Reset();

	// Запоминаем имена файлов конфигурации, контрольных точек и результатов
	startParametersFileName = start;
	checkpointFileName = checkpoint;
	workunitsDirectory = directory;
	tempCheckpointFileName = temp;
	workunitsToGenerate = workunits;

	// Считываем настройки
	startFile.open(startParametersFileName.c_str(), std::ios_base::in);
	checkpointFile.open(checkpointFileName.c_str(), std::ios_base::in);

	if (checkpointFile.is_open())
	{
		// Считываем данные из файла контрольной точки
		Read(checkpointFile);
	}
	else
	{
		// Считываем данные из файла параметров запуска
		if (startFile.is_open())
		{
			Read(startFile);
		}
	}

	startFile.close();
	checkpointFile.close();
}

// Оператор записи состояния генератора
std::ostream& operator << (std::ostream& os, Generator& value)
{
	value.Write(os);

	return os;
}


// Оператор считывания состояния генератора
std::istream& operator >> (std::istream& is, Generator& value)
{
	value.Read(is);

	return is;
}


// Считывание состояние генератора из потока
void Generator::Read(std::istream& is)
{
	int rankToVerify;
	int result = Yes;
	string marker;
	unsigned int storedBit = 0;

	// Сбрасываем флаг инициализированности
	isInitialized = No;

	// Поиск заголовка
	do
	{
		std::getline(is, marker);
	}
	while (marker != generatorStateHeader);

	// Считывание из потока ранга квадрата
	is >> rankToVerify;

	// Считывание данных поиска нужного нам ранга
	if (rankToVerify == Square::Rank)
	{
		// Считывание из потока квадрата
		is >> newSquare;

	// Считывание из потока пути обхода клеток
	for (int i = 0; i < CellsInPath; i++)
    {
		is >> path[i][0];
		is >> path[i][1];
	}

	// Считывание из потока информации о ключевой клетке
	is >> keyRowId;
    is >> keyColumnId;
    is >> keyValue;

    // Считывание информации об обрабатываемой клетке
    is >> rowId;
    is >> columnId;
    is >> cellId;

    // Считывание из потока информации о задействованных значениях и истории значений
		// Считывание информации о значениях на главной диагонали
		for (int i = 0; i < Rank; i++)
		{
			is >> storedBit;
			if (storedBit)
			{
				flagsPrimary |= 1u << i;
			}
			else
			{
				flagsPrimary &= ~(1u << i);
			}
		}

		// Считывание информации о значениях на побочной диагонали
		for (int i = 0; i < Rank; i++)
		{
			is >> storedBit;
			if (storedBit)
			{
				flagsSecondary |= 1u << i;
			}
			else
			{
				flagsSecondary &= ~(1u << i);
			}
		}

		// Считывание информации о значениях в строках
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				is >> storedBit;
				if (storedBit)
				{
					flagsRows[i] |= 1u << j;
				}
				else
				{
					flagsRows[i] &= ~(1u << j);
				}
			}
		}

		// Считывание информации о значениях в столбцах
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				is >> storedBit;
				if (storedBit)
				{
					flagsColumns[i] |= 1u << j;
				}
				else
				{
					flagsColumns[i] &= ~(1u << j);
				}
			}
		}

		// Считывание информации об истории значений в клетках квадрата
		for (int h = 0; h < Rank; h++)
		{
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					is >> storedBit;
					if (storedBit)
					{
						flagsCellsHistory[i][j] |= 1u << h;
					}
					else
					{
						flagsCellsHistory[i][j] &= ~(1u << h);
					}
				}
			}
		}

		// Считываем номер последнего сгенерированного снимка
		is >> snapshotNumber;

		// Считывание строки и столбца ключевой клетки
		is >> workunitKeyRowId;
		is >> workunitKeyColumnId;

		// Выставляем флаг инициализированности
		isInitialized = Yes;
	}
}


// Запись состояния генератора в поток
void Generator::Write(std::ostream& os)
{
	// Запись заголовка
	os << generatorStateHeader << endl << endl;

	// Запись в поток ранга квадрата
	os << Square::Rank << endl;

	// Запись в поток квадрата
	os << newSquare;
  
	// Запись в поток пути обхода клеток
	for (int i = 0; i < CellsInPath; i++)
	{
		os << path[i][0] << " ";
		os << path[i][1] << " ";
		os << endl;
	}
	os << endl;

	// Запись в поток информации о ключевой клетке
	os << keyRowId << " " << keyColumnId << " " << keyValue << endl;

	// Запись информации о текущей клетке
	os << rowId << " " << columnId << " " << cellId  << endl;

	// Записываем пустую строку для удобства
	os << endl;

	// Запись информации о задействованных значениях и истории значений
		// Запись информации о значениях на главной диагонали
		for (int i = 0; i < Rank; i++)
		{
			os << ((flagsPrimary & (1u << i)) > 0 ? 1 : 0) << " ";
		}
		os << endl;

		// Запись информации о значениях на побочной диагонали
		for (int i = 0; i < Rank; i++)
		{
			os << ((flagsSecondary & (1u << i)) > 0 ? 1 : 0) << " ";
		}
		os << endl;

		// Дополнительная пустая строка
		os << endl;

		// Запись информации о значениях в строках
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				os << ((flagsRows[i] & (1u << j)) > 0 ? 1 : 0) << " ";
			}
			os << endl;
		}
		os << endl;

		// Запись информации о значениях в столбцах
		for (int i = 0; i < Rank; i++)
		{
			for (int j = 0; j < Rank; j++)
			{
				os << ((flagsColumns[i] & (1u << j)) > 0 ? 1 : 0) << " ";
			}
			os << endl;
		}
		os << endl;

		// Запись информации об истории значений в клетках квадрата
		for (int h = 0; h < Rank; h++)
		{
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					os << ((flagsCellsHistory[i][j] & (1u << h)) > 0 ? 1 : 0) << " ";
				}
				os << endl;
			}
			os << endl;
		}
		os << endl;

	// Запись в поток информации о номере последнего сгенерированного снимка
	os << snapshotNumber << endl;
	os << endl;

	// Запись строки и столбца ключевой клетки для задания
	os << workunitKeyRowId << " " << workunitKeyColumnId << endl;
	os << endl;
}


// Оператор присваивания
Generator& Generator::operator = (Generator& value)
{
	CopyState(value);

	return *this;
}


// Копирование состояния с заданного объекта
void Generator::CopyState(Generator& source)
{
	// Копируем переменные связанные с путём заполнения ячеек
	for (int i = 0; i < CellsInPath; i++)
	{
		path[i][0] = source.path[i][0];
		path[i][1] = source.path[i][1];
	}

	keyRowId = source.keyRowId;
	keyColumnId = source.keyColumnId;
	keyValue = source.keyValue;

	// Копируем массивы флагов использования значений
	flagsPrimary = source.flagsPrimary;
	flagsSecondary = source.flagsSecondary;

	for (int i = 0; i < Rank; i++)
	{
		flagsColumns[i] = source.flagsColumns[i];
		flagsRows[i] = source.flagsRows[i];
	}

	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			flagsCellsHistory[i][j] = source.flagsCellsHistory[i][j];
		}
	}

	// Копирование названий файлов
	startParametersFileName = source.startParametersFileName;
	workunitsDirectory = source.workunitsDirectory;
	checkpointFileName = source.checkpointFileName;
	tempCheckpointFileName = source.tempCheckpointFileName;

	// Копирование переменных текущего состояния
	isInitialized = source.isInitialized;
	snapshotNumber = source.snapshotNumber;
	rowId = source.rowId;
	columnId = source.columnId;
	cellId = source.cellId;

	// Копирование адресов текстовых констант
	generatorStateHeader = source.generatorStateHeader;
}


// Создание контрольной точки
void Generator::CreateCheckpoint()
{
	fstream newCheckpointFile;

	// Записываем настройки в новый файл контрольной точки
	newCheckpointFile.open(tempCheckpointFileName.c_str(), std::ios_base::out);

	if (newCheckpointFile.is_open())
	{
		Write(newCheckpointFile);
		newCheckpointFile.close();
		remove(checkpointFileName.c_str());
		rename(tempCheckpointFileName.c_str(), checkpointFileName.c_str());
	}
}


// Запуск генерации заданий
void Generator::Start()
{
	unsigned long bitIndex;      // Позиция выставленного бита в "массиве" флагов
	unsigned int freeValuesMask; // Итоговая маска из битов - флагов занятости значений
	unsigned int isGet;          // Флаг получения нового значения для клетки
	int cellValue;               // Новое значение для клетки
	int oldCellValue;            // Старое значение, стоявшее в клетке

	int stop = 0;                // Флаг достижения окончания расчёта

	if (isInitialized == Yes)
	{
		// Подбор значений клеток квадрата
		do
		{
			// Подбор значения для очередной клетки квадрата
				// Считываем координаты клетки
				rowId = path[cellId][0];
				columnId = path[cellId][1];

				// Генерируем новое значение для клетки (rowId, columnId)
					// Сбрасываем значения переменных
					cellValue = Square::Empty;
					freeValuesMask = (1u << Rank) - 1;

					// Подбираем значение для клетки
						// Применяем маску главной диагонали
						if (rowId == columnId)
						{
							freeValuesMask &= flagsPrimary;
						}
						// Применяем маску побочной диагонали
						if (Rank - rowId - 1 == columnId)
						{
							freeValuesMask &= flagsSecondary;
						}

						// Применяем маску строк, столбцов и истории значений
						freeValuesMask &= flagsColumns[columnId] & flagsRows[rowId] & flagsCellsHistory[rowId][columnId];

						// Определяем минимально возможное для использования в клетке значение
						# ifdef _MSC_VER
						isGet = _BitScanForward(&bitIndex, freeValuesMask);
						cellValue = bitIndex;
						# else
						bitIndex = __builtin_ffs(freeValuesMask);
						if (bitIndex > 0)
						{
							isGet = 1;
							cellValue = bitIndex - 1;
						}
						else
						{
							isGet = 0;
						}
						# endif

				// Обработка результата поиска
				if (isGet)
				{
					// Обработка найденного нового значения
						// Считывание текущего значения
						oldCellValue = newSquare.Matrix[rowId][columnId];

						// Запись нового значения
							// Записываем значение в квадрат
							newSquare.Matrix[rowId][columnId] = cellValue;
							// Отмечаем значение в столбцах
							flagsColumns[columnId] ^= 1u << cellValue;
							// Отмечаем значение в строках
							flagsRows[rowId] ^= 1u << cellValue;
							// Отмечаем значение в диагоналях
							if (rowId == columnId)
							{
							 	flagsPrimary ^= 1u << cellValue;
							}
							if (rowId == Rank - 1 - columnId)
							{
								flagsSecondary ^= 1u << cellValue;
							}
							// Отмечаем значение в истории значений клетки
							flagsCellsHistory[rowId][columnId] ^= 1u << cellValue;

						// Возвращение предыдущего значения без зачистки истории 
						// (так как мы работаем с этой клеткой)
						if (oldCellValue != Square::Empty)
						{
							// Возвращаем значение в столбцы
							flagsColumns[columnId] |= 1u << oldCellValue;
							// Возвращаем значение в строки
							flagsRows[rowId] |= 1u << oldCellValue;
							// Возвращаем значение в диагонали
							if (rowId == columnId)
							{
								flagsPrimary |= 1u << oldCellValue;
							}
							if (rowId == Rank - 1 - columnId)
							{
								flagsSecondary |= 1u << oldCellValue;
							}
						}

						// Обработка окончания формирования квадрата
						if (cellId == CellsInPath - 1)
						{
							// Генерируем задание
							ProcessWorkunit();
						}
						else
						{
							// Делаем шаг вперёд
							cellId++;
						}
				}
				else
				{
					// Обработка факта ненахождения нового значения в клетке (rowId; columnId)
						// Возвращаем текущее значение из квадрата в массивы
						// Считываем текущее значение
						cellValue = newSquare.Matrix[rowId][columnId];
						// Возвращаем значение в служебные массивы
						if (cellValue != Square::Empty)
						{
							// Возвращаем значение в столбцы
							flagsColumns[columnId] |= 1u << cellValue;
							// Возвращаем значение в строки
							flagsRows[rowId] |= 1u << cellValue;
							// Возвращаем значение в диагонали
							if (rowId == columnId)
							{
								flagsPrimary |= 1u << cellValue;
							}
							if (rowId == Rank - 1 - columnId)
							{
								flagsSecondary |= 1u << cellValue;
							}
							// Сбрасываем клетку квадрата
							newSquare.Matrix[rowId][columnId] = Square::Empty;
							// Зачищаем историю клетки (rowId, columnId)
							flagsCellsHistory[rowId][columnId] = (1u << Rank) - 1;
						}

						// Делаем шаг назад
						cellId--;
				}

				// Проверяем условия окончания поиска
				if (keyValue == Square::Empty)
				{
					// Выставление флага при завершаеющем значении "-1" при котором производится уход из клетки
					if (newSquare.Matrix[keyRowId][keyColumnId] == keyValue && cellId < 0)
					{
						stop = Yes;
					}
				}
				else
				{
					// Выставление флага при обычном завершаеющем значении
					if (newSquare.Matrix[keyRowId][keyColumnId] == keyValue)
					{
						stop = Yes;
					}
				}

				// Проверяем условие окончания генерации заданий
				if (workunitsCount == workunitsToGenerate)
				{
					stop = Yes;
				}
		}
		while (!stop);

		cout << "Number of combinations: " << workunitsCount << endl;
	}
}


// Обработка квадрата
void Generator::ProcessWorkunit()
{
	switch (Rank)
	{
		case 8:
			ProcessWorkunitR8();
		break;

		case 9:
			ProcessWorkunitR9();
		break;

		case 10:
			ProcessWorkunitR10();
		break;
  }
}


// Формирование очередного задания для квадратов 8-го ранга
void Generator::ProcessWorkunitR8()
{
	fstream workunitFile;
	string workunitFileName;
	std::stringstream wuNameBuilder;
	int workunitNumber;
	int workunitKeyCellValue;

	// Увеличиваем счётчик номера снимка
	snapshotNumber++;

	// Формируем задание
		// Определение номера задания и стопорного значения ключевой клетки
		workunitNumber = snapshotNumber;
		workunitKeyCellValue = newSquare.Matrix[workunitKeyRowId][workunitKeyColumnId];

		// Формируем название файла
		if (!workunitsDirectory.empty())
		{
			wuNameBuilder << workunitsDirectory << "wu_" << std::setw(6) << std::setfill('0') << workunitNumber << ".txt";
		}
		else
		{
			wuNameBuilder << "wu_" << std::setw(6) << std::setfill('0') << workunitNumber << ".txt";
		}
		workunitFileName = wuNameBuilder.str();

		// Формируем файл с заданием
		cout << "Workunit " << workunitFileName << ":" << endl;
		cout << newSquare << endl;

		// Открываем файл
		workunitFile.open(workunitFileName.c_str(), std::ios_base::out);

		// Записываем заголовки и квадрат, с которого надо начинать генерацию
		workunitFile << "# Move search of pairs OLDS status" << endl
			<< endl
			<< "# Generation of DLS status" << endl
			<< endl
			<< Rank << endl
			<< endl
			<< newSquare
			<< endl;

		// Записываем число клеток в пути
		workunitFile << (Rank - 1)*Rank - CellsInPath << endl
			<< endl;

		// Записываем путь переходов по клеткам
		workunitFile << "7 0" << endl << "6 1" << endl << "5 2" << endl << "4 3" << endl << "3 4" << endl << "2 5" << endl << "1 6" << endl
			<< "1 0" << endl << "1 2" << endl << "1 3" << endl << "1 4" << endl << "1 5" << endl << "1 7" << endl
			<< "2 0" << endl << "2 1" << endl << "2 3" << endl << "2 4" << endl << "2 6" << endl << "2 7" << endl
			<< "3 0" << endl << "3 1" << endl << "3 2" << endl << "3 5" << endl << "3 6" << endl << "3 7" << endl
			<< "4 0" << endl << "4 1" << endl << "4 2" << endl << "4 5" << endl << "4 6" << endl << "4 7" << endl
			<< "5 0" << endl << "5 1" << endl << "5 3" << endl << "5 4" << endl << "5 6" << endl << "5 7" << endl
			<< "6 0" << endl << "6 2" << endl << "6 3" << endl << "6 4" << endl << "6 5" << endl << "6 7" << endl
			<< "7 1" << endl << "7 2" << endl << "7 3" << endl << "7 4" << endl << "7 5" << endl << "7 6" << endl << endl;

		// Запись координаты ключевой клетки и её значение
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << workunitKeyCellValue << endl;
		// Запись координат текущей клетки и идентификатора текущей клетки в пути
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << 0 << endl;
		workunitFile << endl;

		// Запись состояний диагоналей, строк, столбцов и истории использования значений (копия из Write!)
			// Запись информации о значениях на главной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsPrimary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Запись информации о значениях на побочной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsSecondary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Дополнительная пустая строка
			workunitFile << endl;

			// Запись информации о значениях в строках
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsRows[i] & (1u << j)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о значениях в столбцах
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsColumns[j] & (1u << i)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации об истории значений в клетках квадрата
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						workunitFile << ((flagsCellsHistory[i][j] & (1u << h)) > 0 ? 1 : 0) << " ";
					}
					workunitFile << endl;
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о числе сгенерированных WU
			workunitFile << 0 << endl
				<< endl;

			// Запись данных компоненты перетасовки строк
			workunitFile << "# Move search component status" << endl
				<< endl
				<< "0 0 0" << endl
				<< "0 0" << endl;

			// Закрываем файл
			workunitFile.close();

	// Увеличиваем счётчик сгенерированных заданий 
	workunitsCount++;
}


// Формирование очередного задания для квадратов 9-го ранга
void Generator::ProcessWorkunitR9()
{
	fstream workunitFile;
	string workunitFileName;
	std::stringstream wuNameBuilder;
	int workunitNumber;
	int workunitKeyCellValue;

	// Увеличиваем счётчик номера снимка
	snapshotNumber++;

	// Формируем задание
		// Определение номера задания и стопорного значения ключевой клетки
		workunitNumber = snapshotNumber;
		workunitKeyCellValue = newSquare.Matrix[workunitKeyRowId][workunitKeyColumnId];

		// Формируем название файла
		if (!workunitsDirectory.empty())
		{
			wuNameBuilder << workunitsDirectory << "/wu_" << std::setw(9) << std::setfill('0') << workunitNumber << ".txt";
		}
		else
		{
			wuNameBuilder << "wu_" << std::setw(9) << std::setfill('0') << workunitNumber << ".txt";
		}
		workunitFileName = wuNameBuilder.str();

		// Формируем файл с заданием
		cout << "Workunit " << workunitFileName << ":" << endl;
		cout << newSquare << endl;

		// Открываем файл
		workunitFile.open(workunitFileName.c_str(), std::ios_base::out);

		// Записываем заголовки и квадрат, с которого надо начинать генерацию
		workunitFile << "# Move search of pairs OLDS status" << endl
			<< endl
			<< "# Generation of DLS status" << endl
			<< endl
			<< Rank << endl
			<< endl
			<< newSquare
			<< endl;

		// Записываем число клеток в пути
		workunitFile << (Rank - 1)*Rank - CellsInPath << endl
			<< endl;

		// Записываем путь переходов по клеткам
		workunitFile << "1 2" << endl << "1 3" << endl << "1 4" << endl << "1 5" << endl << "1 6" << endl << "1 8" << endl
			<< "2 0" << endl << "2 1" << endl << "2 3" << endl << "2 4" << endl << "2 5" << endl << "2 7" << endl << "2 8" << endl
			<< "3 0" << endl << "3 1" << endl << "3 2" << endl << "3 4" << endl << "3 6" << endl << "3 7" << endl << "3 8" << endl
			<< "4 0" << endl << "4 1" << endl << "4 2" << endl << "4 3" << endl << "4 5" << endl << "4 6" << endl << "4 7" << endl << "4 8" << endl
			<< "5 0" << endl << "5 1" << endl << "5 2" << endl << "5 4" << endl << "5 6" << endl << "5 7" << endl << "5 8" << endl
			<< "6 0" << endl << "6 1" << endl << "6 3" << endl << "6 4" << endl << "6 5" << endl << "6 7" << endl << "6 8" << endl
			<< "7 0" << endl << "7 2" << endl << "7 3" << endl << "7 4" << endl << "7 5" << endl << "7 6" << endl << "7 8" << endl
			<< "8 1" << endl << "8 2" << endl << "8 3" << endl << "8 4" << endl << "8 5" << endl << "8 6" << endl << "8 7" << endl << endl;

		// Запись координаты ключевой клетки и её значение
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << workunitKeyCellValue << endl;
		// Запись координат текущей клетки и идентификатора текущей клетки в пути
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << 0 << endl;
		workunitFile << endl;

		// Запись состояний диагоналей, строк, столбцов и истории использования значений (копия из Write!)
			// Запись информации о значениях на главной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsPrimary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Запись информации о значениях на побочной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsSecondary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Дополнительная пустая строка
			workunitFile << endl;

			// Запись информации о значениях в строках
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsRows[i] & (1u << j)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о значениях в столбцах
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsColumns[j] & (1u << i)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации об истории значений в клетках квадрата
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						workunitFile << ((flagsCellsHistory[i][j] & (1u << h)) > 0 ? 1 : 0) << " ";
					}
					workunitFile << endl;
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о числе сгенерированных WU
			workunitFile << 0 << endl
				<< endl;

			// Запись данных компоненты перетасовки строк
			workunitFile << "# Move search component status" << endl
				<< endl
				<< "0 0 0" << endl
				<< "0 0" << endl;

		// Закрываем файл
		workunitFile.close();

	// Увеличиваем счётчик сгенерированных заданий 
	workunitsCount++;
}


// Формирование очередного задания для квадратов 10-го ранга
void Generator::ProcessWorkunitR10()
{
	fstream workunitFile;
	string workunitFileName;
	std::stringstream wuNameBuilder;
	long long workunitNumber;
	int workunitKeyCellValue;

	// Увеличиваем счётчик номера снимка
	snapshotNumber++;

	// Формируем задание
		// Определение номера задания и стопорного значения ключевой клетки
		workunitNumber = snapshotNumber;
		workunitKeyCellValue = newSquare.Matrix[workunitKeyRowId][workunitKeyColumnId];

		// Формируем название файла
		if (!workunitsDirectory.empty())
		{
			wuNameBuilder << workunitsDirectory << "/r10_wu_" << std::setw(16) << std::setfill('0') << workunitNumber << ".txt";
		}
		else
		{
			wuNameBuilder << "rake_wu_r10_" << std::setw(16) << std::setfill('0') << workunitNumber << ".txt";
		}
		workunitFileName = wuNameBuilder.str();

		// Формируем файл с заданием 
		cout << "Workunit " << workunitFileName << ":" << endl;
		cout << newSquare << endl;

		// Открываем файл
		workunitFile.open(workunitFileName.c_str(), std::ios_base::out);

		// Записываем заголовки и квадрат, с которого надо начинать генерацию
		workunitFile << "# RakeSearch of diagonal Latin squares" << endl
			<< endl
			<< Rank << endl
			<< endl
			<< newSquare
			<< endl;

		// Записываем число клеток в пути
		workunitFile << (Rank - 1)*Rank - CellsInPath << endl
		<< endl;

		// Записываем путь переходов по клеткам
		workunitFile
			<< "2 3" << endl << "2 4" << endl << "2 5" << endl << "2 6" << endl << "2 8" << endl << "2 9" << endl
			<< "3 0" << endl << "3 1" << endl << "3 2" << endl << "3 4" << endl << "3 5" << endl << "3 7" << endl << "3 8" << endl << "3 9" << endl
			<< "4 0" << endl << "4 1" << endl << "4 2" << endl << "4 3" << endl << "4 6" << endl << "4 7" << endl << "4 8" << endl << "4 9" << endl
			<< "5 0" << endl << "5 1" << endl << "5 2" << endl << "5 3" << endl << "5 6" << endl << "5 7" << endl << "5 8" << endl << "5 9" << endl
			<< "6 0" << endl << "6 1" << endl << "6 2" << endl << "6 4" << endl << "6 5" << endl << "6 7" << endl << "6 8" << endl << "6 9" << endl
			<< "7 0" << endl << "7 1" << endl << "7 3" << endl << "7 4" << endl << "7 5" << endl << "7 6" << endl << "7 8" << endl << "7 9" << endl
			<< "8 0" << endl << "8 2" << endl << "8 3" << endl << "8 4" << endl << "8 5" << endl << "8 6" << endl << "8 7" << endl << "8 9" << endl
			<< "9 1" << endl << "9 2" << endl << "9 3" << endl << "9 4" << endl << "9 5" << endl << "9 6" << endl << "9 7" << endl << "9 8" << endl << endl;

		// Запись координаты ключевой клетки и её значение
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << workunitKeyCellValue << endl;
		// Запись координат текущей клетки и идентификатора текущей клетки в пути
		workunitFile << workunitKeyRowId << " " << workunitKeyColumnId << " " << 0 << endl;
		workunitFile << endl;

		// Запись состояний диагоналей, строк, столбцов и истории использования значений (копия из Write!)
			// Запись информации о значениях на главной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsPrimary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Запись информации о значениях на побочной диагонали
			for (int i = 0; i < Rank; i++)
			{
				workunitFile << ((flagsSecondary & (1u << i)) > 0 ? 1 : 0) << " ";
			}
			workunitFile << endl;

			// Дополнительная пустая строка
			workunitFile << endl;

			// Запись информации о значениях в строках
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsRows[i] & (1u << j)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о значениях в столбцах
			for (int i = 0; i < Rank; i++)
			{
				for (int j = 0; j < Rank; j++)
				{
					workunitFile << ((flagsColumns[i] & (1u << j)) > 0 ? 1 : 0) << " ";
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации об истории значений в клетках квадрата
			for (int h = 0; h < Rank; h++)
			{
				for (int i = 0; i < Rank; i++)
				{
					for (int j = 0; j < Rank; j++)
					{
						workunitFile << ((flagsCellsHistory[i][j] & (1u << h)) > 0 ? 1 : 0) << " ";
					}
					workunitFile << endl;
				}
				workunitFile << endl;
			}
			workunitFile << endl;

			// Запись информации о числе сгенерированных WU
			workunitFile << 0 << endl
				<< endl;

			// Запись данных компоненты перетасовки строк - число найденных пар для текущего квадрата, общее число пар и число квадратов с парами
			workunitFile << "0 0 0 " << endl;

		// Закрываем файл
		workunitFile.close();

	// Увеличиваем счётчик сгенерированных заданий 
	workunitsCount++;
}

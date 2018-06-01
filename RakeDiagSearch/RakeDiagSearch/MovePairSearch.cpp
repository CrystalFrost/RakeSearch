// Поиск пар диагональных латинских квадратов методом перетасовки строк

# include "MovePairSearch.h"
# include "boinc_api.h"

// Конструктор по умолчанию
MovePairSearch::MovePairSearch()
{
	Reset();
}

// Сброс настроек поиска
void MovePairSearch::Reset()
{
	// Сброс настроек генератора ДЛК
	squareAGenerator.Reset();

	// Сброс значений отдельного поиска
		// Сброс числа найденых пар для заданного ДЛК
		pairsCount = 0;

	// Сброс значений глобальных счётчиков
	totalPairsCount = 0;
	totalSquaresWithPairs = 0;
	totalProcessedSquaresSmall = 0;
	totalProcessedSquaresLarge = 0;

	startParametersFileName = "start_parameters.txt";
	resultFileName = "result.txt";
	checkpointFileName = "checkpoint.txt";
	tempCheckpointFileName = "tmp_checkpoint.txt";

	// Задание константы - заголовка в файле параметров или контрольной точке
	moveSearchGlobalHeader = "# Move search of pairs OLDS status";
	moveSearchComponentHeader = "# Move search component status";

	// Сброс флага инициализации
	isInitialized = 0;
}


// Инициализация поиска
void MovePairSearch::InitializeMoveSearch(string start, string result, 
																			 string checkpoint, string temp)
{
	fstream startFile;
	fstream checkpointFile;

	// Считывание названий имен файлов
	startParametersFileName = start;
	resultFileName = result;
	checkpointFileName = checkpoint;
	tempCheckpointFileName = temp;

	// Считываем состояние генератора и поиска из файла контрольной точки или начальных значений
		// Открытие файлов со стартовыми параметрами и файла контрольной точки
		startFile.open(startParametersFileName.c_str(), std::ios_base::in);
		checkpointFile.open(checkpointFileName.c_str(), std::ios_base::in);

		// Считываение состояния
	if (checkpointFile.is_open())
	{
		// Считывание состояния из файла контрольной точки
		try
		{
			Read(checkpointFile);
			isStartFromCheckpoint = 1;
		}
		catch (...)
		{
			cerr << "Error opening checkpoint file! Starting with workunit start parameters." << endl;
			isStartFromCheckpoint = 0;
		}
		}

	if (isStartFromCheckpoint != 1)
		{
		// Считывание состояния из файла стартовых параметров
		Read(startFile);
		isStartFromCheckpoint = 0;
		}

		// Закрытие файлов
		startFile.close();
		checkpointFile.close();
}


// Чтение состояния поиска из потока
void MovePairSearch::Read(istream& is)
{
	string marker;

	// Сброс флага инициализированности
	isInitialized = 0;

	// Считывание состояния поиска
		// Находим маркер начала состояния
		do
	{
		std::getline(is, marker);

		if (is.eof())
		{
			throw ("Expected start marker, but EOF found.");
		}
		}
		while (marker != moveSearchGlobalHeader);
		
		// Считываем состояние генератора ДЛК
		is >> squareAGenerator;

		// Находим маркер компоненты перетасовки
	do
	{
		std::getline(is, marker);

		if (is.eof())
		{
			throw ("Expected start marker, but EOF found.");
		}
	}
		while (marker != moveSearchComponentHeader);

		// Считываем переменные поиска перетасовкой (по факту - переменные со статистикой)
		is >> pairsCount;
		is >> totalPairsCount;
		is >> totalSquaresWithPairs;
		is >> totalProcessedSquaresLarge;
		is >> totalProcessedSquaresSmall;

	// Выставление флага инициализированности
	isInitialized = 1;
}


// Запись состояния поиска в поток
void MovePairSearch::Write(ostream& os)
{
	// Запись состояния поиска
		// Запись заголовка
		os << moveSearchGlobalHeader << endl;
		os << endl;

		// Запись состояния генератора ДЛК
		os << squareAGenerator;
		os << endl;

		// Запись заголовка блока перетасовки
		os << moveSearchComponentHeader << endl;
		os << endl;

		// Запись статистических показателей
		os << pairsCount << " " << totalPairsCount << " " << totalSquaresWithPairs << endl;
		os << totalProcessedSquaresLarge << " " << totalProcessedSquaresSmall << endl;
		os << endl;
}


// Создание контрольной точки
void MovePairSearch::CreateCheckpoint()
{
	ofstream checkpointFile;

	checkpointFile.open(tempCheckpointFileName.c_str(), std::ios_base::out);
	if (checkpointFile.is_open())
	{
		Write(checkpointFile);
		checkpointFile.close();
		remove(checkpointFileName.c_str());
		rename(tempCheckpointFileName.c_str(), checkpointFileName.c_str());
	}
	else
		cerr << "Error opening checkpoint file!" << endl;
}


// Запуск поиска ортогональных квадратов методом перестановки строк
void MovePairSearch::StartMoveSearch()
{
	// Подписываемся на событие нахождения очередного ДЛК
	squareAGenerator.Subscribe(this); 

	// Запускаем генерацию ДЛК
	squareAGenerator.Start();

	// Отписываемся от события нахождения очередного ДЛК
	squareAGenerator.Unsubscribe();

	// Вывод итогов поиска
	ShowSearchTotals();
}


// Обработчик события построения ДЛК, запускающий поиск к нему пары
void MovePairSearch::OnSquareGenerated(Square& newSquare)
{
	// Запоминание найденного квадрата
	for (int i = 0; i < Rank; i++)
	{
		for (int j = 0; j < Rank; j++)
		{
			squareA[i][j] = newSquare.Matrix[i][j];
		}
	}

	// Запуск перетасовки строк
	MoveRows();

	// Проверка взаимной ортогональности квадратов
	if (pairsCount > 0)
	{
		CheckMutualOrthogonality();
	}

	// Собирание статистики по обработанным квадратам
	totalProcessedSquaresSmall++;

	// Фиксация информации о ходе обработки
	if (totalProcessedSquaresSmall % CheckpointInterval == 0)
	{
		// Обновить прогресс выполнения для клиента BOINC
		double fraction_done;
		if(Rank == 8)
			fraction_done = (double)(totalProcessedSquaresSmall)/5000000.0;
		else 
		{
			if(Rank == 9)
				fraction_done = (double)(totalProcessedSquaresSmall)/275000000.0;
			else
				fraction_done = 0.999999999;
		}

		if(fraction_done >=1) fraction_done = (double)(totalProcessedSquaresSmall)/300000000.0;
		if(fraction_done >=1) fraction_done = 0.999999999;

		boinc_fraction_done(fraction_done); // Сообщить клиенту BOINC о доле выполнения задания

		// Проверка, может ли клиент BOINC создать контрольную точку,
		// и если может, то запустить функцию её записи
		if (boinc_time_to_checkpoint())
		{
			CreateCheckpoint();
			boinc_checkpoint_completed(); // BOINC знает, что контрольная точка записана
		}	

		if(isDebug)
		{
			cout << "# ------------------------" << endl;
			cout << "# Processed " << totalProcessedSquaresLarge << " milliards and " << totalProcessedSquaresSmall << " squares." << endl;
			cout << "# Last processed square:" << endl;
			cout << endl;
			cout << newSquare;
			cout << "# ------------------------" << endl;
		}
	}
}


// Перетасовка строк заданного ДЛК в поиске ОДЛК к нему
void MovePairSearch::MoveRows()
{
	int currentRowId = 1;				// Номер обрабатываемой строки в формируемом квадрате
	int isRowGet = 0;					// Флаг нахождения строки для проверки в рамках комбинации
	int gettingRowId = -1;				// Номер строки из исходного квадрата, подставляемой в обрабатываемую строку формируемого квадрата
	int oldRowId = -1;					// Номер предыдущего вариант исходной строки строки формируемого квадрата, затираемой новым вариантом
	int isDifferent = 0;				// Флаг отличия найденного квадрата от исходного (первая комбинация получается совпадающей с исходным квадратом)

	int currentSquareRows[Rank];		// Массив с перечнем текущих строк, использованных в квадрате. На i-й позиции - номер используемой строки

	unsigned int primaryDiagonalFlags;	// Битовые флаги свободных значений на основной диагонали 
	unsigned int secondaryDiagonalFlags;// Битовые флаги свободных значений на побочной диагонали 
	unsigned int rowsUsageFlags;		// Битовые флаги задействования строк в текущей комбинации
	unsigned int rowsHistoryFlags[Rank];// Битовые флаги истории задействования строк в рамках текущей комбинации

	unsigned int freeRowsMask = 0;		// Маска из соединения флагов задействования строк и истории задействования строк исходного квадрата для текущей строки новой комбинации
	unsigned int testedRowsMask = 0;	// Маска с отмечанием уже проверенных строк исходного квадрата в рамках цикла поиска строки для этой комбинации
	unsigned long bitIndex = 0;			// Номер новой строки для проверки совместимости с комбинацией

	unsigned int isRowFree = 0;			// Флаг наличия "свободной" для проверки в комбинации строки, которую потом можно проверять на диагональность

	// Перебор различных комбинаций строк заданного ДЛК с целью поиска ОДЛК к нему
		// Сброс числа найденых пар для заданного ДЛК
		pairsCount = 0;

		// Сброс значений в векторах использования строк в очередной перестановке и номеров строк, использованных для текущего квадрата
		for (int i = 0; i < Rank; i++)
		{
			currentSquareRows[i] = -1;
		}

		// Выставление флагов возможности использования значений на диагоналях
		primaryDiagonalFlags = (1u << Rank) - 1;
		secondaryDiagonalFlags = (1u << Rank) - 1;
		// Сброс флагов задействовая строк в новой комбинации
		rowsUsageFlags = (1u << Rank) - 1;
		// Сброс истории задействования строк
		for (int i = 0; i < Rank; i++)
		{
			rowsHistoryFlags[i] = (1u << Rank) - 1;
		}

		// Выставляем флаги задействования первой (фиксированной) строки и её значений на диагоналях
		rowsUsageFlags &= ~1u;
		rowsHistoryFlags[0] &= ~1u;
		currentSquareRows[0] = 0;
		primaryDiagonalFlags &= ~(1u << squareA[0][0]);
		secondaryDiagonalFlags &= ~(1u << squareA[0][Rank - 1]);

		// Подбор всех возможных комбинаций из строк исходного квадрата
		while (currentRowId > 0)
		{
			// Очередной шаг в построении новой комбинации
				// Подбор строки из исходного квадрата на позицию currentRowId формирумого квадрата
				isRowGet = 0;
				gettingRowId = -1;
				testedRowsMask = (1u << Rank) - 1;
				
				// Поиск строки, не задействованной сейчас и ранее - для этой же строки
				do
				{
					// Проверка очередной строки на возможность добавления в новую комбинациию
						// Получение маски свободных строк
						freeRowsMask = rowsUsageFlags & rowsHistoryFlags[currentRowId] & testedRowsMask;
						// Получение номера свободной строки
						# ifdef _MSC_VER
						isRowFree = _BitScanForward(&bitIndex, freeRowsMask);
						# else
						bitIndex = __builtin_ffs(freeRowsMask);
						if (bitIndex > 0)
						{
						    isRowFree = 1;
						    bitIndex--;
						}
						else
						{
						    isRowFree = 0;
						}
						# endif

						// Обработка найденной строки
						if (isRowFree)
						{
							// Проверка соместимости найденной строки по диагоналям с уже выбранными в комбинацию строками
							if ((primaryDiagonalFlags & (1u << squareA[bitIndex][currentRowId])) && (secondaryDiagonalFlags & (1u << squareA[bitIndex][Rank - currentRowId - 1])))
							{
								isRowGet = 1;
								gettingRowId = bitIndex;
							}

							// Отмечание строки с номером bitIndex как уже проверенной
							testedRowsMask &= ~(1u << bitIndex);
						}
				}
				while (isRowFree && !isRowGet);
		
				// Обработка найденной строки 
				if (isRowGet)
				{
					// Добавление найденной строки в комбинацию строк формируемого квадрата
						// Считывание номера строки, уже присутствующей на этой позиции
						oldRowId = currentSquareRows[currentRowId];

						// Отмечание использования новой строки в массивах флагов
							// Сброс флага в массиве свободных/использованных строк
							rowsUsageFlags &= ~(1u << gettingRowId);
							// Сброс флага в массиве истории использования строк
							rowsHistoryFlags[currentRowId] &= ~(1u << gettingRowId);
							// Запись номера найденной строки в перечень строк новой комбинации
							currentSquareRows[currentRowId] = gettingRowId;
							// Отмечание диагональных значений из новой строки как использованных
							primaryDiagonalFlags &= ~(1u << squareA[gettingRowId][currentRowId]);
							secondaryDiagonalFlags &= ~(1u << squareA[gettingRowId][Rank - currentRowId - 1]);

						// Убирание флага занятости строки, убираемой из новой комбинации и значений её диагональных клеток
						if (oldRowId != -1)
						{
							rowsUsageFlags |= (1u << oldRowId);
							primaryDiagonalFlags |= (1u << squareA[oldRowId][currentRowId]);
							secondaryDiagonalFlags |= (1u << squareA[oldRowId][Rank - currentRowId - 1]);
						}

						// Обработка полученной комбинации
						if (currentRowId == Rank - 1)
						{
							// Обработка завершённой комбинации
								// Проверка комбинации на совпадение с исходной
								isDifferent = 0;
								for (int i = 0; i < Rank; i++)
								{
									if (currentSquareRows[i] != i)
									{
										isDifferent = 1;
										break;
									}
								}

								// Обработка комбинации
								if (isDifferent)
								{
									// Обработка новой комбинации
										// Формирование квадрата по найденной комбинации
										for (int i = 0; i < Rank; i++)
										{
											for (int j = 0; j < Rank; j++)
											{
												squareB[i][j] = squareA[currentSquareRows[i]][j];
											}
										}

										// Обработка полученного квадрата
										ProcessOrthoSquare();
								}
						}
						else
						{
							// Шаг вперёд - к следующей строк формируемого квадрата
							currentRowId++;
						}
				}
				else
				{
					// Шаг назад в поиске
						// Считывание номера строки исходного квадрата высвобождаемой, при уходе из текущей строки создаваемой комбинации
						oldRowId = currentSquareRows[currentRowId];
						// Отмечание текущей строки новой комбинации как незадействованной
						currentSquareRows[currentRowId] = -1;
						// Убирание флагов задействования строки из исходного квадрата и задействования её диагональных значений в новой комбинации
						if (oldRowId != -1)
						{
							rowsUsageFlags |= (1u << oldRowId);
							primaryDiagonalFlags |= (1u << squareA[oldRowId][currentRowId]);
							secondaryDiagonalFlags |= (1u << squareA[oldRowId][Rank - currentRowId - 1]);
						}
						// Очистка истории оставлямой строки комбинации
						rowsHistoryFlags[currentRowId] = (1u << Rank) - 1;
						// Переход к предыдущей строке создаваемой комбинации
						currentRowId--;
				}
		}
}


// Обработка найденного, возможно что ортогонального квадрата
void MovePairSearch::ProcessOrthoSquare()
{
	ofstream resultFile;				// Поток для I/O в файл с результатами

	Square a(squareA);				// Квадрат A как объект
	Square b(squareB);				// Квадрат B как объект

	int orthoMetric = Rank*Rank;	// Значение метрики ортогональности, говорящее о том, что квадраты - полностью ортогональны

	// Обработка найденного квадрата
	if (Square::OrthoDegree(a, b) == orthoMetric && b.IsDiagonal() && b.IsLatin() && a.IsDiagonal() && a.IsLatin())
	{
		// Запись информации о найденном квадрате
			// Увеличение счётчика квадратов
			pairsCount++;
			totalPairsCount++;

			// Запоминание базового квадрата
			if (pairsCount == 1)
			{
				orthoSquares[pairsCount - 1] = a;
				totalSquaresWithPairs++;
			}

			// Запоминание квадрата - пары
			if (pairsCount < OrhoSquaresCacheSize)
			{
				orthoSquares[pairsCount] = b;
			}

			// Вывод заголовка
			if (pairsCount == 1)
			{
				if(isDebug)
				{
					// Вывод информации о первом квадрате пары в виде заголовка
					cout << "{" << endl;
					cout << "# ------------------------" << endl;
					cout << "# Detected pair for the square: " << endl;
					cout << "# ------------------------" << endl;
					cout << a;
					cout << "# ------------------------" << endl;
				}
				// Вывод информации в файл
				resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
				if (resultFile.is_open())
				{
					resultFile << "{" << endl;
					resultFile << "# ------------------------" << endl;
					resultFile << "# Detected pair for the square: " << endl;
					resultFile << "# ------------------------" << endl;
					resultFile << a;
					resultFile << "# ------------------------" << endl;
					resultFile.close();
				}
				else
				{
					std::cerr << "Error opening file!";
				}
			}

			// Вывод информации о найденной паре
				if(isDebug)
				{
					// Вывод информации в консоль
					cout << b << endl;
				}

				// Вывод информации в файл
				resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
				if (resultFile.is_open())
				{ 
					resultFile << b << endl;
					resultFile.close();
				}
				else
				{
					std::cerr << "Error opening file!";
				}
	}
}


// Проверка взаимной ортогональности набора квадратов, найденного в текущем поиске
void MovePairSearch::CheckMutualOrthogonality()
{
	int orthoMetric = Rank*Rank;
	int maxSquareId;
	ofstream resultFile;

	// Определение верхней границы обрабатываемых квадратов
	if (pairsCount < OrhoSquaresCacheSize)
	{
		maxSquareId = pairsCount;
	}
	else
	{
		maxSquareId = OrhoSquaresCacheSize - 1;
	}

	// Открываем файл с результатами
	resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
	if (!resultFile.is_open()) { cout << "Error opening file!"; return; }

	// Проверка взаимной ортогональности набора квадратов
	for (int i = 0; i <= maxSquareId; i++)
	{
		for (int j = i + 1; j <= maxSquareId; j++)
		{
			if (Square::OrthoDegree(orthoSquares[i], orthoSquares[j]) == orthoMetric)
			{
				if(isDebug) cout << "# Square " << i << " # " << j << endl;
				resultFile << "# Square " << i << " # " << j << endl;
			}
		}
	}
	if(isDebug) cout << endl;
	resultFile << endl;

	// Выводим общее число найденых ОДЛК
	if(isDebug) cout << "# Pairs found: " << pairsCount << endl;
	resultFile << "# Pairs found: " << pairsCount << endl;

	// Ставим отметку об окончании секции результатов
	if(isDebug) cout << "}" << endl;
	resultFile << "}" << endl;

	// Закрываем файл с результатами
	resultFile.close();
}


void MovePairSearch::ShowSearchTotals()
{
	ofstream resultFile;

	if(isDebug)
	{
		// Вывод итогов в консоль
		cout << "# ------------------------" << endl;
		cout << "# Total pairs found: " << totalPairsCount << endl;
		cout << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
		cout << "# ------------------------" << endl;
	}

	// Вывод итогов в файл
	resultFile.open(resultFileName.c_str(), std::ios_base::binary | std::ios_base::app);
	if (resultFile.is_open())
	{
		resultFile << "# ------------------------" << endl;
		resultFile << "# Total pairs found: " << totalPairsCount << endl;
		resultFile << "# Total squares with pairs: " << totalSquaresWithPairs << endl;
		resultFile << "# Processes " << totalProcessedSquaresLarge << " milliards " << totalProcessedSquaresSmall << " squares" << endl;
		resultFile << "# ------------------------" << endl;
		resultFile.close();
	}
	else cerr << "Error opening file!" << endl;
}

﻿// Диагональный латинский квадрат

# if !defined Square_h
# define Square_h

# include <iostream>
# include <vector>

using namespace std;

class Square
{
public:
	static const int Rank = 9;					// Ранг квадрата
	static const int Empty = -1;				// Пустое, не заданное значение
	static const char HeadToken = '{';			// Символ начала информации о квадрате в потоке
	static const char TailToken = '}';			// Символ окончания информации о квадрате в потоке

	static int OrthoDegree(Square& a, Square& b);	// Степень ортогональности квадратов a и b

	Square();										// Конструктор по умолчанию
	Square(const int source[Rank][Rank]);			// Конструктор создания квадрата по матрице
	Square(const Square& source);					// Конструктор копирования

	int operator == (Square& value);										// Перегрузка оператора сравнения - сравниваются компоненты матрицы
	Square& operator = (Square& value);										// Перегрузка оператора присвоения
	friend std::ostream& operator << (std::ostream& os, Square& value);		// Перегрузка оператора вывода данных квадрата
	friend std::istream& operator >> (std::istream& is, Square& value);		// Перегрузка оператора считывания данных квадрата
	friend std::vector<vector<int>>& operator << (std::vector<vector<int>>& outvector, Square& value);	// Запись квадрата в вектор строк (также векторов)
	friend std::vector<vector<int>>& operator >> (std::vector<vector<int>>& invector, Square& value);	// Чтение квадрата из вектора строк (также векторов)

	int IsDiagonal();								// Проверка квадрата на то, что он является диагональным латинским квадратом
	int IsLatin();									// Проверка квадрата на то, что он является латинским квадратом
	void Initialize(const int source[Rank][Rank]);		// Инициализация компонентов квадрата
	void Reset();									// Сброс всех значеий перемененных
	void Read(std::istream& is);					// Чтение квадрата из потока
	void Write(std::ostream& os);					// Запись квадрата в поток

	int Matrix[Rank][Rank];							// Матрица квадрата

protected:
private:
};

# endif
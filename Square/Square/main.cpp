# include <iostream>
# include <fstream>
# include "Square.h"
# include "DLX_DLS.h"

using namespace std;

int main()
{
	/*int matrix[Square::Rank][Square::Rank] =
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
	}*/

	/*Square simpleSquare;
	fstream infile;
	infile.open("square.txt", std::ios_base::in);

	if (infile.is_open())
	{
		infile >> simpleSquare;
		infile.close();
	}*/

	
	/*// ��������� �� ������� �������
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
	infile.close();*/

	// 374064 edges

	/*const int FullOrthoDegree = Square::Rank*Square::Rank;
	const int SquaresCount = 61824;
	Square* squareList = new Square[SquaresCount];
	string infileName = "Graph-1.txt";
	string edgesFileName = "Edges-1.txt";
	fstream infile;
	fstream edgesFile;
	int edgesCount = 0;

	// ���������� ������ ���
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

	// �������� ����� ��� ������ ���� �����
	edgesFile.open(edgesFileName, std::ios_base::out);
	edgesFile << "IdA;IdB" << endl;

	// �������� ��������������� ���
	for (int i = 0; i < SquaresCount; i++)
	{
		// ����� ��������� ������������� i-�� ��������
		for (int j = i + 1; j < SquaresCount; j++)
		{
			// �������� ��������������� i-�� � j-�� ��������
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

	// ���������� ������
	edgesFile.close();
	delete[] squareList;

	cout << "Total edges count: " << edgesCount << endl;
	cout << "Press any key to continue ..." << endl;
	cin.get();*/

	//int squaresCount = 0;
	Square startSquare;						// �������, � �������� ���������� �����
	string infileName = "start_square.txt";	// ��� ����� � ��������� ���������
	fstream infile;							// ���� � ��������� ���������
	string squaresSetFileName = "squares_set.txt";	// ��� ����� � ������� ��������� ���������
	fstream setfile;						// ���� � ������� ��������� ���������

	orth_mate_search finder;				// �������� ����

	vector<vector<int>> startSquareVector;	// �������, � �������� ���������� �����
	vector<vector<vector<int>>> result;		// ��������� ������
	vector<Square> squaresSet;				// ��������� ��������� ���������
	vector<Square> iterationSet;			// ��������� ���������, ��������� � ������ ��������
	Square newSquare;						// �������, ����������� � ���������
	int isNewSquare = 0;					// ���� ������ ��������
	int newSquaresInIteration = 0;			// ����� ����� ���������, ��������� � ������ ��������� ��������

	const int FullOrthoDegree = Square::Rank*Square::Rank;
	string edgesFileName = "squares_edges.txt";
	fstream edgesFile;
	int edgesCount = 0;

	// ���������� ������ ���
	infile.open(infileName, std::ios_base::in);

	if (infile.is_open())
	{
		infile >> startSquare;
	}

	infile.close();

	// ����� ��������� �� ������������� ���������, ������������ � ���������
	/*startSquareVector.resize(Square::Rank);

	for (int rowId = 0; rowId < Square::Rank; rowId++)
	{
		startSquareVector[rowId].resize(Square::Rank);

		for (int columnId = 0; columnId < Square::Rank; columnId++)
		{
			startSquareVector[rowId][columnId] = startSquare.Matrix[rowId][columnId];
		}
	}*/

	startSquareVector << startSquare;

	// ��������� ������ ������� � ���������
	squaresSet.clear();
	squaresSet.push_back(startSquare);

	// ����� ����� ���������, ������������� � ��� ���������
	cout << "Starting to search an orthogonal squares ..." << endl;

	do
	{
		iterationSet.clear();
		newSquaresInIteration = 0;

		// ����� ���������, ������������� ��������� ��������� ���������
		for (int squareId = 0; squareId < squaresSet.size(); squareId++)
		{
			startSquareVector << squaresSet[squareId];
			result.clear();
			finder.check_dlx_rc1(startSquareVector, result);
	
			for (int resultItemId = 0; resultItemId < result.size(); resultItemId++)
			{
				result[resultItemId] >> newSquare;
				iterationSet.push_back(newSquare);
			}
		}

		// �������� ��������� ��������� �� ������������ � ���������� ����� ��������� � �������� ���������
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
				newSquaresInIteration++;
				cout << iterationSet[i] << endl;
			}
		}

		cout << "Found " << newSquaresInIteration << " new squares " << endl;
		cout << "Total squares: " << squaresSet.size() << endl;
	}
	while (newSquaresInIteration > 0);

	// ������ ��������� ��������� � ����
	cout << "Save squares set ..." << endl;

	setfile.open(squaresSetFileName, std::ios_base::out);

	if (setfile.is_open())
	{
		for (int i = 0; i < squaresSet.size(); i++)
		{
			setfile << squaresSet[i];
		}
	}

	setfile.close();


	cout << "Start to count graph edges..." << endl;

	// �������� ����� ��� ������ ���� �����
	edgesFile.open(edgesFileName, std::ios_base::out);
	edgesFile << "IdA;IdB" << endl;

	// �������� ��������������� ���
	for (int i = 0; i < squaresSet.size(); i++)
	{
		// ����� ��������� ������������� i-�� ��������
		for (int j = i + 1; j < squaresSet.size(); j++)
		{
			// �������� ��������������� i-�� � j-�� ��������
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

	// ���������� ������
	edgesFile.close();

	cout << "Total edges count: " << edgesCount << endl;
	cout << "Press any key to continue ..." << endl;
	cin.get(); 

	return 0;
}
# include <iostream>
# include <fstream>
# include <string>
# include <chrono>
# include <thread>
# include <Windows.h>

# include "MovePairSearch.h"
# include "PairSearch.h"

using namespace std;


// ��������� ������� ����� �� �������� �����
string GetFirstFile(string mask)
{
	string result;

	WIN32_FIND_DATA findData;	// ������, ��������� � ������� (����� ������������ � FindNextFile)
	HANDLE findHandle;			// ��������� ������

	findHandle = FindFirstFile(mask.c_str(), &findData);
	
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		result = findData.cFileName;
	}
	else
	{
		result.clear();
	}

return result;
}


// �������� ����������� storage
int PingStorage(string path)
{
	int result = 0;
	fstream pingFile;

	pingFile.open(path, std::ios_base::in, _SH_DENYNO);
	
	if (pingFile.is_open())
	{
		result = 1;
	}

return result;
}


// ���������� ����������
void Compute(string storage, string local)
{
	string workunitsDirectory = storage + "workunit\\";
	string resultsDirectory = storage + "result\\";
	
	string workunitsMask = "wu*.txt";
	string resultsMask = "rs*.txt";

	string pingPath = storage + "ping\\ping.txt";
	string semaphorePath = storage + "semaphore\\semaphore.txt";
	string storageWorkunitsPath = workunitsDirectory + workunitsMask;

	string localWorkunitsMask = local + workunitsMask;
	string localResultsMask = local + resultsMask;
	string localCheckpointMask = local + "checkpoint.txt";

	string localWorkunit;
	string localResult;
	string localCheckpoint;
	string pathLocalWorkunit;
	string pathLocalResult;
	string pathLocalCheckpoint;
	string pathStorageResult;
	string pathStorageWorkunit;
	string storageWorkunit;

	string initStartFileName;
	string initResultFileName;
	string initCheckpointFileName = local + "checkpoint.txt";
	string initTempCheckpointFileName = local + "checkpoint_new.txt";

	int isStorageResolved;
	int isResultSent;

	fstream semaphoreFile;
	WIN32_FIND_DATA fileListData;
	HANDLE fileListHandle;

	MovePairSearch search;

	for ( ; ; )
	{
		// �������� ������� ����� �������, ����������� �����, ����������
		localWorkunit = GetFirstFile(localWorkunitsMask);
		localResult = GetFirstFile(localResultsMask);
		localCheckpoint = GetFirstFile(localCheckpointMask);

		pathLocalWorkunit = local + localWorkunit;
		pathLocalResult = local + localResult;
		pathLocalCheckpoint = local + localCheckpoint;
		pathStorageResult = resultsDirectory + localResult;

		// �������� �����������
		if (!localResult.empty())
		{
			// �������� ���������� � �������� ����� � �������� � ����������� ������
				// �������� ����� � ��������
				std::cout << "Remove a workunit file: " << localWorkunit << endl;
				remove(pathLocalWorkunit.c_str());

				// �������� ����� � ����������� ������
				std::cout << "Remove a checkpoint file: " << localCheckpoint << endl;
				remove(pathLocalCheckpoint.c_str());

				// �������� ����������
					// ����� ������
					isResultSent = 0;
					isStorageResolved = 0;
					// ����������� ����������
					do
					{
						// ������������� ������� ��������
							//  ��������� ����������� ���������
							isStorageResolved = PingStorage(pingPath);
							// ��������� ����
							if (isStorageResolved)
							{
								// ��������� ���� � ��������� ������� �������
								std::cout << "Move result " << localResult << " to storage on " << resultsDirectory << endl;
								rename(pathLocalResult.c_str(), pathStorageResult.c_str());
								isResultSent = 1;
							}
							else
							{
								cout << "Storage inaccessble, result " << localResult << " cannot be sent. Waiting 5 minutes..." << endl;
								std::this_thread::sleep_for(std::chrono::minutes(5));
							}
					}
					while (!isResultSent);
		}

		// ������ ���������� � ����������� �����
		if (!localCheckpoint.empty() && localResult.empty())
		{
			// �������� ������� ����� � ��������
			if (!localWorkunit.empty())
			{
				// ��������� ������
				initStartFileName = local + localWorkunit;
				initResultFileName = local + "rs" + localWorkunit.substr(2, localWorkunit.length() - 2);
				
				std::cout << "Start from checkpoint of workunit " << localWorkunit << endl;

				search.InitializeMoveSearch(initStartFileName, initResultFileName, initCheckpointFileName, initTempCheckpointFileName);
				search.StartMoveSearch();
			}
			else
			{
				std::cout << "Error: delected a checkpoint file " << localCheckpoint << " without workunit file!" << endl;
			}
		}

		// ������ ���������� � ����� �������
		if (localCheckpoint.empty() && localResult.empty() && !localWorkunit.empty())
		{
			// ������ ���������� � ����� �������, ��������������� ��� ������ ����������� ����� � ����������
			initStartFileName = local + localWorkunit;
			initResultFileName = local + "rs" + localWorkunit.substr(2, localWorkunit.length() - 2);
			
			std::cout << "Start from workunit file " << localWorkunit << endl;

			search.InitializeMoveSearch(initStartFileName, initResultFileName, initCheckpointFileName, initTempCheckpointFileName);
			search.StartMoveSearch();
		}

		// ������ ������ �������
		if (localCheckpoint.empty() && localResult.empty() && localWorkunit.empty())
		{
			// ��������� ������ �������
				//  ��������� ����������� ���������
				while(!PingStorage(pingPath))
				{
					cout << "Storage inaccessble, new workunits cannot be received. Waiting 5 minutes..." << endl;
					std::this_thread::sleep_for(std::chrono::minutes(5));
				}

				// ������ ��������
				semaphoreFile.open(semaphorePath, std::ios_base::app, _SH_DENYRW);
				while (!semaphoreFile.is_open())
				{
					std::this_thread::sleep_for(std::chrono::seconds(2));
					semaphoreFile.open(semaphorePath, std::ios_base::app, _SH_DENYRW);
				}

				// ����� ����� � ��������
				fileListHandle = FindFirstFile(storageWorkunitsPath.c_str(), &fileListData);
				if (fileListHandle != INVALID_HANDLE_VALUE)
				{
					storageWorkunit = fileListData.cFileName;
					pathStorageWorkunit = workunitsDirectory + storageWorkunit;
					pathLocalWorkunit = local + storageWorkunit;

					cout << "Catch a WU: " << storageWorkunit << endl;
					rename(pathStorageWorkunit.c_str(), pathLocalWorkunit.c_str());
				}
				else
				{
					cout << "Could not find workunits by '" << workunitsMask << "' mask" << endl;
					break;
				}

				semaphoreFile.close();
		}
	}
}


int main(int argumentsCount, char* argumentsValues[])
{
	string storage;
	string local;

	if (argumentsCount == 3)
	{
		storage = argumentsValues[1];
		local = argumentsValues[2];

		Compute(storage, local);
	}
	else
	{
		std::cout << "Please, specify: 1) storage path; 2) local path." << endl << endl;
	}

	cout << "Press any key to exit ... " << endl;
	cin.get();

	return 0;
}

/*
Sample of Pair search

	int aMatrix[9][9] =
	{
		{0, 1, 2, 3, 4, 5, 6, 7, 8},
		{4, 2, 7, 6, 8, 1, 3, 5, 0},
		{3, 5, 1, 0, 7, 8, 4, 6, 2},
		{6, 3, 8, 4, 1, 7, 0, 2, 5},
		{5, 6, 0, 7, 3, 2, 8, 4, 1},
		{7, 8, 4, 1, 5, 6, 2, 0, 3},
		{8, 7, 6, 2, 0, 3, 5, 1, 4},
		{1, 0, 3, 5, 2, 4, 7, 8, 6},
		{2, 4, 5, 8, 6, 0, 1, 3, 7}
	};

	Square a(aMatrix);
	PairSearch search;

	search.Initialize("start_parameters.txt", "result.txt", "checkpoint.txt", "checkpoint_new.txt");
	search.OnSquareGenerated(a);

*/
// Генерация заданий для поиска "граблением"

# include <stdlib.h>
# include <iostream>
# include <string>
# include "Square.h"
# include "Generator.h"

int main(int argsCount, char* argsValues[])
{
        // Set the default values of variables
        int workunitsCount = 1;
        string workunitsDirectory = "";

        // Define the start, checkpoint and temporary checkpoint file name
        string startFileName = "start_parameters.txt";
        string checkpointFileName = "checkpoint.txt";
        string tempCheckpointFileName = "checkpoint_new.txt";

        // Variables for arguments parsing
        string argument;                            // Processed argument name
        string paramName;                           // Parsed parameter name
        string paramValue;                          // Parsed parameter value
        string countParamName = "--count";          // Parameter name for workunits count
        string directoryParamName = "--directory";  // Parameter name for workunits place directory
        string delimiter = "=";                     // Delimiter for split parameter name and value in argument
        size_t delimeterPosition = -1;              // Reset the value of deimeter position

        // Read the input parameters
        if (argsCount > 0)
        {
            for (int i = 0; i < argsCount; i++)
            {
                // Process the i-th agument
                    // Read the parameter name and value from argument
                    argument = argsValues[i];
                    delimeterPosition = argument.find(delimiter, 0);

                    if (delimeterPosition != string::npos)
                    {
                        paramName = argument.substr(0, delimeterPosition);
                        paramValue = argument.substr(delimeterPosition + 1);
                    }

                    // Process the readed parameter
                    if (paramName == countParamName)
                    {
                        workunitsCount = atoi(paramValue.c_str());
                    }

                    if (paramName == directoryParamName)
                    {
                        workunitsDirectory = paramValue;
                    }
            }
        }

        // Explain parsed parameters
        cout << "Workunits to generate: " << workunitsCount << endl;
        cout << "Workunits directory: " << workunitsDirectory << endl;

        Generator wuGenerator;
        wuGenerator.GenerateWorkunits(startFileName, workunitsDirectory, checkpointFileName, tempCheckpointFileName, workunitsCount);

        cout << "Workunits generation complete!" << endl;
		
        return 0;
}
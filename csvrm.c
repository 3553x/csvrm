#include <stdio.h>
#include <stdlib.h>

void printUsage(char *[]);
int* orderCols(int *, int);
int* parseArgs(char *[], int);
void invVal(void);

int main(int argc, char *argv[])
{
	FILE* inputFile;
	FILE* tmpFile;
	/*number of columns that are to be deleted*/
	int colNum = argc - 2;
	int* ordrdCols;
	int* args;

	if (argc < 3)
	{
		printUsage(argv);
		return 1;
	}
	args = parseArgs(argv, colNum);
	ordrdCols = orderCols(args, colNum); 
	if ((inputFile = fopen(argv[1], "r")) == NULL)
	{
		perror("ERROR while opening CSV-file");
		exit(1);
	}
	if ((tmpFile = tmpfile()) == NULL)
	{
		perror("ERROR while creating temporary file");
		exit(1);
	}
	
	int escape = 0;
	int valueCount = 0;
	int argn = 0;

	do
	{
		char c = getc(inputFile);
		switch (c)
		{
			case '\n':
				if (valueCount + 1 < *(ordrdCols + colNum - 1))
				{
					/*user tried to delete a column that is not present */
					perror("Invalid CSV format or invalid columns");
					exit(1);
				}
				argn = escape = valueCount = 0;
				break;
			case '"':
				/*ghetto binary switch*/
				escape += 1;
				escape %= 2;
				break;
			case ',':
				if (!escape)
					valueCount += 1;	
				break;
		}
		if (c != EOF)
		{
			/*checks if current value should be deleted, doesn't delete newline*/
			if (valueCount == *(ordrdCols + argn) - 1 && c != '\n')
				continue;
			else if (valueCount == *(ordrdCols + argn)){
				if (argn + 1 < colNum)
					argn += 1;
				if (valueCount + 1 == *(ordrdCols + argn))
					continue;
			}
			/*removes leading , after deleteing first column*/
			if (*ordrdCols == 1 && c == ',' && valueCount == 1)
				continue;
			putc(c, tmpFile);
		}
	} while (!feof(inputFile));

	fclose(inputFile);
	rewind(tmpFile);
	inputFile = fopen(argv[1], "w");
	char a;
	while ((a = getc(tmpFile)) != EOF)
		putc(a, inputFile);
	fclose(inputFile);
	fclose(tmpFile);
	exit(0);
}

void printUsage(char *argv[])
{
	printf("Usage: %s FILE colnumber1 [colnumber2] ...\n", argv[0]);
}

int* orderCols(int *inputCols, int colNum)
{
	int *orderedCols = malloc(colNum * sizeof(int));
	int colsToSort = colNum;

	while (colsToSort != 0)
	{
		int indexOfBiggestValue = 0;
		for (int i = 0; i < colNum; i++)
		{
			if (*(inputCols + indexOfBiggestValue) < *(inputCols + i))
				indexOfBiggestValue = i;
		}
		/*the ordered columns are ascending we start with the biggest values*/
		*(orderedCols + --colsToSort) = *(inputCols + indexOfBiggestValue);
		*(inputCols + indexOfBiggestValue) = 0;
	}
	return orderedCols;
}

int* parseArgs(char* argv[], int colNum)
{
	int* parsedArgs = malloc(colNum * sizeof(int));

	for (int i = 0; i < colNum; i++)
	{
		if (*(parsedArgs + i) = atoi(argv[2 + i]) < 1)
			invVal();
		for (int j = 1; i >= j; j++)
		{
			if (*(parsedArgs + i - j) == *(parsedArgs + i))
				invVal();
		}
	}
	return parsedArgs;	
}

void invVal(void)
{
	perror("Invalid or duplicate colnumber");
	exit(1);
}

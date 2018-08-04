/* To compile and run with gcc:

 gcc SudokuSolver.c -o Sudoku.exe && ./Sudoku.exe
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int Board[9][9];
	int Possibilities[9][9][9];
	int Options[9][9];
	int Zeros;
} Sudoku;

Sudoku* SudokuRead();

Sudoku* SudokuCopy(Sudoku*);

int AddNewNums(Sudoku*);

int Check(Sudoku*);

Sudoku* Iterate(Sudoku*);

int AddNumber(Sudoku*, int, int, int);

int Guess(Sudoku*, int, int);

void PrintSudoku(Sudoku*);





int main()
{
	Sudoku* Puzzle = SudokuRead();
	
	printf("\n\nStarting Puzzle:");
	PrintSudoku(Puzzle);
	Puzzle = Iterate(Puzzle);
}



Sudoku* SudokuCopy(Sudoku* Source)
{
	Sudoku* Copy = malloc(sizeof(Sudoku));
	if (Copy != NULL)
	{
		memcpy(Copy, Source, sizeof(Sudoku));
	}
	return Copy;
}


int Check(Sudoku* S)
{
	int a, b;
	
	//If puzzle is complete, return 1
	if (S->Zeros == 0)
	{
		return 1;
	}
	
	//If incomplete, find if valid
	for (a = 0; a < 9; a++)
	{
		for (b = 0; b < 9; b++)
		{
			if (S->Board[a][b] == 0)
			{
				if (S->Options[a][b] == 0)
				{
					//Return -1 if cannot be solved
					return -1;
				}
			}
		}
	}
	//Otherwise, return 0 to proceed
	return 0;
}


int AddNumber(Sudoku* S, int x, int y, int num)
{
	int i, j, L, H, x_sub, y_sub;
	
	//Reduce number of zeros by 1
	S->Zeros = S->Zeros - 1;
	
	//Set number to space in puzzle
	S->Board[x][y] = num;
	
	//Set options for that space to 0
	S->Options[x][y] = 0;
	
	//Find Block
	L = x / 3;
	H = y / 3;
	
	//Iterate over block
	for (i = 0; i< 3; i++)
	{
		x_sub = 3 * L + i;
		
		for (j = 0; j < 3; j++)
		{
			y_sub = 3 * H + j;
			
			//Set this number to not possible in block, if not already
			if (S->Possibilities[num - 1][x_sub][y_sub] != 0)
			{
				S->Possibilities[num - 1][x_sub][y_sub] = 0;
				
				//Then reduce options of block spaces by one, if unoccupied 
				if (S->Options[x_sub][y_sub] > 0)
				{
					S->Options[x_sub][y_sub] = S->Options[x_sub][y_sub] - 1;
				}	
			}
		}
	}

	//Iterate over row/column
	for (i = 0; i < 9; i++)
	{
		//Set given number not possible in column
		if (S->Possibilities[num - 1][x][i] != 0)
		{
			S->Possibilities[num - 1][x][i] = 0;
			
			//Reduce options accordingly
			if (S->Options[x][i] > 0)
			{
				S->Options[x][i] = S->Options[x][i] - 1;
			}	
		}
		
		//Repeat for row
		if (S->Possibilities[num - 1][i][y] != 0)
		{
			S->Possibilities[num - 1][i][y] = 0;
			
			if (S->Options[i][y] > 0)
			{
				S->Options[i][y] = S->Options[i][y] - 1;
			}
		}
		
		//No other numbers are possible at the occupied space
		S->Possibilities[i][x][y] = 0;
	}
	return num;	
}

Sudoku* SudokuRead()
{
	int i, j, k;
	int NumToAdd = 0;
	
	Sudoku* P = NULL;
	
	FILE* in = NULL;
	
	in = fopen("Puzzle.txt", "r");
	
	if (in == NULL)
	{
		printf("\n\nFile 'Puzzle.txt' is not found.\n\n");
		exit(0);
	}

	//Allocate memory for puzzle
	P = malloc(sizeof(Sudoku));
	if (P == NULL)
	{
		printf("\n\nMalloc Error.\n\n");
		exit(0);
	}
	
	//set all spaces to all possible
	P->Zeros = 81;
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			P->Options[i][j] = 9;
			for (k = 0; k < 9; k++)
			{
				P->Possibilities[i][j][k] = 1;
			}
		}
	}	
	
	//Read in values from file
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			fscanf(in, "%d", &NumToAdd);
			if (NumToAdd > 0)
			{
				AddNumber(P, i, j, NumToAdd);
			}
		}
	}
	fclose(in);
	return P;		
}	


int AddNewNums(Sudoku* S)
{
	int i, j, k;
	int Replaced = 0;
	
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (S->Options[i][j] == 1)
			{
				for (k = 0; k < 9; k++)
				{
					if (S->Possibilities[k][i][j] == 1)
					{
						AddNumber(S, i, j, (k + 1));
						Replaced++;
					}
				}
			}
		}
	}	
	return Replaced;
}

Sudoku* Iterate(Sudoku* S)
{
	int NumsAdded = 0;
	int Status = 0;
	int i;
	int n_guesses = 2;
	int GuessedNumber;
	Sudoku* Possibilities[9];
	
	for (i = 0; i < 9; i++)
	{
		Possibilities[i] = NULL;
	}

	while (AddNewNums(S))
	{
		printf("\n\nAdded numbers. Progress:");
		PrintSudoku(S);
	}
	
	Status = Check(S);
	
	if (Status == 1)
	{
		return S;
	}
	else if (Status == -1)
	{
		printf("\n\nBad Solution:");
		PrintSudoku(S);
		
		free(S);
		printf("\n\nNow working with another option for previous guess.");
		return NULL;	
	}
	else
	{
		for (i = 0; i < n_guesses; i++)
		{
			if (Possibilities[i] == NULL)
			{
				Possibilities[i] = SudokuCopy(S);
			}
			
			if (Guess(Possibilities[i], i, n_guesses) == 0)
			{
				n_guesses++;
				i = 0;
			}	
		}
		
		
		printf("\n\nNeeded to guess. %d possibilities:", n_guesses);
		for (i = 0; i < n_guesses; i++)
		{
			PrintSudoku(Possibilities[i]);
		}
		
		
		free(S);
		
		for (i = 0; i < n_guesses; i++)
		{
			if(Iterate(Possibilities[i]) != NULL)
			{
				printf("\n\nSolution found! Result:");
				PrintSudoku(Possibilities[i]);
				exit(0);
			}
		}
		printf("\n\nSomething bad happened. A solution was not found. :(\n\n");
		return NULL;
	}
}


void PrintSudoku(Sudoku* S)
{
	int i, j;
	for (i = 0; i < 9; i++)
	{
		printf("\n");
		if (((i % 3) == 0) && (i > 0))
		{
			printf("------+-------+------\n");
		}
		for (j = 0; j < 9; j++)
		{
			printf("%d ", S->Board[i][j]);
			if ((j == 2) || (j == 5))
			printf("| ");
		}
	}
	printf("\n%d spaces remaining.", S->Zeros);
	printf("\n\n");
}


int Guess(Sudoku* S, int a, int n)
{
	int i, j, k;
	int Replaced = 0;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (S->Options[i][j] == n)
			{
				for (k = 0; k < 9; k++)
				{
					if (S->Possibilities[k][i][j] == 1)
					{
						if (a == 0)
						{
							AddNumber(S, i, j, (k + 1));
							return (k + 1);
						}
						else
						{
							a--;
						}
					}
				}
			}
		}
	}
	return 0;
}	


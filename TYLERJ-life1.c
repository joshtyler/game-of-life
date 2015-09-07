/*
	life1.c v1.0
	Created by Joshua Tyler (URN:6213642)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOARD_WIDTH 78
#define MAX_BOARD_HEIGHT 50

/* Structure to hold co-ordinates of a point. */
typedef struct
{
	int column;
	int row;
} coord;

/* Stores the width and height of all the boards used in the program. */
int boardWidth, boardHeight;

/* Function prototypes, function descriptions can be found with the function definitions. */
int readFileToBoard(const char* fileName, char (*boardToWrite)[boardWidth]);
void printBoard(char (*boardToRead)[boardWidth]);
void printBorderRow(void);
void iterateBoard(char (*boardToRead)[boardWidth], char (*boardToWrite)[boardWidth]);
int numberOfNeighbours(char (*board)[boardWidth], coord current);

/*
	Function: main()
	Purpose: Iterate through generations of an initial game of life state provided by the user.
	Arguments: The file containing the initial configuration,
	           the width and height of the game of life board,
	           and the number of generations to iterate the board through.
	Return value: EXIT_SUCCESS if the program completes successfully,
	              EXIT_FAILURE if there is a problem in program execution.
	Inputs from user: None.
	Outputs to user: The game of life board at each generation it is calculated.
	                 "Finished" once the program has executed.
	                 Error messages if any of the information given to the program is invalid.
 */
int main(int argc, char* argv[])
{
	if(argc != 5)
	{
		fprintf(stderr, "Invalid arguments.\n"
		                "Usage: %s <file-name containing initial data> <width> <height> <no. of generations to calculate>\n"
		                "The program will now exit.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Read the width, height and number of generations arguments into integers */
	extern int boardWidth, boardHeight;
	int noOfGenerations;

	/* Set the integers to -1 before reading them from the function arguments, to ensure erroneous input is detected. */
	boardWidth = boardHeight = noOfGenerations = -1;

	sscanf(argv[2], "%d", &boardWidth);
	if( (boardWidth < 0) || (boardWidth > MAX_BOARD_WIDTH) )
	{
		fprintf(stderr, "Invalid board width.\n"
		                "Please ensure that board width is an integer greater than or equal to zero, and less than or equal to %d.\n"
		                "The program will now exit.\n", MAX_BOARD_WIDTH);
		exit(EXIT_FAILURE);
	}

	sscanf(argv[3], "%d", &boardHeight);
	if( (boardHeight < 0) || (boardHeight > MAX_BOARD_HEIGHT) )
	{
		fprintf(stderr, "Invalid board height.\n"
		                "Please ensure that board height is an integer greater than or equal to zero, and less than or equal to %d.\n"
		                "The program will now exit.\n", MAX_BOARD_HEIGHT);
		exit(EXIT_FAILURE);
	}

	sscanf(argv[4], "%d", &noOfGenerations);
	if(noOfGenerations < 0)
	{
		fputs("Invalid no. of generations to calculate.\n"
		      "Please ensure that The no. of generations to calculate is an integer greater than or equal tozero.\n"
		      "The program will now exit.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* 2D array to represent boards
	   Two boards are necessary so that the rules can be applied to one board, and the results saved in another.
	   This method allows the rules to be applied to each cell simultaneously.
	   Additionally, a temporary pointer is necessary so that the two boards can be swapped. */
	char (*currentBoard)[boardWidth];
	char (*nextBoard)[boardWidth];
	char (*tempBoardptr)[boardWidth];
	currentBoard = ( char (*)[boardWidth] )malloc(boardWidth * boardHeight * sizeof(char));
	nextBoard = ( char (*)[boardWidth] )malloc(boardWidth * boardHeight * sizeof(char));

		if( (currentBoard == NULL) || (nextBoard == NULL) )
		{
			fputs("Memory allocation error.\n"
			      "The program will now exit.\n", stderr);
		}

	/* Initialise currentBoard to be full of dead cells (spaces) */
	memset(currentBoard, ' ', boardWidth * boardHeight * sizeof(char));

	/* Read the initial live cells to the current board. */
	if(!readFileToBoard(argv[1], currentBoard))
	{
		fputs("The program will now exit.\n", stderr);
		exit(EXIT_FAILURE);
	}


	/* Loop to iterate the board and print it out for the number of generations specified. */
	int i;
	for(i = 0; i <= noOfGenerations; i++)
	{
		/* Print the current board to stdout. */
		printBoard(currentBoard);

		/* Iterate currentBoard, saving the results in nextBoard.
		   It is not necessary to initialise nextBoard to be the same as currentBoard because iterateBoard() writes to every used cell. */
		iterateBoard(currentBoard, nextBoard);

		/* Swap the two boards over, so that the current board is the one that has just had the iterations saved in it */
		tempBoardptr = currentBoard;
		currentBoard = nextBoard;
		nextBoard = tempBoardptr;
	}
		

	/* After the for loop, we are finished */
	puts("Finished");
	free(currentBoard);
	free(nextBoard);
	return EXIT_SUCCESS;
}

/*
	Function: readFileToCurrentBoard()
	Purpose: Read the initial board configuration from a file, and save the live cells in the correct place on a board.
	Arguments: The filename of the configuration file (fileName)
	           A pointer to the board array to write the live cells to (boardToWrite)
	Return value: 1 upon successful reading.
	              0 upon unsuccessful reading
	Inputs from user: None.
	Outputs to user: Error messages if there if there is a problem with the file.
	Note: As per the specification, this function does no error checking on the format of the input file,
	      other than ensuring the input file exists and ensuring the co-ordinates are within the defined board.
	      So please ensure that any input files are carefully formatted.
 */
int readFileToBoard(const char* fileName, char (*boardToWrite)[boardWidth])
{
	/* Attempt to open the file */
	FILE *inputFilePointer;
	inputFilePointer = fopen(fileName, "r");

	/* If the inputFilePointer is NULL, the opening must have failed */
	if(inputFilePointer == NULL)
	{
		fputs("Error opening board configuration file.\n"
		      "Please ensure that the file exists.\n", stderr);
		return 0;
	}

	/* The first integer in the file is the number of coordinates that that file contains. */
	int noOfCoordsToRead;
	fscanf(inputFilePointer, "%d", &noOfCoordsToRead);

	/* Now loop through the file until we've read the specifed no. of co-ordinates,
	   read the each co-ordinate from the file,
	   and save it in the appropriate place on the board. */
	coord currentPoint;
	for(; noOfCoordsToRead > 0; noOfCoordsToRead--)
	{
		/* Read the co-ordinates from the file */
		fscanf(inputFilePointer, "%d%d", &currentPoint.row, &currentPoint.column);

		/* Ensure that the co-ordiates are within the defined range. Return 0 (the error return) if they're not */
		if( (currentPoint.row >= boardHeight) || (currentPoint.column >= boardWidth) )
		{
			fputs("Co-ordinate outside board dimensions.\n", stderr);
			return 0;
		}

		/* If the defined co-ordinates are correct, define each one as a live cell. */
		boardToWrite[currentPoint.row][currentPoint.column] = 'X';
	}

	/* Close the input file and return the "success" return. */
	fclose(inputFilePointer);
	return 1;
}

/*
	Function: printBoard()
	Purpose: Print the current iteration of the board to stdout.
	Arguments: None.
	Return value: None.
	Inputs from user: None.
	Outputs to user: The board that is printed.
 */
void printBoard(char (*boardToRead)[boardWidth])
{
	/* Call printBorderRow(), to print the top border of the grid. */
	printBorderRow();

	/* Loop through and print out each row in turn. */
	int i;
	for(i=0; i < boardHeight; i++)
		/* Each row is stored as an array of characters, so can be printed out as a strign with defined length boardWidth.
		   Pipes are put on the beginning and end of the string to create the border.*/
		printf("|%.*s|\n", boardWidth, boardToRead[i]);

	/* Call printBorderRow() again, to print the bottom border */
	printBorderRow();

	/* Put two blank lines after the board is printed, as specified. */
	for(i=1; i<=2; i++)
	putchar('\n');

	return;
}

/*
	Function: printBorderRow()
	Purpose: Print a "border row" (the border that appears at the top or bottom of each printed board) to stdout.
	Arguments: None.
	Return value: None.
	Inputs from user: None.
	Outputs to user: The border row that is printed to stdout.
 */
void printBorderRow(void)
{
	/*Put a '*' to the the left corner of the border */
	putchar('*');

	/* Now we need as many '-'s as there are columns in the board */
	int i;
	for(i=0; i < boardWidth; i++)
		putchar('-');

	/* Now we need a * for the top right hand corner */
	putchar('*');

	/* Then finally a new line character so that the next output goes to a blank line. */
	putchar('\n');

	return;
}

/*
	Function: iterateBoard()
	Purpose: Perform the logical operations on each and every cell in a board, to see if it will live or die in the next state.
	         Then write these changes to a different board.
	Arguments: The board to read the current state from (boardToRead), and the board to write the next state to (boardToWrite).
	Return value: None.
	Inputs from user: None.
	Outputs to user: None.
*/
void iterateBoard(char (*boardToRead)[boardWidth], char (*boardToWrite)[boardWidth])
{

	/* Since we will be looping through each cell of the board, we will use a coord structure as our loop counter */
	coord counter;

	/* Outside loop loops through the rows */
	for(counter.row = 0; counter.row < boardHeight; counter.row++)
		/* Inside loop loops through the columns */
		for(counter.column = 0; counter.column < boardWidth; counter.column++)
			/* numberOfNeighbours() returns the number of live neighbours that a cell has.
			   We can use this with a switch to create different outcomes each possible number of neighbours */
			switch( numberOfNeighbours(boardToRead, counter) )
			{
				/* 0, 1 or 4+ (i.e default: ) neighbours means the cell will need to be dead */
				case 0: case 1: default:
					boardToWrite[counter.row][counter.column] = ' ';
					break;

				/* 2 neighbours means that the cell continues in its current state */
				case 2:
					boardToWrite[counter.row][counter.column] = boardToRead[counter.row][counter.column];
					break;

				/* 3 neighbours means the cell will be alive */
				case 3:
					boardToWrite[counter.row][counter.column] = 'X';
					break;
			}

	return;

}

/*
	Function: numberOfNeighbours()
	Purpose: Calculate the number of live neighbours that a cell has, and return this number
	Arguments: The board containing the cell whose neighbours need checking (board), and the co-ordinates of the cell the check (current).
	Return value: An integer contianing the number of live neighbours that a cell has
	Inputs from user: None.
	Outputs to user: None.
 */
int numberOfNeighbours(char (*board)[boardWidth], coord current)
{
	/* Return variable */
	int neighbours = 0;

	/* Starting from the far top left, check all the cells in a clockwise order, to see if a neighbour cell is alive */

	/*Top left */
	if( (current.row > 0) && (current.column > 0) )
		if (board[current.row -1][current.column -1] != ' ')
			neighbours++;

	/*Top middle */
	if(current.row > 0)
		if (board[current.row -1][current.column] != ' ')
			neighbours++;

	/*Top right */
	if( (current.row > 0) && (current.column < (boardWidth -1)) )
		if (board[current.row -1][current.column +1] != ' ')
			neighbours++;

	/* Centre right */
	if(current.column < (boardWidth -1) )
		if (board[current.row][current.column +1] != ' ')
			neighbours++;

	/* Bottom right */
	if( (current.column < (boardWidth -1)) && (current.row < (boardHeight -1)) )
		if (board[current.row + 1][current.column +1] != ' ')
			neighbours++;

	/* Bottom middle */
	if(current.row < (boardHeight -1))
		if (board[current.row +1][current.column] != ' ')
			neighbours++;

	/* Bottom left */
	if( (current.column > 0) && (current.row < (boardHeight -1)) )
		if (board[current.row +1][current.column -1] != ' ')
			neighbours++;

	/* Centre left */
	if(current.column > 0)
		if (board[current.row][current.column -1] != ' ')
			neighbours++;

	/* Return the number of neighbours that there are. */
	return neighbours;
}


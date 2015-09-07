/*
	lextolife.c v1.0
	Program to convert data from http://www.argentum.freeserve.co.uk/lex_home.htm to the format accepted by the game of life programming assignments.

	Usage: ./lextolife <input file> <output file> <number of blank columns> <number of blank rows>

	<input file> is a text file containing an initial game of life state, copied and pasted from the lexicon above.
	<output file> is a text file containing a list of co-ordinates of all the live cells in an initial state, as specified by the assignment specifications.
	<number of blank columns> is the number of blank columns to leave full of dead cells, before the input from input file.
	<number of blank rows> is the number of blank columns to leave full of dead cells, before the input from input file.

	<number of blank columns> and <number of blank rows> are  useful for states which will expand above of to the left of their initial state.

	Created by Joshua Tyler
	12/05/13
*/


#include <stdio.h>
#include <stdlib.h>

/* The characters that represent various entities in the input file. */
#define LIVE_CELL_CHARACTER 'O'
#define DEAD_CELL_CHARACTER '.'

/* Structure to hold the co-ordinates of a point in a linked list*/
typedef struct coord
{
	int column;
	int row;
	struct coord* next;
} coord;

/* Head pointer for co-ordinate linked list */
coord* head = NULL;

/* Function prototypes */
int readCellsFromInput(FILE* fp, int blankColumns, int blankRows);
void writeOutputFile(FILE *fp, int numberLiveCells);

int main(int argc, char* argv[])
{
	/* Check no. of arguments */
	if(argc != 5)
	{
		fprintf(stderr, "Invalid arguments.\n"
		                "Usage: %s <input file> <output file> <number of blank columns> <number of blank rows>\n"
		                "The program will now exit.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open input file. */
	FILE *inputFilePtr;

	inputFilePtr = fopen(argv[1], "r");
	if(inputFilePtr == NULL)
	{
		fprintf(stderr, "Error opening input file (%s).\n"
		                "Please ensure that the specified file exists.\n"
		                "The program will now close.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Scan the numbers of blank rows and columns to integers */
	int blankRows, blankColumns;

	if(sscanf(argv[3], "%d", &blankColumns) != 1)
	{
		fputs("Invalid number of blank columns specified.\n"
		      "The program will now close.\n",stderr);
		exit(EXIT_FAILURE);
	}
	if(sscanf(argv[4], "%d", &blankRows) != 1)
	{
		fputs("Invalid number of blank rows specified.\n"
		      "The program will now close.\n",stderr);
		exit(EXIT_FAILURE);
	}

	/* Variable to store number of live cells in the input file. */
	int numberLiveCells;

	/* Read the live cells from the input to a co-ordinate linked list. */
	numberLiveCells = readCellsFromInput(inputFilePtr, blankColumns, blankRows);
	fclose(inputFilePtr);

	/* Check if the output file exists by attempting to open it. */
	FILE *outputFilePtr;

	outputFilePtr = fopen(argv[2], "r");
	if(outputFilePtr != NULL)
	{
		fprintf(stderr, "WARNING, The file you are attempting to write to (%s) already exits.\n"
		                "It will be overwritten. Do you wish to continue?\n"
		                "[y]es/[n]o\n", argv[2]);
		int c;
		while(1)
		{
			c = getchar();
			if( (c == 'y') || (c == 'Y') )
			{
				/* Clear input buffer before exiting */
				while(c != '\n')
					c = getchar();
				/* Close the file */
				fclose(outputFilePtr);
				break;
			}else if( (c == 'n') || (c == 'N') )
				exit(EXIT_FAILURE);

			/* If we get to here, the input was invalid */
			fprintf(stderr, "Invalid input.\n");

			/* Clear buffer and loop */
			while(c != '\n')
				c = getchar();
		}
			
	}

	/* Open the output file for writing. */
	outputFilePtr = fopen(argv[2], "w");
	if(outputFilePtr == NULL)
	{
		fprintf(stderr, "Error opening output file (%s) for writing.\n"
		                "The program will now close.\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	/* Now write the output file */
	writeOutputFile(outputFilePtr, numberLiveCells);
	fclose(outputFilePtr);

	return EXIT_SUCCESS;
}

/* Reads input from fp and generates a coordinate linked list.
   Offsets the input file by by the number of blank rows and columns specified.
   Returns no. of live cells.
   FUNCTION DOES NO ERROR CHECKING */
int readCellsFromInput(FILE* fp, int blankColumns, int blankRows)
{

	/* Position acts as the loop counter, and stores the current position in the input file.
	   position.next is NULL so that  the linked list is properly terminated. */
	coord position;
	position.next = NULL;

	/* head is the linked list head pointer.
	   last always points to the last element in the linked list. */
	extern coord *head;
	coord *last;

	int character;
	int numberLiveCells = 0;
	
	/* External loop does rows, character initialised to 'a' to ensure it's not EOF. */
	for(position.row= blankRows, character = 'a'; character != EOF; position.row++)
		/* Internal loop does columns, character initialised to 'a' to ensure it's not currently EOF or '\n'. */
		for(position.column=blankColumns, character = 'a'; (character != '\n') && (character != EOF); position.column++)
		{
			/*Loop until we have a character that we're interested in. */
			do
				character = fgetc(fp);
			while( (character != '\n') && (character != EOF) && (character != LIVE_CELL_CHARACTER) && (character != DEAD_CELL_CHARACTER) );

			if(character == LIVE_CELL_CHARACTER)
			{
				/*If head is null, store this coordinate in the head.
				  If the head is not null, put it on the end (using the last pointer) */
				if(head == NULL)
				{
					head = (coord *)malloc(sizeof(coord));
					if(head == NULL)
					{
						fputs("Memory allocation error.\n"
						      "The program will now exit.\n", stderr);
						exit(EXIT_FAILURE);
					}
					last = head;
				} else {
					last->next = (coord *)malloc(sizeof(coord));
					if(last->next == NULL)
					{
						fputs("Memory allocation error.\n"
						      "The program will now exit.\n", stderr);
						exit(EXIT_FAILURE);
					}
					last = last->next;
				}
			/* Set the newly allocated memory equal to the current coordinates */
			*last = position;
			/* Increment the live cell counter */
			numberLiveCells++;
			}

		}
			
	return numberLiveCells;
}

/* Writes output from linked list to fp */
void writeOutputFile(FILE *fp, int numberLiveCells)
{
	fprintf(fp,"%d\n", numberLiveCells);

	coord *current = head;

	while(current !=NULL)
	{
		fprintf(fp,"%d %d\n", current->row, current->column);
		current = current->next;
	}

	return;
}


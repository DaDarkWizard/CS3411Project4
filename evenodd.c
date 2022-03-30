/*
 * Ben Hodsdon; Dasker Masker
 * CS3411
 * 3/16/2022
 *
 * Parses input passed in from the scanner.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char** argv){

	// Received input from stdin and write it to a temp file
	FILE* fp = fopen("temp","w+");
	char buf[1024];
	fgets(buf,1024,stdin);
	fputs(buf,fp);


	// Reads temp file to determine number of string inputs
	rewind(fp);
	char word[1024];
	int size = 0;
	while(fscanf(fp,"%s",word) > 0){size++;}

	// Reads string inputs into local array
	rewind(fp);
	int i = 0;
	char table[size][1024];
	while(fscanf(fp,"%s",word) > 0){

		strcpy(table[i],word);
		i++;
	}

	// Removes file pointer and temp file
	fclose(fp);
	remove("temp");

	// Prints word size pairs to stdout
	for(int i = 0; i < size; i++){

		int len = strlen(table[i]);
		printf("\n	%s	%d",table[i],len);

	}

	// Condition for even
	if(strcmp(argv[0],"even") == 0){

	}

	// Condition for odd
	else if(strcmp(argv[0],"odd") == 0){

	}

	return -1;
}


/*
 *	saispe.c
 *	the Șaișpe assembler
 *
 *	author: Vlad Dumitru (deveah@gmail.com)
 *	license: TODO
*/

/*
 *	INTRO ---------------------------------------------------------------------
 *
 *		This file belongs to the Șaișpe project, which aims to be a simple and
 *	literate COM assembler. Its original purpose was to aid the studying of
 *	assemblers and their behaviour. I, the author, have chosen the COM format
 *	as its output because of its simplicity; also, the negative aspects of the
 *	format can be neglected, since they aren't in the way of the study.
 *		I'm currently writing this project on a Linux box, so DOSBox is
 *	essential in the development of this assembler. Also, I would recommend
 *	Windows users to use DOSBox as well, since it will be the main target VM.
 *		In order to view the contents of the file at its best, you will need
 *	a text editor with syntax highlighting, configured with a tab space of 4,
 *	and a minimum of 80 columns.
 *		The assembler requires no libraries other than the C standard library,
 *	and is compiled as such:
 *
 *		$ gcc saispe.c -o saispe -Wall -Wextra
 *
 *	TODO ----------------------------------------------------------------------
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>

#define COMMENT_CHARACTER '#'
#define STRING_DELIMITER '"'

#define MODE_CODE 0
#define MODE_STRING 1
#define MODE_COMMENT 2

#define SAISPE_VERSION 1

#define MAX_INPUT_FILES 16
#define DEFAULT_OUTPUT_FILENAME "output.com"
#define MAX_TOKEN_LENGTH 256

#define TOKEN_WORD 0
#define TOKEN_STRING 1

/*#include "saispe.h"
*/

int saispe_verbose = 0;


char *input_filename[MAX_INPUT_FILES];
int input_files = 0;
char *output_filename = NULL;
FILE *input_file, *output_file;

void print_help( char* self )
{
	printf(
		"saispe v%02i - http://github.com/deveah/saispe\n"
		"usage: %s [input] [flags]\n"
		"flags:\n"
		"\t-o\toutput filename\n"
		"\t-v\tverbose\n"
		"\t-h\tshow this help and exit\n",
		SAISPE_VERSION,
		self
	);
}

int parse( char* token, int type )
{
	printf( "[%s] %s\n", (type==TOKEN_WORD)?"word":"string", token );
}

int tokenize( FILE *f )
{
	int cc;
	char token[MAX_TOKEN_LENGTH];
	int ti = 0;

	int mode = MODE_CODE;

	while( !feof( f ) )
	{
		cc = fgetc( f );

		if( mode == MODE_CODE )
		{
			if(	( cc == ' ' ) ||
				( cc == '\n' ) ||
				( cc == '\t' ) )
			{
				token[ti] = 0;
				
				if( ti > 0 )
					parse( token, TOKEN_WORD );
				
				ti = 0;
			}
			else if ( cc == COMMENT_CHARACTER )
			{
				mode = MODE_COMMENT;
			}
			else if ( cc == STRING_DELIMITER )
			{
				mode = MODE_STRING;
			}
			else
			{
				token[ti++] = cc;
			}
		}
		else if( mode == MODE_COMMENT )
		{
			if( cc == '\n' )
				mode = MODE_CODE;	
		}
		else if( mode == MODE_STRING )
		{
			/* TODO escapes */

			if( cc == STRING_DELIMITER )
			{
				token[ti] = 0;
				
				/* empty strings are allowed */
				parse( token, TOKEN_STRING );

				ti = 0;
				mode = MODE_CODE;
			}
			else
				token[ti++] = cc;
		}
	}

	return 0;
}

int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		printf( "saispe: no input files\n" );
		return -1;
	}
	
	/* parse commandline arguments */
	int i;
	for( i = 1; i < argc; i++ )
	{
		/* check whether it's a flag */
		if( argv[i][0] == '-' )
		{
			if( argv[i][1] == 'v' )
				saispe_verbose = 1;
			
			if( argv[i][1] == 'h' )
			{
				print_help( argv[0] );
				return 0;
			}

			if( argv[i][1] == 'o' )
			{
				if( argc < i+1 )
				{
					printf( "saispe: incomplete argument\n" );
					return -1;
				}

				output_filename = argv[++i];
			}
		}
		else
		{
			input_filename[input_files] = argv[i];
			input_files++;
		}
	}

	if( output_filename == NULL )
		output_filename = DEFAULT_OUTPUT_FILENAME;

	/* start tokenizing each file, in given order */

	for( i = 0; i < input_files; i++ )
	{
		input_file = fopen( input_filename[i], "r" );
		if( input_file == NULL )
		{
			printf( "saispe: file error\n" );
			return -1;
		}

		tokenize( input_file );

		fclose( input_file );
	}

	return 0;
}



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
#include <string.h>
#include <errno.h>

/*	delimiter defines;
 *	if a line contatins COMMENT_CHARACTER, it will be treated from that moment
 *	on as a comment
*/
#define COMMENT_CHARACTER '#'
#define STRING_DELIMITER '"'

/*	modes needed when passing through the input files */
#define MODE_CODE 0
#define MODE_STRING 1
#define MODE_COMMENT 2

#define SAISPE_VERSION 1

#define MAX_INPUT_FILES 16
#define DEFAULT_OUTPUT_FILENAME "output.com"
#define MAX_TOKEN_LENGTH 256

/*	token types */
#define TOKEN_WORD 0
#define TOKEN_STRING 1
#define TOKEN_NUMBER 2
#define TOKEN_POINTER_NAMED 3
#define TOKEN_POINTER_VALUE 4

/*	verbose flag - either 0 (false) or 1 (true) */
int saispe_verbose = 0;

/*	the array of input file paths, and the total input file count */
char *input_filename[MAX_INPUT_FILES];
int input_files = 0;

char *output_filename = NULL;

/*	file handles for input and output;
 *	only one input file handle is needed since all lexing is done sequentially */
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

/*	the function that parses the tokens;
 *	TODO
*/
int parse( void *token, int type )
{
	switch( type )
	{
		case TOKEN_WORD:
			printf( "[word] %s\n", (char*)token );
			break;
		case TOKEN_NUMBER:
			printf( "[number] %i\n", (int)token );
			break;
		case TOKEN_STRING:
			printf( "[string] \"%s\"\n", (char*)token );
			break;
		case TOKEN_POINTER_NAMED:
			printf( "[nptr] %s\n", (char*)token );
			break;
		case TOKEN_POINTER_VALUE:
			printf( "[vptr] 0x%08x\n", (int)token );
			break;
	}
}

/*	checks whether the string only contains digits */
int isnumber( char *s )
{
	int i = 0;
	while( s[i] != 0 )
	{
		if( !isdigit( s[i] ) )
			return 0;
		i++;
	}
	return 1;
}

/*	checks whether the string only contains hexadecimal digits */
int isxnumber( char *s )
{
	int i = 0;
	while( s[i] != 0 )
	{
		if( !isxdigit( s[i] ) )
			return 0;
		i++;
	}
	return 1;
}

/*	the function that scans through a file and passes the resulting tokens to
 *	the parser; TODO
*/
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
				{
					/*	check whether the token is a number
						acceptable formats:
							hex - prefixed by '0x'
							integer - no additional markers
					*/
					if(	( token[0] == '0' ) && ( token[1] == 'x' ) )
					{
						int temp = strtol( token, NULL, 16 );
						parse( temp, TOKEN_NUMBER );
					}
					else if( isnumber( token ) )
					{
						int temp = strtol( token, NULL, 10 );
						parse( temp, TOKEN_NUMBER );
					}

					/*	check whether the token is a pointer
						format: prefixed by '@' - either number(hex) or named
					*/
					else if( token[0] == '@' )
					{
						if( isxnumber( token+1 ) )
						{
							int temp = strtol( token+1, NULL, 16 );
							parse( temp, TOKEN_POINTER_VALUE );
						}
						else
							parse( token+1, TOKEN_POINTER_NAMED );
					}
					else
						parse( token, TOKEN_WORD );
				}

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
				if( argc < i+2 )
				{
					printf( "saispe: incomplete commandline argument\n" );
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

	/* open the output file */
	output_file = fopen( output_filename, "w" );

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

	fclose( output_file );

	return 0;
}


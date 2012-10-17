
/*
 *	saispe.c
 *	the Șaișpe assembler
 *
 *	author: Vlad Dumitru (deveah@gmail.com)
 *	license: TODO
*/

#include <stdio.h>
#include <stdlib.h>

#define SAISPE_COMMENT_CHARACTER '#'

#define SAISPE_CODE 0
#define SAISPE_DATA 1
#define SAISPE_COMMENT 2

#define SAISPE_VERSION 1

#define SAISPE_MAX_INPUT_FILES 16
#define SAISPE_DEFAULT_OUTPUT_FILENAME "output.com"
#define SAISPE_MAX_TOKEN_LENGTH 256

/*#include "saispe.h"
*/

int saispe_verbose = 0;


char *input_filename[SAISPE_MAX_INPUT_FILES];
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

int parse( char* token )
{
	printf( ":: %s\n", token );
}

int tokenize( FILE *f )
{
	int cc;
	char token[SAISPE_MAX_TOKEN_LENGTH];
	int ti = 0;

	int mode = SAISPE_CODE;

	while( !feof( f ) )
	{
		cc = fgetc( f );

		/* TODO commenting breaks stuff */

		if( ( cc == '\n' ) && ( mode == SAISPE_COMMENT ) )
		{
			printf( "jbang!\n" );
			mode = SAISPE_CODE;
		}

		if(	( cc == ' ' ) ||
			( cc == '\n' ) ||
			( cc == '\t' ) )
		{
			if( mode == SAISPE_CODE )
			{
				token[ti] = 0;
				
				if( ti > 0 )
					parse( token );
				
				ti = 0;
			}
		}
		else if ( cc == SAISPE_COMMENT_CHARACTER )
		{
			mode = SAISPE_COMMENT;
		}
		else
		{
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
		output_filename = SAISPE_DEFAULT_OUTPUT_FILENAME;

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


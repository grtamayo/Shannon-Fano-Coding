/*
	---- A Static Shannon-Fano Encoding Implementation ----

	Filename:     GTSFD2.C (the decoder to GTSF2.C)
	Written by:   Gerald R. Tamayo, 2005
*/
#include <stdio.h>
#include <stdlib.h>
#include "utypes.h"
#include "gtbitio.c"
#include "sf.c"

typedef struct {
	char algorithm[4];
	ulong file_size;
} file_stamp;

void copyright( void );

int main( int argc, char *argv[] )
{
	file_stamp fstamp;
	sffreq_type freq;
	int c = 0;
	ulong i_file_size = 0;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: gtsfd2 infile outfile");
		copyright();
		return 0;
	}

	if ( (gIN = fopen( argv[1], "rb" )) == NULL ) {
		fprintf(stderr, "\nError opening input file.");
		return 0;
	}
	fread( &fstamp, sizeof(file_stamp), 1, gIN );

	if ( (pOUT = fopen( argv[2], "wb" )) == NULL ) {
		fprintf(stderr, "\nError opening output file.");
		goto halt_prog;
	}
	init_put_buffer();
	
	/* ---- start the Shannon-Fano Decompression process. ---- */
	
	/* initialize sffreq[] table. */
	init_sffreq();

	/* read the number of symbols from the file. */
	fread( &sfcount, sizeof(int), 1, gIN );

	/* now read the symbols from the file. */
	for ( c = 0; c < sfcount; c++ ){
		fread( &freq, sizeof(sffreq_type), 1, gIN );
		sffreq[ (unsigned char) freq.b ] = freq;
	}

	/* prepare the tree list of symbols. */
	init_sflist();
	create_symbol_list();

	top = create_node();   /* allocate memory for top. */
	top->next = list;      /* point to the symbol list. */

	/* then create the corresponding Shannon-Fano tree. */
	create_shannon_fano_tree( top );

	/* called after fopening gIN, and after reading the file stamp. */
	init_get_buffer();

	fprintf(stderr, "\n Name of input file  : %s", argv[1] );
	fprintf(stderr, "\n\n Decompressing...");

	/* finally, decompress. */
	for ( i_file_size = fstamp.file_size; i_file_size; i_file_size-- ){
		c = sfdecompress( top );
		pfputc( (uchar) c );
	}
	flush_put_buffer();
	
	fprintf(stderr, "complete.\n");
	
	fprintf(stderr, "\n Name of output file : %s\n", argv[2] );
	
	halt_prog:
	
	free_get_buffer();
	free_put_buffer();
	if ( gIN ) fclose( gIN );
	if ( pOUT ) fclose( pOUT );
	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n\n Written by: Gerald R. Tamayo, 2005/2024\n");
}

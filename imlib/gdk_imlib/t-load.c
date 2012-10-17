#include <stdio.h>
#include "gdk_imlib.h"


int
main( int ac, char* av[] )
{
    GdkImlibImage* img;

    if (ac != 2) {
	fprintf( stderr, "usage: %s filename\n", av[0] );
	return 33;
    }

    img = gdk_imlib_load_image( av[1] );

    if ( img == NULL ) {
	fprintf( stderr, "failed to load file '%s'.\n", av[1] );
	return 1;
    }

    return 0;
}

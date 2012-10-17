#include <stdio.h>
#include "Imlib.h"


int
main( int ac, char* av[] )
{
    Display *disp;
    ImlibData *id;
    XSetWindowAttributes attr;
    Window win;
    ImlibImage *img;
    Pixmap p,m;
    int w,h;
     
    /* Be nice and tell the user if they don't, to provide a file as an arg */
    if (ac != 2) {
	fprintf( stderr, "usage: %s filename\n", av[0] );
	return 33;
    }

    /* Connect to the default Xserver */
    disp = XOpenDisplay(NULL);

    /* Immediately afterwards Intitialise Imlib */
    id = Imlib_init(disp);

    /* Load the image specified as the first argument */
    img = Imlib_load_image(id,av[1]);


    if ( img == NULL ) {
	fprintf( stderr, "failed to load file '%s'.\n", av[1] );
	return 1;
    }

    return 0;
}

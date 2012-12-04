#ifdef PLD_png
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>

#define RESNAME  "gdisplay"
#define RESCLASS "gDisplay"

static int  x_create_window(void);
static int  x_display_image(void);
static void x_cleanup(void);
static int  x_msb(unsigned long u32val);

static unsigned long image_width, image_height, image_rowbytes;
static unsigned char bg_red=0, bg_green=0, bg_blue=0;

static png_structp png_ptr = NULL;
static png_infop   info_ptr = NULL;

static char  titlebar[1024], *window_name = titlebar;
static char *appname;
static char *icon_name;
static char *res_name  = RESNAME;
static char *res_class = RESCLASS;

static char         *displayname;
static XImage       *ximage;
static Display      *display;
static int           depth;
static Visual       *visual;
static XVisualInfo  *visual_list;
static int           RShift, GShift, BShift;
static unsigned long RMask, GMask, BMask;
static Window        window;
static GC            gc;
static Colormap      colormap;

static int have_nondefault_visual = FALSE;
static int have_colormap = FALSE;
static int have_window = FALSE;
static int have_gc = FALSE;

static png_uint_32    width, height;
static unsigned char *image_data = NULL;

static int bit_depth, color_type;
static int image_channels;

int png_init(FILE *fp, unsigned long *pWidth, unsigned long *pHeight){
  unsigned char sig[8];

  fread(sig, 1, 8, fp);
  if(!png_check_sig(sig, 8))
    return 1;

  if(!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
					NULL, NULL, NULL)))
    return 4;
  
  if(!(info_ptr = png_create_info_struct(png_ptr)))
    return 4;
/*
#if (PNG_LIBPNG_VER < 10400 || PNG_LIBPNG_VER > 10500)
  if(setjmp(png_jmpbuf(png_ptr))){
#else
  if(setjmp(png_ptr)->jmpbuf){
#endif
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 2;
  }
*/
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	       NULL,NULL,NULL);
  *pWidth = width;
  *pHeight = height;

  return 0;
}

unsigned char* get_image(int *pChannels, unsigned long *pRowbytes){
  double gamma;
  png_uint_32 i, rowbytes;
  png_bytepp row_pointers = NULL;
/*
#if (PNG_LIBPNG_VER < 10400 || PNG_LIBPNG_VER > 10500)
  if(setjmp(png_jmpbuf(png_ptr))){
#else
  if(setjmp(png_ptr)->jmpbuf){
#endif
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return NULL;
  }
*/
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);
  if (bit_depth == 16)
    png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  *pChannels = (int)png_get_channels(png_ptr, info_ptr);

  if((image_data = (unsigned char*)malloc(rowbytes*height)) == NULL){
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return NULL;
  }
  if((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL){
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(image_data);
    image_data = NULL;
    return NULL;
  }

  for(i = 0; i < height; ++i)
    row_pointers[i] = image_data + i*rowbytes;

  png_read_image(png_ptr, row_pointers);

  free(row_pointers);
  row_pointers = NULL;

  png_read_end(png_ptr, NULL);

  return image_data;
}

static x_msb(unsigned long u32val){
  int i;

  for(i = 31; i >= 0; --i){
    if(u32val & 0x80000000L)
      break;
    u32val <<= 1;
  }
  return i;
}

static int x_create_window(void){
  unsigned char *xdata;
  int            need_colormap = FALSE;
  int            screen, pad;
  unsigned long  bg_pixel = 0L;
  unsigned long  attrmask;
  Window         root;
  XEvent         e;
  XGCValues      gcvalues;
  XSetWindowAttributes attr;
  XTextProperty  windowName, *pWindowName = &windowName;
  XTextProperty  iconName, *pIconName = &iconName;
  XVisualInfo    visual_info;
  XSizeHints    *size_hints;
  XWMHints      *wm_hints;
  XClassHint    *class_hints;

  screen = DefaultScreen(display);
  depth  = DisplayPlanes(display, screen);
  root   = RootWindow(display, screen);

  if(depth != 16 && depth != 24 && depth != 32){
    fprintf (stderr, "Screen depth %d not supported\n", depth);
  }
  XMatchVisualInfo(display, screen, depth,
		   (depth == 8)? PseudoColor : TrueColor, &visual_info);
  visual = visual_info.visual;

  RMask = visual->red_mask;
  GMask = visual->green_mask;
  BMask = visual->blue_mask;

  if(depth == 8 || need_colormap){
    colormap = XCreateColormap(display, root, visual, AllocNone);
    if(!colormap){
      return 2;
    }
    have_colormap = TRUE;
  }
  if(depth ==15 || depth == 16){
    RShift = 15 - x_msb(RMask);
    GShift = 15 - x_msb(GMask);
    BShift = 15 - x_msb(BMask);
  }else if(depth > 16){
    RShift = x_msb(RMask) - 7;
    GShift = x_msb(GMask) - 7;
    BShift = x_msb(BMask) - 7;
  }
  if(depth >= 15 && (RShift < 0 || GShift < 0 || BShift < 0)){
    return 2;
  }

  attr.backing_store = Always;
  attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
  attrmask = CWBackingStore | CWEventMask;
  if(have_nondefault_visual){
    attr.colormap = colormap;
    attr.background_pixel = 0;
    attr.border_pixel = 1;
    attrmask |= CWColormap | CWBackPixel | CWBorderPixel;
  }

  window = XCreateWindow(display, root, 0, 0, image_width, image_height, 0,
			 depth, InputOutput, visual, attrmask, &attr);

  if(window == None)
    return 2;
  else
    have_window = TRUE;

  if(depth == 8)
    XSetWindowColormap(display, window, colormap);

  if(!XStringListToTextProperty(&window_name, 1, pWindowName))
    pWindowName = NULL;
  if(!XStringListToTextProperty(&icon_name, 1, pIconName))
    pIconName = NULL;

  if((size_hints = XAllocSizeHints()) != NULL){
    size_hints->flags      = PMinSize | PMaxSize;
    size_hints->min_width  = size_hints->max_width = (int)image_width;
    size_hints->min_height = size_hints->max_height = (int)image_height;
  }

  if((wm_hints = XAllocWMHints()) != NULL){
    wm_hints->initial_state = NormalState;
    wm_hints->input = True;
    wm_hints->flags = StateHint | InputHint;
  }

  if((class_hints = XAllocClassHint()) != NULL){
    class_hints->res_name  = res_name;
    class_hints->res_class = res_class;
  }

  XSetWMProperties(display, window, pWindowName, pIconName, NULL, 0,
		   size_hints, wm_hints, class_hints);

  if(pWindowName)
    XFree(pWindowName->value);
  if(pIconName)
    XFree(pIconName->value);
  if(size_hints)
    XFree(size_hints);
  if(wm_hints)
    XFree(wm_hints);
  if(class_hints)
    XFree(class_hints);

  XMapWindow(display, window);

  gc = XCreateGC(display, window, 0, &gcvalues);
  have_gc = TRUE;

  if(depth == 24 || depth == 320){
    bg_pixel = ((unsigned long)bg_red << RShift) |
      ((unsigned long)bg_green << GShift) |
      ((unsigned long)bg_blue << BShift);
  }else if(depth == 16){
    bg_pixel = ((((unsigned long)bg_red << 8) >> RShift) & RMask) |
      ((((unsigned long)bg_green << 8) >> GShift) & GMask) |
      ((((unsigned long)bg_blue << 8) >> BShift) & BMask);
  }

  XSetForeground(display, gc, bg_pixel);
  XFillRectangle(display, window, gc, 0, 0, image_width, image_height);

  do
    XNextEvent(display, &e);
  while(e.type != Expose || e.xexpose.count);

  XFlush(display);

  if(depth == 24 || depth == 32){
    xdata = (unsigned char*)malloc(4*image_width*image_height);
    pad = 32;
  }else if(depth == 16){
    xdata = (unsigned char*)malloc(2*image_width*image_height);
    pad = 16;
  }else{
    xdata = (unsigned char*)malloc(image_width*image_height);
    pad = 8;
  }

  if(!xdata)
    return 4;

  ximage = XCreateImage(display, visual, depth, ZPixmap, 0,
			(char*)xdata, image_width, image_height, pad, 0);

  if(!ximage){
    free(xdata);
    return 3;
  }

  ximage->byte_order = MSBFirst;

  return 0;
}

static int x_display_image(void){
  unsigned char *src;
  char *dest;
  unsigned char r, g, b, a;
  unsigned long i, row, lastrow = 0;
  unsigned long pixel;
  int ximage_rowbytes = ximage->bytes_per_line;

  if(depth == 24 || depth == 32){
    unsigned long red, green, blue;

    for(lastrow = row = 0; row < image_height; ++row){
      src = image_data + row*image_rowbytes;
      dest = ximage->data + row*ximage_rowbytes;
      if(image_channels == 3){
	for( i = image_width; i > 0; --i){
	  red   = *src++;
	  green = *src++;
	  blue  = *src++;
	  pixel = (red << RShift) |
	    (green << GShift) |
	    (blue << BShift);
	  *dest++ = (char)((pixel >> 24) & 0xff);
	  *dest++ = (char)((pixel >> 16) & 0xff);
	  *dest++ = (char)((pixel >>  8) & 0xff);
	  *dest++ = (char)( pixel        & 0xff);
	}
      }else{
	for(i = image_width; i > 0; --i){
	  r = *src++;
	  g = *src++;
	  b = *src++;
	  a = *src++;
	  if(a == 255){
	    red   = r;
	    green = g;
	    blue  = b;
	  }else if(a == 0){
	    red   = bg_red;
	    green = bg_green;
	    blue  = bg_blue;
	  }
	  pixel = (red << RShift) |
	    (green << GShift) |
	    (blue << BShift);
	  *dest++ = (char)((pixel >> 24) & 0xff);
	  *dest++ = (char)((pixel >> 16) & 0xff);
	  *dest++ = (char)((pixel >>  8) & 0xff);
	  *dest++ = (char)( pixel        & 0xff);
	}
      }
      if(((row+1) & 0xf) == 0){
	XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
		  (int)lastrow, image_width, 16);
	XFlush(display);
	lastrow = row + 1;
      }
    }
  }else if(depth == 16){
    unsigned short red, green, blue;

    for(lastrow = row = 0; row < image_height; ++row){
      src = image_data + row*image_rowbytes;
      dest = ximage->data + row*ximage_rowbytes;
      if(image_channels == 3){
	for(i = image_width; i > 0; --i){
	  red = ((unsigned short)(*src) << 8);
	  ++src;
	  green = ((unsigned short)(*src) << 8);
	  ++src;
	  blue = ((unsigned short)(*src) << 8);
	  ++src;
	}
      }else{
	for(i = image_width; i > 0; --i){
	  r = *src++;
	  g = *src++;
	  b = *src++;
	  a = *src++;
	  if(a == 255){
	    red   = ((unsigned short)r << 8);
	    green = ((unsigned short)g << 8);
	    blue  = ((unsigned short)b << 8);
	  }else if(a == 0){
	    red   = ((unsigned short)bg_red   << 8);
	    green = ((unsigned short)bg_green << 8);
	    blue  = ((unsigned short)bg_blue  << 8);
	  }else{
	    red   = ((unsigned short)r << 8);
	    green = ((unsigned short)g << 8);
	    blue  = ((unsigned short)b << 8);
	  }
	  pixel = ((red >> RShift) & RMask) |
	    ((green >> GShift) & GMask) |
	    ((blue >> BShift) & BMask);
	  *dest++ = (char)((pixel >> 8) & 0xff);
	  *dest++ = (char)( pixel       & 0xff);
	}
      }
      if(((row+1) & 0xf) == 0){
	XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
		  (int)lastrow, image_width, 16);
	XFlush(display);
	lastrow = row + 1;
      }
    }
  }
  if(lastrow < image_height){
    XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
	      (int)lastrow, image_width, image_height-lastrow);
    XFlush(display);
  }
}

static void x_cleanup(void){
  if(image_data){
    free(image_data);
    image_data = NULL;
  }

  if(ximage){
    if(ximage->data){
      free(ximage->data);
      ximage->data = (char*)NULL;
    }
    XDestroyImage(ximage);
    ximage = NULL;
  }

  if(have_gc)
    XFreeGC(display, gc);

  if(have_window)
    XDestroyWindow(display, window);

  if(have_colormap)
    XFreeColormap(display, colormap);

  if(have_nondefault_visual)
    XFree(visual_list);
}

int display_png(char* filename, char* gtool, char* accession){
  FILE  *fp;
  int    img_channels;
  XEvent e;
  KeySym k;

  appname = gtool;
  icon_name = accession;

  sprintf(titlebar, "%s: %s", gtool, accession);

  if(!(fp = fopen(filename,"rb")))
    return 1;

  if(png_init(fp, &image_width, &image_height))
    return 1;
  if((image_data = get_image(&image_channels, &image_rowbytes)) == NULL)
    return 1;

  display = XOpenDisplay(NULL);
  if(x_create_window())
    return 1;
  if(x_display_image())
    return 1;
  do
    XNextEvent(display, &e);
  while(!(e.type == ButtonPress && e.xbutton.button == Button1) &&
	!(e.type == KeyPress &&
	  ((k = XLookupKeysym(&e.xkey, 0)) == XK_q || k == XK_Escape) ));
  x_cleanup();

  return 0;
}
#else
int display_png(char* filename, char* gtool, char* accession){
  fprintf(stderr,"PNG support not found.\n");
  return 0;
}
#endif

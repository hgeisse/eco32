/*
 * dsp.c -- display simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "cpu.h"
#include "timer.h"
#include "dsp.h"
#include "kbd.h"


static Bool debug = false;
static Bool volatile installed = false;


static void blinkScreen(void);


/**************************************************************/
/**************************************************************/

/* common definitions, global variables */


#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


#define WINDOW_SIZE_X		640
#define WINDOW_SIZE_Y		480
#define WINDOW_POS_X		100
#define WINDOW_POS_Y		100


#define C2B(c,ch)		(((((c) & 0xFF) * ch.scale) >> 8) * ch.factor)
#define RGB2PIXEL(r,g,b)	(0xFF000000 | \
				 C2B(r, vga.red) | \
				 C2B(g, vga.green) | \
				 C2B(b, vga.blue))


typedef struct {
  unsigned long scale;
  unsigned long factor;
} ColorChannel;


typedef struct {
  int argc;
  char **argv;
  Display *display;
  Window win;
  GC gc;
  XImage *image;
  ColorChannel red, green, blue;
  XExposeEvent expose;
  XClientMessageEvent shutdown;
} VGA;


static VGA vga;


/**************************************************************/

/* monitor server */


static ColorChannel mask2channel(unsigned long mask) {
  unsigned long f;
  ColorChannel ch;

  if (mask == 0) {
    error("color mask is 0 in mask2channel");
  }
  for (f = 1; (mask & 1) == 0; f <<= 1) {
    mask >>= 1;
  }
  ch.factor = f;
  ch.scale = mask + 1;
  while ((mask & 1) != 0) {
    mask >>= 1;
  }
  if (mask != 0) {
    error("scattered color mask bits in mask2channel");
  }
  return ch;
}


static void initMonitor(int argc, char *argv[]) {
  int screenNum;
  Window rootWin;
  XVisualInfo visualTemp;
  XVisualInfo *visualInfo;
  int visualCount;
  int bestMatch;
  int bestDepth;
  Visual *visual;
  int i;
  unsigned long pixel;
  int x, y;
  Colormap colormap;
  XSetWindowAttributes attrib;
  XSizeHints *sizeHints;
  XWMHints *wmHints;
  XClassHint *classHints;
  XTextProperty windowName;
  XGCValues gcValues;

  /* connect to X server */
  if (XInitThreads() == 0) {
    error("no thread support for X11");
  }
  vga.display = XOpenDisplay(NULL);
  if (vga.display == NULL) {
    error("cannot connect to X server");
  }
  screenNum = DefaultScreen(vga.display);
  rootWin = RootWindow(vga.display, screenNum);
  /* find TrueColor visual */
  visualTemp.screen = screenNum;
  visualTemp.class = TrueColor;
  visualInfo = XGetVisualInfo(vga.display,
                              VisualClassMask | VisualScreenMask,
                              &visualTemp, &visualCount);
  if (visualInfo == NULL || visualCount == 0) {
    error("no TrueColor visual found");
  }
  bestMatch = 0;
  bestDepth = visualInfo[0].depth;
  visual = visualInfo[0].visual;
  for (i = 1; i < visualCount; i++) {
    if (visualInfo[i].depth > bestDepth) {
      bestMatch = i;
      bestDepth = visualInfo[i].depth;
      visual = visualInfo[i].visual;
    }
  }
  /* build color channels */
  vga.red = mask2channel(visualInfo[bestMatch].red_mask);
  vga.green = mask2channel(visualInfo[bestMatch].green_mask);
  vga.blue = mask2channel(visualInfo[bestMatch].blue_mask);
  /* create and initialize image */
  vga.image = XCreateImage(vga.display, visual, bestDepth, ZPixmap,
                           0, NULL, WINDOW_SIZE_X, WINDOW_SIZE_Y, 32, 0);
  if (vga.image == NULL) {
    error("cannot allocate image");
  }
  vga.image->data = malloc(vga.image->height * vga.image->bytes_per_line);
  if (vga.image->data == NULL) {
    error("cannot allocate image memory");
  }
  pixel = RGB2PIXEL(0, 0, 0);
  for (y = 0; y < WINDOW_SIZE_Y; y++) {
    for (x = 0; x < WINDOW_SIZE_X; x++) {
      XPutPixel(vga.image, x, y, pixel);
    }
  }
  /* allocate a colormap */
  colormap = XCreateColormap(vga.display, rootWin, visual, AllocNone);
  /* create the window */
  attrib.colormap = colormap;
  attrib.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
  attrib.background_pixel = RGB2PIXEL(0, 0, 0);
  attrib.border_pixel = RGB2PIXEL(0, 0, 0);
  vga.win =
    XCreateWindow(vga.display, rootWin,
                  WINDOW_POS_X, WINDOW_POS_Y,
                  WINDOW_SIZE_X, WINDOW_SIZE_Y,
                  0, bestDepth, InputOutput, visual,
                  CWEventMask | CWColormap | CWBackPixel | CWBorderPixel,
                  &attrib);
  /* give hints to window manager */
  sizeHints = XAllocSizeHints();
  wmHints = XAllocWMHints();
  classHints = XAllocClassHint();
  if (sizeHints == NULL ||
      wmHints == NULL ||
      classHints == NULL) {
    error("hint allocation failed");
  }
  sizeHints->flags = PMinSize | PMaxSize;
  sizeHints->min_width = WINDOW_SIZE_X;
  sizeHints->min_height = WINDOW_SIZE_Y;
  sizeHints->max_width = WINDOW_SIZE_X;
  sizeHints->max_height = WINDOW_SIZE_Y;
  wmHints->flags = StateHint | InputHint;
  wmHints->input = True;
  wmHints->initial_state = NormalState;
  classHints->res_name = "ECO32 Console";
  classHints->res_class = "ECO32";
  if (XStringListToTextProperty(&classHints->res_name, 1, &windowName) == 0) {
    error("property allocation failed");
  }
  XSetWMProperties(vga.display, vga.win, &windowName, NULL,
                   argv, argc, sizeHints, wmHints, classHints);
  /* create a GC */
  vga.gc = XCreateGC(vga.display, vga.win, 0, &gcValues);
  /* finally get the window displayed */
  XMapWindow(vga.display, vga.win);
  /* prepare expose event */
  vga.expose.type = Expose;
  vga.expose.display = vga.display;
  vga.expose.window = vga.win;
  vga.expose.x = 0;
  vga.expose.y = 0;
  vga.expose.width = WINDOW_SIZE_X;
  vga.expose.height = WINDOW_SIZE_Y;
  vga.expose.count = 0;
  /* prepare shutdown event */
  vga.shutdown.type = ClientMessage;
  vga.shutdown.display = vga.display;
  vga.shutdown.window = vga.win;
  vga.shutdown.message_type = XA_WM_COMMAND;
  vga.shutdown.format = 8;
  /* say that the graphics controller is installed */
  XSync(vga.display, False);
  installed = true;
}


static void exitMonitor(void) {
  XFreeGC(vga.display, vga.gc);
  XUnmapWindow(vga.display, vga.win);
  XDestroyWindow(vga.display, vga.win);
  XDestroyImage(vga.image);
  XCloseDisplay(vga.display);
  installed = false;
}


static int ioErrorHandler(Display *display) {
  error("connection to monitor window lost");
  /* never reached */
  return 0;
}


static void *server(void *ignore) {
  Bool run;
  XEvent event;

  initMonitor(vga.argc, vga.argv);
  XSetIOErrorHandler(ioErrorHandler);
  run = true;
  while (run) {
    XNextEvent(vga.display, &event);
    switch (event.type) {
      case Expose:
        blinkScreen();
        XPutImage(vga.display, vga.win, vga.gc, vga.image,
                  event.xexpose.x, event.xexpose.y,
                  event.xexpose.x, event.xexpose.y,
                  event.xexpose.width, event.xexpose.height);
        break;
      case ClientMessage:
        if (event.xclient.message_type == XA_WM_COMMAND &&
            event.xclient.format == 8) {
          run = false;
        }
        break;
      case KeyPress:
        keyPressed(event.xkey.keycode);
        break;
      case KeyRelease:
        keyReleased(event.xkey.keycode);
        break;
      default:
        break;
    }
  }
  exitMonitor();
  return NULL;
}


/**************************************************************/

/* refresh timer */


static Bool volatile refreshRunning = false;
static Bool volatile blink = false;


static void *refresh(void *ignore) {
  static int blinkCounter = 0;
  struct timespec delay;

  while (refreshRunning) {
    if (++blinkCounter == 5) {
      blinkCounter = 0;
      blink = !blink;
    }
    XSendEvent(vga.display, vga.win, False, 0, (XEvent *) &vga.expose);
    XFlush(vga.display);
    delay.tv_sec = 0;
    delay.tv_nsec = 100 * 1000 * 1000;
    nanosleep(&delay, &delay);
  }
  return NULL;
}


/**************************************************************/

/* server interface */


static int myArgc = 1;
static char *myArgv[] = {
  "eco32",
  NULL
};

static pthread_t monitorThread;
static pthread_t refreshThread;


static void vgaInit(void) {
  /* start monitor server in a separate thread */
  vga.argc = myArgc;
  vga.argv = myArgv;
  if (pthread_create(&monitorThread, NULL, server, NULL) != 0) {
    error("cannot start monitor server");
  }
  while (!installed) ;
  /* start refresh timer in another thread */
  refreshRunning = true;
  if (pthread_create(&refreshThread, NULL, refresh, NULL) != 0) {
    error("cannot start refresh timer");
  }
}


static void vgaExit(void) {
  refreshRunning = false;
  pthread_join(refreshThread, NULL);
  XSendEvent(vga.display, vga.win, False, 0, (XEvent *) &vga.shutdown);
  XSync(vga.display, False);
  pthread_join(monitorThread, NULL);
}


static void vgaWrite(int x, int y, int r, int g, int b) {
  XPutPixel(vga.image, x, y, RGB2PIXEL(r, g, b));
}


/**************************************************************/
/**************************************************************/


#define CELL_SIZE_X	8
#define CELL_SIZE_Y	16

#define TEXT_SIZE_X	(WINDOW_SIZE_X / CELL_SIZE_X)
#define TEXT_SIZE_Y	(WINDOW_SIZE_Y / CELL_SIZE_Y)


static Half text[TEXT_SIZE_Y][TEXT_SIZE_X];

static Byte font[] = {
#include "font"
};


static void updateCharacter(int x, int y, Half c) {
  int xx, yy;
  int i, j;
  Byte pixels;
  int r, g, b;

  xx = x * CELL_SIZE_X;
  yy = y * CELL_SIZE_Y;
  for (j = 0; j < CELL_SIZE_Y; j++) {
    pixels = font[(c & 0x00FF) * CELL_SIZE_Y + j];
    for (i = 0; i < CELL_SIZE_X; i++) {
      if ((pixels & (1 << (CELL_SIZE_X - 1 - i))) != 0 &&
          !((c & 0x8000) != 0 && blink)) {
        /* foreground */
        if (c & 0x0800) {
          /* intensify bit is on */
          r = (c & 0x0400) ? 255 : 73;
          g = (c & 0x0200) ? 255 : 73;
          b = (c & 0x0100) ? 255 : 73;
        } else {
          /* intensify bit is off */
          r = (c & 0x0400) ? 146 : 0;
          g = (c & 0x0200) ? 146 : 0;
          b = (c & 0x0100) ? 146 : 0;
        }
      } else {
        /* background */
        r = (c & 0x4000) ? 146 : 0;
        g = (c & 0x2000) ? 146 : 0;
        b = (c & 0x1000) ? 146 : 0;
      }
      vgaWrite(xx + i, yy + j, r, g, b);
    }
  }
}


static void blinkScreen(void) {
  static Bool previousBlink = false;
  int x, y;
  Half c;

  if (previousBlink == blink) {
    /* blink state didn't change, nothing to do */
    return;
  }
  previousBlink = blink;
  for (y = 0; y < TEXT_SIZE_Y; y++) {
    for (x = 0; x < TEXT_SIZE_X; x++) {
      /* update the character if it is blinking */
      c = text[y][x];
      if (c & 0x8000) {
        updateCharacter(x, y, c);
      }
    }
  }
}


static Half readCharAndAttr(int x, int y) {
  return text[y][x];
}


static void writeCharAndAttr(int x, int y, Half c) {
  text[y][x] = c;
  updateCharacter(x, y, c);
}


static char *screen[] = {
#include "screen"
};


static void loadSplashScreen(void) {
  int x, y;
  char *p;

  for (y = 0; y < TEXT_SIZE_Y; y++) {
    p = screen[y];
    for (x = 0; x < TEXT_SIZE_X; x++) {
      writeCharAndAttr(x, y, (Half) 0x0700 | (Half) *p);
      p++;
    }
  }
}


/**************************************************************/


Word displayRead(Word addr) {
  int x, y;
  Word data;

  if (debug) {
    cPrintf("\n**** DISPLAY READ from 0x%08X", addr);
  }
  if (!installed) {
    throwException(EXC_BUS_TIMEOUT);
  }
  x = (addr >> 2) & ((1 << 7) - 1);
  y = (addr >> 9) & ((1 << 5) - 1);
  if (x >= TEXT_SIZE_X || y >= TEXT_SIZE_Y) {
    throwException(EXC_BUS_TIMEOUT);
  }
  data = readCharAndAttr(x, y);
  if (debug) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void displayWrite(Word addr, Word data) {
  int x, y;

  if (debug) {
    cPrintf("\n**** DISPLAY WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  if (!installed) {
    throwException(EXC_BUS_TIMEOUT);
  }
  x = (addr >> 2) & ((1 << 7) - 1);
  y = (addr >> 9) & ((1 << 5) - 1);
  if (x >= TEXT_SIZE_X || y >= TEXT_SIZE_Y) {
    throwException(EXC_BUS_TIMEOUT);
  }
  writeCharAndAttr(x, y, data);
}


void displayReset(void) {
  if (!installed) {
    return;
  }
  cPrintf("Resetting Display...\n");
  loadSplashScreen();
}


void displayInit(void) {
  if (!installed) {
    vgaInit();
  }
  displayReset();
}


void displayExit(void) {
  if (!installed) {
    return;
  }
  vgaExit();
}

// Copyright (C) 1996 X Consortium
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// Except as contained in this notice, the name of the X Consortium shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from the X Consortium.
// X Window System is a trademark of X Consortium, Inc.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

typedef struct {
  unsigned int width;
  unsigned int height;
  int x;
  int y;
} custom_window_obj;

Window get_active_window(Display *p_display) {
  Atom net_active_window = XInternAtom(p_display, "_NET_ACTIVE_WINDOW", False); // Get atom (properties) and do not create one if it dne
  Atom actual_type; // Holds the type of property data returned by XGetWindowProperty
  int actual_format; // Holds the format of XGetWindowProperty that is returned
  unsigned long num_items; // Holds the number of items in the property data (focused windows)
  unsigned long bytes_after; // Holds the number of bytes remaining after the requested data
  unsigned char *prop_data; // Holds the ACTUAL property data returned by XGetWindowProperty
  Window active_window; // Holds the ID of the focused window (once extracted from XGetWindowProperty)
                         
  Status get_window_prop_result = XGetWindowProperty(
  p_display, DefaultRootWindow(p_display), net_active_window, // X11 conn, root window, an atom (_NET_ACTIVE_WINDOW prop)
  0, 1, False, // starting offset within prop, max num elements to retrieve, delete data after retrieving?
  XA_WINDOW, &actual_type, &actual_format, // expected prop data type, holds prop type, holds prop format (e.g. 8-bit, 16-bit, etc)
  &num_items, &bytes_after, &prop_data); // holds num of items returned, holds num bytes remaining in prop, holds actual prop data

  // Return the window if successful
  if (get_window_prop_result == Success && num_items == 1) {
    active_window = *(Window *)prop_data; // cast to get window ID
    XFree(prop_data); // Free the allocated memory for prop
    return active_window;
  }
                         
  return None;
}

// Takes in new window width as argv[1] and new window height as argv[2]
int main(int argc, char *argv[]) {
  // Printing help menu if required
  if (argc == 2 && (strncmp(argv[1], "--help", 2) == 0)) {
    printf("=== FrameFix ===\n");
    printf("Layout: ./FrameFix <new_window_width> <new_window_height> <new_window_y>\n");
    return 0;
  } 

  // Responding to insufficient num of arguments for program run
  if (argc != 4) {
    fprintf(stderr, "an invalid num of arguments were provided (requires 4 including bin)\n");
    return 1;
  }

  // Getting the width, height and y from user input (error checking included)
  char *width_arg_string = argv[1];
  int width_arg_int = atoi(width_arg_string);
  if (width_arg_int == 0) { // atoi returns 0 when failure (0 not useful if actually 0 in this case)
    fprintf(stderr, "failed to convert the provided width argument to an int\n");
    return 1;
  }
  char *height_arg_string = argv[2];
  int height_arg_int = atoi(height_arg_string); // atoi returns 0 when failure (0 not useful if actually 0 in this case)
  if (height_arg_int == 0) {
    fprintf(stderr, "failed to convert the provided height argument to an int\n");
    return 1;
  }
  char *y_arg_string = argv[3];
  int y_arg_int = atoi(y_arg_string); // atoi returns 0 when failure (0 not useful if actually 0 in this case)
  if (y_arg_int == 0) {
    fprintf(stderr, "failed to convert the provided y argument to an int\n");
    return 1;
  }

  // Open connection to the X server
  char *display_name = NULL; // Select default display
  Display *p_display = XOpenDisplay(display_name); 
  if (p_display == NULL) {
      fprintf(stderr, "unable to open X11 display\n");
      return 1;
  }

  Window focused_window = get_active_window(p_display);
  if (focused_window == None || focused_window == BadWindow) {
    fprintf(stderr, "failed to retrieve the currently focused window\n");
    return 1;
  }

  // Getting screen dimensions
  Screen *p_screen = DefaultScreenOfDisplay(p_display);
  int screen_width = p_screen->width;

  // Determining how much to move the focused window by --> Centring on screen for simplicities sake
  int middle_screen_width = screen_width/2;
  int new_x = middle_screen_width - (width_arg_int/2); // Width centre
  // NOTE: new_y not configured due to taskbar (panel) being treated as a window (screen res != workarea) --> window not centred in workarea
  
  // Creating a struct to hold new window location/sizing info
  custom_window_obj new_size = { .width = width_arg_int, .height = height_arg_int, .x = new_x, .y = y_arg_int };

  // Moving the focused window
  int move_status = XMoveWindow(p_display, focused_window, new_size.x, new_size.y);
  if (move_status == 0) {
    fprintf(stderr, "failed to move the focused window");
    return 1;
  }

  // Resizing the focused window
  int resize_status = XResizeWindow(p_display, focused_window, new_size.width, new_size.height);
  if (resize_status == 0) {
    fprintf(stderr, "failed to resize the focused window");
    return 1;
  }

  // Closing the connection to the X11 server display
  XCloseDisplay(p_display);
  return 0;
}


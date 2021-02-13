
// *neode.onsave* setgo gcc -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -fsyntax-only gtkfiledialog.hpp

#ifndef HAVE_GTKFILEDIALOG_H
#define HAVE_GTKFILEDIALOG_H

#include "filedialog.hpp"
#include "gtk/gtk.h"
#include <string>

using namespace std;

/**
 * GtkFileDialog - implementation of FileDialog using GTK3
 *
 */
class GtkFileDialog : public FileDialog
{
public:
    /**
     * Initialize a new GtkFileDialog
     */
    GtkFileDialog();
    ~GtkFileDialog();
    
    /**
     * Show the dialog using whatever path the dialog feels like
     */
    void show();
    
    /**
     * Show the dialog starting at a specific given path
     */
    void show(string path);
    
    /**
     * Response indicates whether the user clicked on "OK" or "Cancel"
     */
    FileDialog::Response getResponse();
    
    /**
     * If a file was selected and the user clicked on "OK", stores filename in parameter and returns True
     * otherwise, returns false
     */
    bool getFilename(string& filename);

private:
    GtkWidget* dialog;
    
    /**
     * Response from GTK ends up here
     */
    gint res;
};

#endif


// *neode.onsave* setgo gcc -fsyntax-only filedialog.hpp

#ifndef HAVE_FILEDIALOG_H
#define HAVE_FILEDIALOG_H

#include <string>

using namespace std;

/**
 * FileDialog interface
 *
 */
class FileDialog
{
public:
    enum Response {Accept, Cancel};
    
    /**
     * Show the dialog using whatever path the dialog feels like
     */
    virtual void show() = 0;
    
    /**
     * Show the dialog starting at a specific given path
     */
    virtual void show(string path) = 0;
    
    /**
     * Response indicates whether the user clicked on "OK" or "Cancel"
     */
    virtual FileDialog::Response getResponse() = 0;
    
    /**
     * If a file was selected and the user clicked on "OK", stores filename in parameter and returns True
     * otherwise, returns false
     */
    virtual bool getFilename(string& filename) = 0;
    virtual ~FileDialog(){}
};

#endif

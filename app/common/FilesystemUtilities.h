#ifndef FilesystemUtilities_H
#define FilesystemUtilities_H

#include "wx/string.h"
#include <deque>

// Extremely Windows-specific.
// TODO unit tests.
namespace rootmap
{
    class FilesystemUtilities
    {
    public:
        static std::string createDirIfNonexistent(wxString absoluteBasePath, std::deque<wxString>& subdirectories);
        /**
            Given a directory path `basePath`, desired child directories `subdirectories` and ROOTMAP `version`,
            returns the absolute path of the combination of `basePath` and `subdirectories`.
            If `basePath` is a relative path, the absolute path will be rooted in the ROOTMAP installation directory.
            Note that `subdirectories` is a deque to facilitate the implementation, so that the provided subdirs
            end up being created as children of the evaluated absolute base path.

            Creates directories if they do not exist.

            Example 1. To resolve C:\Temp\foo\bar\baz:
            
            std::deque<string> subdirectories;
            subdirectories.push_front("baz");
            subdirectories.push_front("bar");
            subdirectories.push_front("foo");
            std::string absolutePath = resolveAbsolutePath("C:\\Temp", subdirectories, "0.1.2");

            Example 1. To resolve "Output\hello" directory in ROOTMAP install directory:
            
            std::deque<string> subdirectories;
            subdirectories.push_front("hello");
            std::string absolutePath = resolveAbsolutePath("Output", subdirectories, "0.1.2"); // Returns "C:\Users\[username]\AppData\Roaming\ROOTMAP\v0.1.2\Output\hello"
        */
        static std::string resolveAbsolutePath(wxString basePath, std::deque<wxString>& subdirectories, wxString version);
    };
}
#endif

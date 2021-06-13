#include "app/common/FilesystemUtilities.h"
#include "core/common/RmAssert.h"
#include "core/utility/Utility.h"

#if defined( WIN32 )
#pragma warning( push )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4786 )
#endif    // defined( WIN32 ) 

namespace rootmap
{
    std::string FilesystemUtilities::createDirIfNonexistent(wxString absoluteBasePath, std::deque<wxString>& subdirectories)
    {
        if (subdirectories.empty())
        {
            return absoluteBasePath.EndsWith("\\")
                ? absoluteBasePath
                : (absoluteBasePath + "\\");
        }

        wxString absolutePath = absoluteBasePath + "\\" << subdirectories.front();
        subdirectories.pop_front();
        // Use Windows file attributes for convenience
        DWORD fileAttributes = GetFileAttributes(absolutePath.wc_str());
        // If directory doesn't exist, try creating it.
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
        {
            if (!CreateDirectory(absolutePath.wc_str(), __nullptr))
            {
                wxString errString = "Could not find or create path ";
                errString << absolutePath << "\n" << GetLastError();
                RmAssert(false, errString);
            }
        }
        return createDirIfNonexistent(absolutePath, subdirectories);
    }

    std::string FilesystemUtilities::resolveAbsolutePath(wxString basePath, std::deque<wxString>& subdirectories, wxString version)
    {
        wxString outPath = basePath;

        // Assume to be relative if path does not contain a colon.
        if (outPath.find(":") == std::string::npos)
        {
            outPath = getenv("APPDATA");
            if (!version.Lower().StartsWith("v"))
            {
                version = "v" + version;
            }
            subdirectories.push_front(basePath);
            subdirectories.push_front(version);
            subdirectories.push_front("ROOTMAP");
        }

        // Ensure path only uses backslashes
        outPath.Replace("/", "\\");
        // Ensure path has no trailing backslashes
        if (outPath.EndsWith("\\"))
        {
            outPath = outPath.BeforeLast('\\');
        }

        return createDirIfNonexistent(outPath, subdirectories);
    }
}

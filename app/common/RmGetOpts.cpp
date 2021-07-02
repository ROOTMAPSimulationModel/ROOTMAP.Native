/**
  * RmGetOpts.cpp
  *
  * A singleton class that stores all the runtime parameters, a.k.a. command
  * line options. Runtime parameters can come not just from the command line,
  * but also be set internally.
  *
  * Because the class is a singleton, runtime parameters can be accessed from
  * anywhere in the code instead of passing them around as parameters
  */

#include "app/common/RmGetOpts.h"
#include "app/common/FilesystemUtilities.h"
#include "core/common/RmAssert.h"

namespace rootmap
{
    RmGetOpts::RmGetOpts()
    {
        // Set fallbacks for directories, in case they are not set externally
        std::deque<wxString> subdirs;
        setOpt(RMGETOPTS_WORKINGDIR, FilesystemUtilities::resolveAbsolutePath("", subdirs, getVersion()).c_str());

        subdirs.clear();
        subdirs.push_back("Output");
        setOpt(RMGETOPTS_OUTPUTDIR, FilesystemUtilities::resolveAbsolutePath("", subdirs, getVersion()).c_str());

        subdirs.clear();
        subdirs.push_back("Configurations");
        subdirs.push_back("default");
        setOpt(RMGETOPTS_CONFIGDIR, FilesystemUtilities::resolveAbsolutePath("", subdirs, getVersion()).c_str());
    }

    RmGetOpts& RmGetOpts::getInstance()
    {
        static RmGetOpts rp; // the one-and-only instance
        return rp;
    }

    std::string RmGetOpts::getOpt(const char* name)
    {
        RmAssert(name != __nullptr, "Named passed was NULL");
        RmAssert(name[0] != '\0', "Name passed was NULL");

        const static std::string empty;

        ParamDetailsMap::const_iterator i = m_Params.find(name);
        if (i == m_Params.end())
        {
            // LOG( "RmGetOpts::get(): param %s has not been set", name );

            return empty;
        }

        // LOG( "RmGetOpts::get(): param %s == %s", name, i->second.c_str() );

        return i->second;
    }


    void RmGetOpts::setOpt(const char* name, const char* value)
    {
        RmAssert(name != __nullptr, "Name passed was NULL");
        RmAssert(name[0] != '\0', "Name passed was NULL");
        RmAssert(value != __nullptr, "Value passed was NULL");

        std::string oldValue;

        { // first threadguard scope
            // Prevent an insert occuring while a read is happening!

            // LOG( "RmGetOpts::set(): param %s = %s", name, value );

            // Remember the old value.
            ParamDetailsMap::const_iterator cit = m_Params.find(name);
            if (cit != m_Params.end())
            {
                oldValue = cit->second;
            }

            // Set the new value.
            m_Params[name] = value;
        }
    }

    bool RmGetOpts::isOptSet(const char* name)
    {
        RmAssert(name != __nullptr, "Name passed was NULL");
        RmAssert(name[0] != '\0', "Name passed was NULL");

        // We want to protect the find from an insert

        bool isSet = m_Params.find(name) != m_Params.end();

        // LOG( "RmGetOpts::isSet(): isSet(%s) = %d", name, isSet );

        return isSet;
    }


    void RmGetOpts::getAllOpts(ParamVector& params)
    {
        params.clear();

        // We want to protect the find from an insert

        ParamDetailsMap::const_iterator it;
        int i = 0;
        for (it = m_Params.begin(); it != m_Params.end(); ++it)
        {
            params.resize(i + 1);
            params[i].name = it->first;
            params[i].value = it->second;
            ++i;
        }
    }


    bool RmGetOpts::parseCmdLine(int argc, char* argv[])
    {
        // LOG("parseCmdLine(argc, argv)");

        // Convert the cmdline to a vector of string arguments and then parse
        // the args themselves

        std::vector<std::string> args;
        convertCmdLine(argc, argv, args);
        return parseCmdLine(args);
    }

    bool RmGetOpts::parseCmdLine(const char* cmdline)
    {
        // LOG("parseCmdLine(cmdline)");
        RmAssert(cmdline != __nullptr, "Command line passed was NULL");

        // Convert the cmdline to a vector of string arguments and then parse
        // the args themselves

        std::vector<std::string> args;
        convertCmdLine(cmdline, args);
        return parseCmdLine(args);
    }


    bool RmGetOpts::parseCmdLine(const std::vector<std::string>& args)
    {
        // LOG( "parseCmdLine(args)");

        // First arg is USUALLY the program name

        unsigned int i = 0;
        if (args.size() > 0)
        {
            if (args[0].size() > 0)
            {
                if (args[0][0] != '-')
                {
                    setOpt(RMGETOPTS_PROGNAME, args[0].c_str());
                    i = 1;
                }
            }
        }

        // The rest should be standard parameters

        std::string name;
        std::string value;
        const char* str;

        // Do the DebugUtil stuff in order to avoid unneccessary files being
        // created or disabled debug level messages being logged.
        std::string debugDisabledName;
        std::string debugDisabledValue;
        std::string debugLevelFileName;
        std::string debugLevelFileValue;
        std::string debugPidFileName;
        std::string debugPidFileValue;
        std::string debugFileName;
        std::string debugFileValue;

        for (; i < args.size(); ++i)
        {
            str = args[i].c_str();
            if (!extractArg(str, name, value))
            {
                // LOG( "parseCmdLine(): failed");
                return false;
            }

            if (!name.empty())
            {
                setOpt(name.c_str(), value.c_str());
            }
        }

        return true;
    }


    bool RmGetOpts::checkUsage(const char* usage, std::string& error)
    {
        // LOG("CheckUsage()");
        RmAssert(usage != __nullptr, "Usage passed was NULL");

        error = "";

        // Split the usage string into whitespace-separated arguments

        std::vector<std::string> args;
        convertCmdLine(usage, args);

        if (args.empty())
        {
            error = "You have a bad usage string!";
            return false;
        }

        std::string name;
        std::string value;
        unsigned int i = 0;
        bool optional;

        // First argument may be the program name (optional), in which case
        // we ignore it

        if (args[i].size() > 0 && args[i][0] != '-' && args[i][0] != '[')
        {
            ++i; // skip first arg
        }

        // Extract the name/value components of each argument. The whole name=value
        // argument can be surrounded by square brackets, in which case it is
        // optional

        for (; i < args.size(); ++i)
        {
            if (!extractArg(args[i].c_str(), name, value, optional))
            {
                error = "You have a bad usage string!";
                return false;
            }

            // blank name is okay as long as there is no value

            if (name.empty())
            {
                if (value.empty())
                {
                    continue; // extract the next argument
                }
                error = "You have a bad usage string!";
                return false;
            }

            // If the parameter has not been set then we have an error, unless
            // the parameter is optional (in which case we can just skip to the
            // next argument)

            if (!isOptSet(name.c_str()))
            {
                if (optional)
                {
                    continue; // flies in the buttermilk...
                }
                error = "Parameter ";
                error += name;
                error += " has not been specified";
                return false;
            }
            if (value.empty()) // this is a flag parameter
            {
                value = getOpt(name.c_str());
                if (!value.empty())
                {
                    error = "A value has been specified for flag parameter ";
                    error += name;
                    return false;
                }
            }
        }

        return true;
    }


    bool RmGetOpts::extractArg(const char* str, std::string& name, std::string& value)
    {
        bool status = true;
        name = "";
        value = "";

        // Skip leading whitespace

        while (isspace(*str))
        {
            ++str;
        }

        // Expect the name to start with two hyphens followed by an alpha

        if (strncmp(str, "--", 2) == 0)
        {
            str += 2;
        }
        else
        {
            status = false;
        }

        if (!isalpha(*str))
        {
            status = false;
        }

        // Extract the name. Hyphens are skipped but cause the next character
        // to be upshifted

        bool upshift = true;
        while (status && *str != '\0' && *str != '=' && !isspace(*str))
        {
            if (*str == '-')
            {
                upshift = true;
            }
            else if (upshift)
            {
                name += toupper(*str);
                upshift = false;
            }
            else
            {
                name += *str;
            }
            ++str;
        }

        // If we have an "=" then extract the value

        if (status && *str == '=')
        {
            ++str;
            while (*str != '\0')
            {
                value += *str;
                ++str;
            }
        }

        return status;
    }


    bool RmGetOpts::extractArg(const char* str, std::string& name,
        std::string& value, bool& optional)
    {
        optional = false;

        // If the first non-white character is an opening square bracket then the
        // parameter declaration to follow is optional

        while (isspace(*str))
        {
            ++str;
        }

        if (*str == '[')
        {
            optional = true;
            ++str;
        }

        // Extract the argument from the given string

        if (!extractArg(str, name, value))
        {
            return false;
        }

        // Ignore if the name is empty

        if (name.empty())
        {
            return true;
        }

        // If we have an optional parameter then make sure we have a matching
        // bracket at the end of the name component (or at the end of the value
        // if a value has been specified)

        if (optional)
        {
            std::string::size_type i = value.size();
            if (i > 0)
            {
                // trim closing brace from value

                i--;
                if (value[i] != ']')
                {
                    return false; // no matching brace
                }
                value.erase(i);
            }
            else
            {
                // trim closing brace from name

                i = name.size() - 1;
                if (name[i] != ']')
                {
                    return false; // no matching brace
                }
                name.erase(i);
            }
        }

        return true;
    }

    void RmGetOpts::convertCmdLine(int argc, char* argv[], std::vector<std::string>& args)
    {
        args.resize(0);
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    void RmGetOpts::convertCmdLine(const char* cmdline, std::vector<std::string>& args)
    {
        RmAssert(cmdline != __nullptr, "Command line passed was NULL");

        args.resize(0);

        // Spaces are allowed in the value part of the argument.

        std::string arg;
        bool inValue; // True if we're up to the "value" part of the arg.
        char nextChar; // The next character in the command line.
        bool reachedDoubleDash; // True if we've reached a '--'.

        while (*cmdline != '\0')
        {
            inValue = false;
            nextChar = '\0';
            arg = "";
            reachedDoubleDash = false;

            // Skip leading whitespace
            while (isspace(*cmdline))
            {
                ++cmdline;
            }

            if (*cmdline != '\0')
            {
                nextChar = *(cmdline + 1);
            }

            // The current argument ends when we reach the end of the
            // command line, or we reach a space when processing the name of
            // the arg, or when we reach a -- in the value part of the arg.
            while (*cmdline != '\0' && ((!inValue && !isspace(*cmdline))
                || inValue && !reachedDoubleDash))
            {
                if (*cmdline == '=')
                {
                    inValue = true;
                }
                arg += *cmdline;
                ++cmdline;
                if (*cmdline != '\0')
                {
                    nextChar = *(cmdline + 1);
                }

                reachedDoubleDash = (*cmdline == '-' && nextChar == '-');
            }

            // Trim trailing whitespace
            unsigned int i = arg.find_last_not_of(" ");
            std::string trimmedArg = arg.substr(0, i + 1);

            if (!arg.empty())
            {
                args.push_back(trimmedArg);
            }
        }
    }

    wxString RmGetOpts::getVersion()
    {
        // Ensure the version number we return does not start with v or V.
        if (rootmapVersion.rfind("v", 0) == 0 || rootmapVersion.rfind("V", 0) == 0) {
            return rootmapVersion.substr(1);
        }
        return rootmapVersion;
    }
} /* namespace rootmap */

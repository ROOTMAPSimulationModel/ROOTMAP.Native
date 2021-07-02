/**
  * RmGetOpts.h
  *
  *
  * Last modification: $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
  * Last modified by:  $Author: robert $
  *
  * A singleton class that stores all the command line parameters.
  *
  * Because the class is a singleton, command line parameters can be accessed from
  * anywhere in the code instead of passing them around as parameters
  *
  */
#ifndef RmGetOpts_H
#define RmGetOpts_H

#if defined( WIN32 )
#pragma warning( push )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4786 )
#endif // #if defined( WIN32 )

#include <list>
#include <map>
#include <deque>
#include <sstream>
#include <string>
#include <vector>
#include "wx/string.h"

extern const std::string rootmapVersion;

namespace rootmap
{
    class RmGetOpts
    {
    public:

        /**
          * getInstance
          *
          * Returns a single static instance of RmGetOpts
          *
          * @return The one and only RmGetOpts instance
          */
        static RmGetOpts& getInstance();

        /**
          * ~RmGetOpts
          *
          * Destructor
          */
        virtual ~RmGetOpts()
        {
        }

        /**
          * set
          *
          * Add a parameter
          *
          * @param name Name of parameter
          * @param value Value of parameter
          *
          * Pre: name and value are not NULL
          */
        void setOpt(const char* name, const char* value);

        /**
          * get
          *
          * Retrieve a parameter value
          *
          * @return Value of parameter
          *
          * @param name Name of parameter
          *
          * Pre: name is not NULL
          */
        std::string getOpt(const char* name);

        /**
          * isSet
          *
          * Determine whether a parameter with the given name has been set
          *
          * @return True (parameter set), False (parameter not set)
          *
          * @param name Name of parameter
          *
          * Pre: name is not NULL
          */
        bool isOptSet(const char* name);

        struct ParamNameValue
        {
            std::string name;
            std::string value;
        };

        typedef std::vector<ParamNameValue> ParamVector;

        /**
          * getAll
          *
          * Retrieve all the parameters
          *
          * @param params Vector of Name/Value parameter pairs
          *
          * Pre: name is not NULL
          */
        void getAllOpts(ParamVector& params);

        /**
          * COMMAND LINE MAPPING
          *
          * Command line parameters are expected to conform to the gnu format -
          * parameter names are prefixed with "--" and words within a parameter name
          * are separated with a single hyphen. Names and values are separated with
          * an "=". Values are optional - if there is no "=" then the parameter is a
          * flag.
          *
          * eg "foo --entity-name=BAR --verbose"
          *
          * If the first argument does not start with "--" then it is assumed to be
          * the program name.
          *
          * When we parse the command line we convert the parameter names to
          * CamelHump notation - the leading "--" is discarded, the first letter is
          * upshifted, subsequent hyphens are discarded and the succeeding character
          * upshifted. The conversion only applies to the name, the value is left
          * unmodified. The program name is handled as the value of a parameter with
          * name RPARAM_PROGNAME. In the above example we would set parameter
          * RPARAM_PROGNAME to "foo", parameter "EntityName" to "BAR" and
          * parameter "Verbose" to "".

          * The following methods parse the given command line (several command line
          * representations) and set corresponding parameters. Return false if the
          * command line does not conform to the expected format
          *
          */

          /**
            * parseCmdLine
            *
            * Parses the command line parameters.
            * See COMMAND LINE MAPPING comments above.
            *
            * @return Success of parse (true: success)
            *
            * @param argc Number of parameters
            * @param argv Vector of parameters
            */
        bool parseCmdLine(int argc, char* argv[]);

        /**
          * parseCmdLine
          *
          * Parses the command line parameters.
          * See COMMAND LINE MAPPING comments above.
          *
          * @return Success of parse (true: success)
          *
          * @param cmdline Number of parameters
          * @param argv Vector of parameters
          *
          * Pre: cmdline is not NULL
          */
        bool parseCmdLine(const char* cmdline);

        /**
          * parseCmdLine
          *
          * Parses the command line parameters.
          * See COMMAND LINE MAPPING comments above.
          *
          * @return Success of parse (true: success)
          *
          * @param args Vector of strings consisting of
          *             parameters and parameter values
          *
          * Pre: cmdline is not NULL
          */
        bool parseCmdLine(const std::vector<std::string>& args);

        /**
          * checkUsage
          *
          * This method checks to see whether the parameters specified in the given
          * usage string have been set. It ignores the leading program name and uses
          * the values only to distinguish between parameters that take values and
          * flag parameters. Returns true if all specified parameters have been set
          * correctly. For instance, a usage string "foo --entity-name=BAR --verbose"
          * would return true if parameter "EntityName" has been set and parameter
          * "Verbose" has been set to a blank string. The error message will be set
          * to blank if the method succeeds
          *
          * You can also make parameters optional by surrounding them with square
          * brackets. eg "foo --entity-name=BAR [--verbose]". In this case failure
          * to specify a parameter is not an error, but providing a non-blank value
          * to the flag parameter would be
          *
          * @return True (correct usage string), False (incorrect usage string)
          *
          * @param usage The usage string
          * @param error The reason for the error
          *
          * Pre: The usage cannot be NULL
          */
        bool checkUsage(const char* usage, std::string& error);

        wxString getVersion();

    private:

        /**
          * constructor
          *
          * Returns a single static instance of this class
          *
          * @return The one and only RmGetOpts instance
          */
        RmGetOpts();

        typedef std::map<std::string, std::string> ParamDetailsMap;

        ParamDetailsMap m_Params;

        /**
          * convertCmdLine
          *
          * Split the given cmdline string into a vector of whitespace-separated
          * arguments
          * TODO: handle escaped whitespace (quotes, backslash)
          *
          * @param cmdline The command line
          * @param args Vector of arguments
          *
          * Pre: The command line cannot be NULL
          */
        void convertCmdLine(const char* cmdline, std::vector<std::string>& args);

        /**
          * convertCmdLine
          *
          * Convert the standard argc/argv cmdline to a vector of arguments
          *
          * @param argc Number of arguments
          * @param args Vector of arguments
          */
        void convertCmdLine(int argc, char* argv[], std::vector<std::string>& args);

        /**
          * extractArg
          *
          * Extract a single argument from the given string, split it into name
          * and value components, and CamelHump the parameter name (as described
          * above). Everything after the "=" will be assigned to the value, including
          * whitespace. Returns true if the argument corresponds to the standard
          * format. The input string will be advanced to the next argument (points
          * to the end of the string if no more arguments)
          *
          * @return True (argument corresponds to standard format), False (it doesn't)
          *
          * @param str   Single argument
          * @param name  Parameter name
          * @param value Parameter value
          */
        bool extractArg(const char* str, std::string& name, std::string& value);

        /**
          * extractArg
          *
          * Extract an argument as above, except that we allow the argument to be
          * surrounded by square brackets indicating that it is optional. This is
          * called by CheckUsage(). eg "[--name=value]" "[--name]"
          *
          * @return True (argument corresponds to standard format), False (it doesn't)
          *
          * @param str      Single argument
          * @param name     Parameter name
          * @param value    Parameter value
          * @param optional Flag indication if parameter is optional
          */
        bool extractArg(const char* str, std::string& name, std::string& value, bool& optional);
    }; // class RmGetOpts
} /* namespace rootmap */

constexpr auto RMGETOPTS_PROGNAME = "ProgramName";
constexpr auto RMGETOPTS_WORKINGDIR = "WorkingDir";
constexpr auto RMGETOPTS_CONFIGDIR = "ConfigDir";
constexpr auto RMGETOPTS_CONFIGFILE = "ConfigFile";
constexpr auto RMGETOPTS_OUTPUTDIR = "OutputDir";
constexpr auto RMGETOPTS_LOGFILENAME = "LogFileName";
constexpr auto RMGETOPTS_SEED = "Seed";

#if defined( WIN32 )
#pragma warning( pop )
#endif // #if defined( WIN32 ) 

#endif  // RmGetOpts_H

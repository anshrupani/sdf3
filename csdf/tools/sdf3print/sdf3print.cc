/*
 *  TU Eindhoven
 *  Eindhoven, The Netherlands
 *
 *  Name            :   sdf3print.cc
 *
 *  Author          :   Sander Stuijk (sander@ics.ele.tue.nl)
 *
 *  Date            :   April 23, 2007
 *
 *  Function        :   (C)SDFG print functionality
 *
 *  History         :
 *      23-04-07    :   Initial version.
 *
 * $Id: sdf3print.cc,v 1.1.1.1.2.2 2010-04-25 01:21:16 mgeilen Exp $
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * In other words, you are welcome to use, share and improve this program.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them.   Happy coding!
 */

#include "sdf3print.h"
#include "sdf/sdf.h"
#include "csdf/csdf.h"
namespace CSDF
{
    typedef struct _CPair
    {
        CString key;
        CString value;
    } CPair;

    typedef list<CPair>         CPairs;
    typedef CPairs::iterator    CPairsIter;

    /**
     * Settings
     * Struct to store program settings.
     */
    typedef struct _Settings
    {
        // Input file with graph
        CString graphFile;

        // Output file
        CString outputFile;

        // Switch argument(s) given to analysis algorithm
        CPairs arguments;

        // Application graph
        CNode *xmlAppGraph;
    } Settings;

    /**
     * settings
     * Program settings.
     */
    Settings settings;

    /**
     * helpMessage ()
     * Function prints help message for the tool.
     */
    void helpMessage(ostream &out)
    {
        out << "SDF3 " << TOOL << " (version " << DOTTED_VERSION ")" << endl;
        out << endl;
        out << "Usage: " << TOOL << " --graph <file> --format <type>";
        out << " [--output <file>]" << endl;
        out << "   --graph  <file>     input SDF graph" << endl;
        out << "   --output <file>     output file (default: stdout)" << endl;
        out << "   --format <type>     output format:" << endl;
        out << "       txt                                       " << endl;
        out << "       buffy                                     " << endl;
    }

    /**
     * parseSwitchArgument ()
     * The function parses the string 'arguments' into a sequence of (arg, value)
     * pairs. The syntax as as follows:
     *
     * pair := key(value)
     * arg := pair,pair,...
     *
     * Note: value may be a pair itself, but this is not expanded into a set of
     * pairs (i.e. nested pairs are not supported).
     */
    CPairs parseSwitchArgument(CString arguments)
    {
        CPairs pairs;
        CPair p;

        while (arguments.size() != 0)
        {
            char c;
            p.key = "";
            p.value = "";

            // Get key from argument string
            do
            {
                c = arguments[0];
                arguments = arguments.substr(1);
                if (c == ',' || c == '(')
                    break;
                p.key += c;
            }
            while (arguments.size() != 0);

            // Is next part of argument a value?
            if (c == '(')
            {
                CString::size_type ePos = 0;
                int level = 1;

                // Find the matching closing brace
                while (level != 0 && arguments.size() != 0)
                {
                    if (arguments.operator []((int) ePos) == ')')
                        level--;
                    else if (arguments.operator []((int) ePos) == '(')
                        level++;

                    // Next
                    ePos++;
                }

                // Closing brace found?
                if (level != 0)
                    throw CException("Missing closing brace in value of argument.");

                // Get value
                p.value = arguments.substr(0, ePos - 1);

                // More arguments left?
                if (arguments.size() > ePos)
                    arguments = arguments.substr(ePos + 1);
                else
                    arguments = "";
            }

            // Add pair to list of pairs
            pairs.push_back(p);
        }

        return pairs;
    }

    /**
     * parseCommandLine ()
     * The function parses the command line arguments and add info to the
     * supplied settings structure.
     */
    void parseCommandLine(int argc, char **argv)
    {
        int arg = 1;

        while (arg < argc)
        {
            // Configuration file
            if (argv[arg] == CString("--graph") && arg + 1 < argc)
            {
                arg++;
                settings.graphFile = argv[arg];
            }
            else if (argv[arg] == CString("--output") && arg + 1 < argc)
            {
                arg++;
                settings.outputFile = argv[arg];
            }
            else if (argv[arg] == CString("--format") && arg + 1 < argc)
            {
                arg++;
                settings.arguments = parseSwitchArgument(argv[arg]);
            }
            else
            {
                helpMessage(cerr);
                throw CException("");
            }

            // Next argument
            arg++;
        }
    }

    /**
     * loadApplicationGraphFromFile ()
     * The function returns a pointer to an XML data structures contained in the
     * supplied file that describes the SDFG.
     */
    CNode *loadApplicationGraphFromFile(CString &file, CString module)
    {
        CNode *appGraphNode, *sdf3Node;
        CDoc *appGraphDoc;

        // Open file
        appGraphDoc = CParseFile(file);
        if (appGraphDoc == NULL)
            throw CException("Failed loading application from '" + file + "'.");

        // Locate the sdf3 root element and check module type
        sdf3Node = CGetRootNode(appGraphDoc);
        if (CGetAttribute(sdf3Node, "type") != module)
        {
            throw CException("Root element in file '" + file + "' is not "
                             "of type '" + module + "'.");
        }

        // Get application graph node
        appGraphNode = CGetChildNode(sdf3Node, "applicationGraph");
        if (appGraphNode == NULL)
            throw CException("No application graph in '" + file + "'.");

        return appGraphNode;
    }

    /**
     * initSettings ()
     * The function initializes the program settings.
     */
    void initSettings(int argc, char **argv)
    {
        // Parse the command line
        parseCommandLine(argc, argv);

        // Check required settings
        if (settings.graphFile.empty() || settings.arguments.size() == 0)
        {
            helpMessage(cerr);
            throw CException("");
        }

        // Load application graph
        settings.xmlAppGraph = loadApplicationGraphFromFile(settings.graphFile,
                               MODULE);
    }

    /**
     * printCSDFG ()
     * The function outputs the CSDF graph in the requested format.
     */
    void printCSDFG(TimedCSDFgraph *g, CPairs &format, ostream &out)
    {
        if (format.front().key == "txt")
        {
            g->print(out);
        }
        else if (format.front().key == "buffy")
        {
            outputCSDFGasBuffyModel(g, out);
        }
        else
        {
            throw CException("Invalid output format requested.");
        }
    }

    /**
     * printCSDFG ()
     * The function outputs the CSDF graph.
     */
    void printCSDFG(ostream &out)
    {
        CNode *csdfNode, *csdfPropertiesNode;
        TimedCSDFgraph *csdfGraph;

        // Find csdf graph in XML structure
        csdfNode = CGetChildNode(settings.xmlAppGraph, "csdf");
        if (csdfNode == NULL)
            throw CException("Invalid xml file - missing 'csdf' node");
        csdfPropertiesNode = CGetChildNode(settings.xmlAppGraph, "csdfProperties");

        // Construction CSDF graph model
        csdfGraph = constructTimedCSDFgraph(csdfNode, csdfPropertiesNode);

        // The actual printing...
        printCSDFG(csdfGraph, settings.arguments, out);

        // Cleanup
        delete csdfGraph;
    }
} //namespace CSDF
/**
 * main ()
 * It does none of the hard work, but it is very needed...
 */
int main(int argc, char **argv)
{
    int exit_status = 0;
    ofstream out;

    try
    {
        // Initialize the program
        initSettings(argc, argv);

        // Set output stream
        if (!settings.outputFile.empty())
            out.open(settings.outputFile.c_str());
        else
            ((ostream &)(out)).rdbuf(cout.rdbuf());

        // Perform requested actions
        printCSDFG(out);
    }
    catch (CException &e)
    {
        cerr << e;
        exit_status = 1;
    }

    return exit_status;
}


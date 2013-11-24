/*
 * generators_common.cpp
 *
 * Tool used to parse an input template .h and .cpp files
 * and replace tokens for output files based on parameters.
 *
 * See LICENCE.md for Copyright information.
 */

#include <algorithm>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "generators_common.h"

namespace po = boost::program_options;
namespace yg = yiqi::generators;
namespace ygu = yiqi::generators::util;
namespace ygt = yiqi::generators::transformations;

void
yg::GetOptions (int                       argc,
                char                      **argv,
                std::string               &input,
                std::string               &output,
                std::vector <std::string> &tools)
{
    po::options_description description ("Options");

    description.add_options ()
        ("input,i",
         po::value<std::string> (&input)->required (),
         "File to read template from")
        ("output,o",
         po::value<std::string> (&output)->required (),
         "File to write result to")
        ("tool,t",
         po::value<std::vector<std::string>> (&tools)->multitoken(),
         "Tools");

    po::positional_options_description positionalDescription;
    positionalDescription.add ("tool", -1);

    po::variables_map vm;
    po::command_line_parser parser (argc, argv);
    
    po::store (parser.options (description)
                   .positional (positionalDescription)
                   .run (),
               vm);

    try
    {
        po::notify (vm);

        /* The tools need to be in sorted order when inserting
         * them into lists for various efficiency reasons */
        std::sort (tools.begin (), tools.end ());
    }
    catch (std::exception const &e)
    {
        std::stringstream ss;

        ss << e.what () << std::endl;
        ss << "Options Available: " << description << std::endl;

        throw std::logic_error (ss.str ());
    }
}

std::string
yg::ReadInputFile (std::string const &input)
{
    std::fstream inputFile (input.c_str (),
                            std::ios_base::in);

    char inputFileBuffer[256];
    std::stringstream inputFileData;

    while (!inputFile.eof ())
    {
        if (inputFile.fail ())
        {
            std::stringstream ss;

            ss << "Failed to read file "
               << input;

            throw std::runtime_error (ss.str ());
        }

        inputFile.getline (inputFileBuffer, 256);
        inputFileData << inputFileBuffer << "\n";
    }

    return inputFileData.str ();
}

void
yg::WriteToOutputFile (std::string const &output,
                       std::string const &data)
{
    std::fstream outputFile (output.c_str (),
                             std::ios_base::out |
                             std::ios_base::trunc);

    outputFile << data;

    if (outputFile.fail ())
    {
        std::stringstream ss;
        ss << "Written file " << output << " may be inconsistent!";

        throw std::runtime_error (ss.str ());
    }
}

void
ygt::DeleteComments (std::string &str)
{
    size_t beginCommentPosition = str.find ("/*");

    while (beginCommentPosition != std::string::npos)
    {
        size_t endCommentPosition = str.find ("*/");

        if (endCommentPosition == std::string::npos)
            throw std::runtime_error ("Comment does not end, "
                                      "malformed file?");

        std::string::iterator beginCommentIterator (str.begin ());
        std::string::iterator endCommentIterator (str.begin ());

        std::advance (beginCommentIterator, beginCommentPosition);

        /* We need to advance 2 extra since std::string::find gives us
         * the position of the character where the match starts, but
         * we want to erase inclusive of the end */
        std::advance (endCommentIterator, endCommentPosition + 2);

        std::string::iterator erasureIterator =
            str.erase (beginCommentIterator, endCommentIterator);

        beginCommentPosition = std::distance (str.begin (),
                                              erasureIterator);

        beginCommentPosition = str.find ("/*", beginCommentPosition);
    }
}

void
ygt::RemoveLineIfEndingWithDEL (std::string &str)
{
    size_t position = str.find ("// DEL");

    while (position != std::string::npos)
    {
        size_t nextEndLine = str.find ("\n", position);
        size_t previousEndLine = str.rfind ("\n", position);

        if (previousEndLine == std::string::npos)
            previousEndLine = 0;

        if (nextEndLine == std::string::npos)
            throw std::runtime_error ("No newline at end of file");

        std::string::iterator nextEndLinePosition (str.begin ());
        std::advance (nextEndLinePosition, nextEndLine);

        std::string::iterator previousEndLinePosition (str.begin ());
        std::advance (previousEndLinePosition, previousEndLine);

        std::string::iterator nextPosition =
            str.erase (previousEndLinePosition, nextEndLinePosition);

        position = std::distance (str.begin (), nextPosition);
        position = str.find ("// DEL", position);
    }
}

void
ygt::CompressNewlines (std::string &str)
{
    size_t position = str.find ("\n");

    while (position != std::string::npos)
    {
        char const *cstr (str.c_str ());
        size_t compressEndPoint = position;

        while ((cstr + compressEndPoint)[0] == '\n')
        {
            ++compressEndPoint;

            if (compressEndPoint >= str.size ())
                compressEndPoint = str.size ();
        }

        size_t eraseDistance = compressEndPoint - position;

        /* Compress only if we are dealing with more than 2 \n's */
        if (eraseDistance > 2)
        {
            str.erase (position, compressEndPoint - position);
            str.insert (position, "\n\n");
        }

        position = str.find ("\n", position + 1);
    }
}

void
ygt::GenerateInstrumentationToolsN (std::string &string,
                                    size_t      toolsSize)
{
    std::string const searchToken ("INSTRUMENTATION_TOOLS_N");
    size_t tokenBegin = string.find (searchToken);

    while (tokenBegin != std::string::npos)
    {
        string.erase (tokenBegin, searchToken.size ());
        string.insert (tokenBegin, std::to_string (toolsSize));

        tokenBegin = string.find (searchToken);
    }
}

unsigned int
ygu::IndentationLevelOfToken (std::string &str, size_t pos)
{
    size_t const previousNewlinePosition = str.rfind ("\n", pos);
    unsigned int const indentation = pos - (previousNewlinePosition + 1);
    return indentation;
}

void
ygu::InsertStringsAtIndentationLevel (StringVector const &collection,
                                      unsigned int       indentation,
                                      size_t             insertPosition,
                                      size_t             replacementEndPos,
                                      std::string        &string)
{
    std::string whitespace (indentation, ' ');
    unsigned int counter = 0;
    unsigned int const collectionSize = collection.size ();

    size_t eraseDistance = replacementEndPos - insertPosition;
    string.erase (insertPosition, eraseDistance);

    for (auto const &str : collection)
    {
        std::stringstream ss;

        /* Only indent the first levels */
        if (counter++)
            ss << whitespace;

        ss << str;

        /* Don't insert newlines at the end of the collection */
        if (counter != collectionSize)
            ss << "\n";

        std::string insertString (ss.str ());
        string.insert (insertPosition, insertString);
        insertPosition += insertString.size ();
    }
}

typedef std::vector<std::string> StringVector;
typedef std::function<std::string (std::string const &,
                                   size_t,
                                   size_t)> Transformation;

void
ygu::GenerateFromTools (std::string          &string,
                        std::string    const &searchToken,
                        Transformation const &transform,
                        StringVector   const &tools)
{
    size_t searchTokenBeginPosition = string.find (searchToken);
    size_t searchTokenEndPosition = searchTokenBeginPosition +
                                    searchToken.size ();

    if (searchTokenBeginPosition == std::string::npos)
    {
        std::stringstream ss;

        ss << "Required replacement token "
           << searchToken
           << " not found in file";
        throw std::runtime_error (ss.str ());
    }

    unsigned int indentation =
        IndentationLevelOfToken (string,
                                 searchTokenBeginPosition);

    std::vector <std::string> replacements;

    replacements.reserve (tools.size ());

    size_t count = 0;
    for (std::string const &str : tools)
        replacements.push_back (transform (str, ++count, tools.size ()));

    InsertStringsAtIndentationLevel (replacements,
                                     indentation,
                                     searchTokenBeginPosition,
                                     searchTokenEndPosition,
                                     string);
}

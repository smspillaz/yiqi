/*
 * systempaths.cpp:
 * Some helper functions to manipulate information from
 * the operating system, such as the PATH variable
 *
 * See LICENCE.md for Copyright information
 */

#include <boost/algorithm/string.hpp>

#include "systempaths.h"

namespace ysys = yiqi::system;

std::vector <std::string>
ysys::SplitPathString (char const *systemPath)
{
    if (systemPath)
    {
        std::vector <std::string> availablePaths;

        boost::split (availablePaths, systemPath, boost::is_any_of (":"));
        return availablePaths;
    }

    return std::vector <std::string> ();
}

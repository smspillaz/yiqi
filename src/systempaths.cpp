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
    return std::vector <std::string> ();
}

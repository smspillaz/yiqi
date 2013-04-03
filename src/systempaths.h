/*
 * systempaths.h:
 * Some helper functions to manipulate information from
 * the operating system, such as the PATH variable
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_SYSTEM_PATHS_H
#define YIQI_SYSTEM_PATHS_H

#include <string>
#include <vector>

namespace yiqi
{
    namespace system
    {
        /**
         * @brief SplitPathString takes a ":" delimited string
         * and splits it up into a vector of components
         * @param systemPath the path obtained from getenv ()
         * @return an std::vector <std::string> of components
         */
        std::vector <std::string>
        SplitPathString (char const *systemPath);
    }
}

#endif // YIQI_SYSTEM_PATHS_H

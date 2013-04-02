/*
 * construction.cpp:
 * Implementation of the options -> construction parameters logic
 *
 * See LICENCE.md for Copyright information
 */

#include "construction.h"

namespace yc = yiqi::construction;

yc::Parameters::Unique
yc::ParseOptionsToParameters (int               argc,
			      char              **argv,
			      const yc::Options &description)
{
    return yc::Parameters::Unique ();
}

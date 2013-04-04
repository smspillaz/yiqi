/*
 * exceptioncleanup.cpp
 * Provide a RAII class used to roll-back operations if an exception
 * occurred in between them
 *
 * See LICENCE.md for Copyright information
 */

#include <functional>

#include "exceptioncleanup.h"

namespace yu = yiqi::util;

yu::ExceptionCleanup::ExceptionCleanup (Func const &func,
                                        bool       &commit) :
    mFunc (func),
    mCommit (commit)
{
}

yu::ExceptionCleanup::~ExceptionCleanup ()
{
    if (!mCommit)
        mFunc ();
}

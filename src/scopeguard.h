/*
 * scopeguard.h
 * A simple class ensure that a particular piece of code is always
 * executed on a destructor, or not, if a flag is set to dismiss it
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_SCOPEGUARD_H
#define YIQI_SCOPEGUARD_H

#include <functional>

namespace yiqi
{
    class ScopeGuard
    {
        public:

            typedef std::function <void ()> Function;

            ScopeGuard (const Function &func) :
                function (func),
                dismissed (false)
            {
            }

            ~ScopeGuard ()
            {
                if (!dismissed)
                    function();
            }

            void dismiss ()
            {
                dismissed = true;
            }

        private:

          Function function;
          bool dismissed;
    };
}

#endif

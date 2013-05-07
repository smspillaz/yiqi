/*
 * exceptioncleanup.h
 * Provide a RAII class used to roll-back operations if an exception
 * occurred in between them
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_EXCEPTION_CLEANUP_H
#define YIQI_EXCEPTION_CLEANUP_H

namespace yiqi
{
    namespace util
    {
        /**
         * @brief The ExceptionCleanup class provides a method to guaruntee
         * cleanup of a function if an exception is thrown the commit () member
         * is called
         */
        class ExceptionCleanup
        {
            public:

                typedef std::function <void ()> Func;
                ExceptionCleanup (Func const &func);
                ~ExceptionCleanup ();

                void commit ();

            private:

                ExceptionCleanup (ExceptionCleanup const &) = delete;
                ExceptionCleanup & operator= (ExceptionCleanup const &) = delete;

                Func mFunc;
                bool mCommit;
        };
    }
}

#endif // YIQI_EXCEPTION_CLEANUP_H

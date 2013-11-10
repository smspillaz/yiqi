/*
 * operating_system_wrapper.h:
 * A simple interface to provide exception-safe entry-points into
 * common UNIX functionality.
 *
 * This is an implementation detail, clients who need to use operating system
 * functionality should use the ysysapi::SystemCalls interface
 *
 * See LICENCE.md for Copyright information
 */

 #ifndef YIQI_OPERATING_SYSTEM_WRAPPER_H
 #define YIQI_OPERATING_SYSTEM_WRAPPER_H

namespace yiqi
{
    namespace system
    {
        namespace unix
        {
            class OperatingSystemWrapper
            {
                public:

                    virtual ~OperatingSystemWrapper () {};

                    virtual int access (char const *path,
                                        int        flags) noexcept = 0;
                    virtual int execve (char const         *path,
                                        char const * const *argv,
                                        char const * const *env) noexcept = 0;
                    virtual char const * getenv (char const *env) noexcept = 0;
                    virtual char const * const * environment () noexcept = 0;
                    virtual int pipe (int p[2]) noexcept = 0;

                protected:

                    OperatingSystemWrapper () = default;

                private:

                    OperatingSystemWrapper (const OperatingSystemWrapper &) = delete;
                    OperatingSystemWrapper &
                    operator= (const OperatingSystemWrapper &) = delete;
            };
        }
    }
}

#endif

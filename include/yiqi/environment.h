/*
 * environment.h:
 *
 * Factory functions for a ::testing::Environment *
 * that can be passed to ::testing::AddGlobalTestEnvironment
 *
 * See LICENCE.md for Copyright information
 */
#ifndef YIQI_ENVIRONMENT_H
#define YIQI_ENVIRONMENT_H

namespace testing
{
    class Environment;
}

namespace yiqi
{
    ::testing::Environment * CreateTestEnvironment (int argc,
                                                    char **argv);
}

#endif

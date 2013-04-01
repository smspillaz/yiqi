/*
 * Copyright (c) 2013 Sam Spilsbury <smspillaz@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Sam Spilsbury <smspillaz@gmail.com>
 */

#include <gtest/gtest.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace
{
    class YiqiEnvironment :
        public ::testing::Environment
    {
        public:

            virtual ~YiqiEnvironment () = default;
            virtual void SetUp ();
            virtual void TearDown ();
    };
}

void
YiqiEnvironment::SetUp ()
{
}

void
YiqiEnvironment::TearDown()
{
}

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    ::testing::AddGlobalTestEnvironment(new YiqiEnvironment);

    return RUN_ALL_TESTS ();
}

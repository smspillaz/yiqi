/*
 * passthrough_constants.cpp
 *
 * Definitions for messages passed by the passthrough binary to
 * the stdout
 *
 * See LICENCE.md for Copyright information
 */

#include "passthrough_constants.h"

namespace ytp = yiqi::testing::passthrough;

std::string const ytp::StartupMessage ("PASSTHROUGH BINARY LAUNCHED");
std::string const ytp::OptionHeader ("OPTION: ");

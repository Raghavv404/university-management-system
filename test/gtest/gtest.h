// Stub header that delegates to our minimal_gtest shim when no real GTest is
// available. With CMake + a real GoogleTest, this directory is not on the
// include path - the real <gtest/gtest.h> is found instead.
#pragma once
#include "../minimal_gtest.h"

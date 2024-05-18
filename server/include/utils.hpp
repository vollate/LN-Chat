#pragma once

#include <iostream>

#ifdef LN_DEBUG
// Print only in debug mode
#define dPrint(val) {std::cout << val << std::endl;}
#else
// Print nothing in release mode
#define dPrint(val)
#endif
#pragma once

#include <iostream>

#ifdef LN_DEBUG
#define dPrint(val) {std::cout << val << std::endl;}
#else
#define dPrint(val)
#endif
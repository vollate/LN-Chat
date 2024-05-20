#pragma once

#ifdef LN_DEBUG
// Print only in debug mode
#define dPrint(val)                                                                                                              \
    { std::cout << val << '\n'; }
#else
// Print nothing in release mode
#define dPrint(val)
#endif

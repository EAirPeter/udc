#ifndef UDC_PRINT_HPP_
#define UDC_PRINT_HPP_

#include <iostream>

#include "Location.hpp"

namespace udc {

namespace _impl_FmtStr {
extern char g_aBuf[4096];
}

template<class ...tArgs>
inline const char *FmtStr(const char *pszFormat, tArgs &&...vArgs) noexcept {
    using _impl_FmtStr::g_aBuf;
    std::snprintf(g_aBuf, sizeof(g_aBuf), pszFormat, std::forward<tArgs>(vArgs)...);
    return g_aBuf;
}

template<class ...tArgs>
inline void PrintError(const location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ":   error: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

template<class ...tArgs>
inline void PrintWarning(const location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ": warning: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

template<class ...tArgs>
inline void PrintNote(const location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ":    note: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

}

#endif

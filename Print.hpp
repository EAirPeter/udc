#ifndef UDC_PRINT_HPP_
#define UDC_PRINT_HPP_

#include <iostream>

#include "Location.hpp"

namespace udc {

template<class ...tArgs>
inline void PrintError(const Location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ":   error: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

template<class ...tArgs>
inline void PrintWarning(const Location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ": warning: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

template<class ...tArgs>
inline void PrintNote(const Location &vLoc, tArgs &&...vArgs) {
    ((std::cerr << "At " << vLoc << ":    note: ") << ... << std::forward<tArgs>(vArgs)) << std::endl;
}

}

#endif

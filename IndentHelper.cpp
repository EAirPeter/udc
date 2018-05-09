#include "IndentHelper.hpp"

#include <ostream>

namespace udc {

std::ostream &operator<<(std::ostream &os, const IndentHelper &vIndent) {
    for (auto i = vIndent.cIndent; i; --i)
        os << "  ";
    return os;
}

}

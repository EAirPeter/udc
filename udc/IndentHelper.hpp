#ifndef UDC_INDENT_HELPER_HPP_
#define UDC_INDENT_HELPER_HPP_

#include <cstdint>
#include <iosfwd>

namespace udc {

struct IndentHelper {
    std::uint32_t cIndent;
};

std::ostream &operator <<(std::ostream &os, const IndentHelper &vIndent);

constexpr IndentHelper Indent(std::uint32_t cIndent) noexcept {
    return {cIndent};
}

}

#endif

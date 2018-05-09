#include <iomanip>

#include "../IndentHelper.hpp"
#include "StrLit.hpp"

namespace udc::ast {

StrLit::StrLit(const Location &vLocation, std::string &&sVal) noexcept :
    Base(vLocation), x_sVal(std::move(sVal))
{}

StrLit::~StrLit() {}

void StrLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << std::quoted(x_sVal);
}

}

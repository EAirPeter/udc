#include "../IndentHelper.hpp"
#include "IntType.hpp"

namespace udc::ast {

IntType::IntType(const Location &vLocation) noexcept : Base(vLocation) {}

IntType::~IntType() {}

void IntType::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "int";
}

std::string IntType::GetName() const noexcept {
    return "int";
}

}

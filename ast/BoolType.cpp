#include "../IndentHelper.hpp"
#include "BoolType.hpp"

namespace udc::ast {

BoolType::BoolType(const Location &vLocation) noexcept : Base(vLocation) {}

BoolType::~BoolType() {}

void BoolType::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "bool";
}

std::string BoolType::GetName() const noexcept {
    return "bool";
}

}

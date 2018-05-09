#include "../IndentHelper.hpp"
#include "VoidType.hpp"

namespace udc::ast {

VoidType::VoidType(const Location &vLocation) noexcept : Base(vLocation) {}

VoidType::~VoidType() {}

void VoidType::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "void";
}

std::string VoidType::GetName() const noexcept {
    return "void";
}

}

#include "../IndentHelper.hpp"
#include "StrType.hpp"

namespace udc::ast {

StrType::StrType(const Location &vLocation) noexcept : Base(vLocation) {}

StrType::~StrType() {}

void StrType::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "class";
}

std::string StrType::GetName() const noexcept {
    return "string";
}

}

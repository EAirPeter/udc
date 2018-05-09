#include "../IndentHelper.hpp"
#include "ClassType.hpp"

namespace udc::ast {

ClassType::ClassType(const Location &vLocation, std::string &&sName) noexcept :
    Base(vLocation), x_sName(std::move(sName))
{}

ClassType::~ClassType() {}

void ClassType::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "class " << x_sName;
}

std::string ClassType::GetName() const noexcept {
    return x_sName;
}


}

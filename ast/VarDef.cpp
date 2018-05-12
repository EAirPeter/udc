#include "TypeName.hpp"
#include "VarDef.hpp"

namespace udc::ast {

VarDef::VarDef(const Location &vLocation, std::unique_ptr<TypeName> &&upType, std::string &&sName) noexcept :
    NodeBase(vLocation), x_upType(std::move(upType)), x_sName(std::move(sName))
{}

VarDef::~VarDef() {}

void VarDef::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upType->Print(os, cIndent);
    os << ' ' << x_sName;
}

}

#include "TypeName.hpp"
#include "VarDef.hpp"

namespace udc::ast {

VarDef::VarDef(const location &loc, std::unique_ptr<TypeName> &&upType, std::string &&sName) noexcept :
    NodeBase(loc), x_upType(std::move(upType)), x_sName(std::move(sName))
{}

VarDef::~VarDef() {}

void VarDef::Print(std::ostream &os, std::uint32_t cIndent) const {
    x_upType->Print(os, cIndent);
    os << ' ' << x_sName;
}

}

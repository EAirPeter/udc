#include "VarDef.hpp"

namespace udc::ast {

VarDef::VarDef(const Location &vLocation, std::unique_ptr<Type> &&upType, std::string &&sName) noexcept :
    FieldDef(vLocation), x_upType(std::move(upType)), x_sName(std::move(sName))
{}

VarDef::~VarDef() {}

void VarDef::Y_Print(std::ostream &os, uint32_t cIndent) const {
    x_upType->Print(os, 0);
    os << ' ' << x_sName;
}

}

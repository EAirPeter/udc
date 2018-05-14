#include "../IndentHelper.hpp"
#include "ClassDef.hpp"

namespace udc::ast {

ClassDef::ClassDef(
    const Location &vLocation,
    std::string &&sName,
    std::optional<std::string> &&soBase,
    RefVec<NodeBase> &&vecFields
) noexcept :
    NodeBase(vLocation),
    x_sName(std::move(sName)),
    x_soBase(std::move(soBase)),
    x_vecFields(std::move(vecFields)),
    x_stFn(true),
    x_stVar(true)
{}

ClassDef::~ClassDef() {}

void ClassDef::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "class " << x_sName;
    if (x_soBase)
        os << " extends " << *x_soBase;
    for (NodeBase &vField : x_vecFields) {
        os << std::endl;
        vField.Print(os, cIndent + 1);
    }
}

bool ClassDef::Accepts(const INonArrayType &ty) const noexcept {
    auto pCls = dynamic_cast<const ClassDef *>(&ty);
    if (!pCls)
        return false;
    while (pCls && pCls != this)
        pCls = pCls->GetBase();
    return pCls == this;
}

void ClassDef::Print(std::ostream &os) const {
    os << "class " << x_sName;
}

}

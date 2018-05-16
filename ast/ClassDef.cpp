#include "../IndentHelper.hpp"
#include "ClassDef.hpp"

namespace udc::ast {

ClassDef::ClassDef(
    const Location &vLocation,
    std::string &&sName,
    std::optional<std::string> &&soBase,
    std::vector<std::unique_ptr<NodeBase>> &&vecFields
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
    for (auto &upField : x_vecFields) {
        os << std::endl;
        upField->Print(os, cIndent + 1);
    }
}

bool ClassDef::Accepts(const INonArrayType &ty) const noexcept {
    if (ty == eval::NullType::vInstance)
        return true;
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

bool ClassDef::SetBase(ClassDef &tyBase) noexcept {
    auto pThisRoot = X_GetRoot();
    auto pBaseRoot = tyBase.X_GetRoot();
    if (pThisRoot == pBaseRoot)
        return false;
    x_pRoot = pBaseRoot;
    x_pBase = &tyBase;
    x_stFn.SetParent(tyBase.x_stFn);
    x_stVar.SetParent(tyBase.x_stVar);
    tyBase.x_vecDeriveds.emplace_back(this);
    return true;
}

ClassDef *ClassDef::X_GetRoot() noexcept {
    return !x_pRoot ? this : x_pRoot = x_pRoot->X_GetRoot();
}

}

#include "../../ScopeHelper.hpp"
#include "../All.hpp"
#include "InheritVisitor.hpp"

namespace udc::ast::eval {

void InheritVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pstClass, &vProg.GetClassTable());
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void InheritVisitor::Visit(ClassDef &vClass) noexcept {
    auto &soName = vClass.GetBaseName();
    if (!soName)
        return;
    auto pBase = x_pstClass->Lookup(*soName);
    if (!pBase) {
        Y_RjNotFound(vClass.GetLocation(), "class", *soName);
        return;
    }
    if (!vClass.SetBase(*pBase)) {
        Y_RjInheritCycle(vClass.GetLocation());
        return;
    }
}

}

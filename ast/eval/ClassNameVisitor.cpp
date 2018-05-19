#include "../../Driver.hpp"
#include "../../ScopeHelper.hpp"
#include "../All.hpp"
#include "ClassNameVisitor.hpp"

namespace udc::ast::eval {

void ClassNameVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    ENTER_SCOPE(x_pstClass, &vProg.GetClassTable());
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void ClassNameVisitor::Visit(ClassDef &vClass) noexcept {
    auto &sName = vClass.GetName();
    auto pPrevious = x_pstClass->Add(sName, &vClass);
    if (pPrevious) {
        Y_RjRedefinition(vClass.GetLocation(), "class", sName, pPrevious->GetLocation());
        return;
    }
    vClass.SetType(x_pTyReg->Put(vClass));
}

}

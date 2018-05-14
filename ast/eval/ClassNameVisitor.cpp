#include "../../Driver.hpp"
#include "../All.hpp"
#include "ClassNameVisitor.hpp"

namespace udc::ast::eval {

void ClassNameVisitor::Visit(Program &vProg) noexcept {
    x_pstClass = &vProg.GetClassTable();
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void ClassNameVisitor::Visit(ClassDef &vClass) noexcept {
    auto &sName = vClass.GetName();
    auto pPrevious = x_pstClass->Add(sName, &vClass);
    if (pPrevious) {
        Y_Reject();
        y_vDriver.PrintError(
            vClass.GetLocation(),
            "redefinition of class ", sName
        );
        y_vDriver.PrintNote(
            pPrevious->GetLocation(),
            "previous definition is here"
        );
        return;
    }
}

}

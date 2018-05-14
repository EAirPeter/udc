#include "../../Driver.hpp"
#include "../All.hpp"
#include "InheritVisitor.hpp"

namespace udc::ast::eval {

void InheritVisitor::Visit(Program &vProg) noexcept {
    x_pstClass = &vProg.GetClassTable();
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void InheritVisitor::Visit(ClassDef &vClass) noexcept {
    auto &soName = vClass.GetBaseName();
    if (!soName)
        return;
    auto pBase = x_pstClass->Lookup(*soName);
    if (!pBase) {
        Y_Reject();
        y_vDriver.PrintError(vClass.GetLocation(), "unknown base class ", *soName);
        return;
    }
    vClass.SetBase(*pBase);
}

}

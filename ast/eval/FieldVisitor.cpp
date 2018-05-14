#include "../../Driver.hpp"
#include "../All.hpp"
#include "FieldVisitor.hpp"

namespace udc::ast::eval {

void FieldVisitor::Visit(Program &vProg) noexcept {
    x_pstClass = &vProg.GetClassTable();
    for (auto &upClass : vProg.GetClasses())
        if (upClass->GetDeriveds().empty())
            upClass->AcceptVisitor(*this);
}

void FieldVisitor::Visit(ClassDef &vClass) noexcept {
    x_pstFn = &vClass.GetFnTable();
    x_pstVar = &vClass.GetVarTable();
    for (auto &upField : vClass.GetFields())
        upField->AcceptVisitor(*this);
    for (auto &pDerived : vClass.GetDeriveds())
        pDerived->AcceptVisitor(*this);
}

void FieldVisitor::Visit(FnDef &vFn) noexcept {
    auto &sName = vFn.GetName();
    auto pPrevious = vFn.IsStatic() ?
        x_pstFn->AddNoOverride(sName, &vFn) :
        x_pstFn->Add(sName, &vFn);
    if (pPrevious) {
        Y_Reject();
        y_vDriver.PrintError(
            vFn.GetLocation(),
            "redefinition of function ", sName
        );
        y_vDriver.PrintNote(
            pPrevious->GetLocation(),
            "previous definition is here"
        );
        return;
    }
    vFn.GetTypeName()->AcceptVisitor(*this);
    vFn.SetType(x_ty);
}

void FieldVisitor::Visit(VarDef &vVar) noexcept {
    auto &sName = vVar.GetName();
    auto pPrevious = x_pstVar->AddNoOverride(sName, &vVar);
    if (pPrevious) {
        Y_Reject();
        y_vDriver.PrintError(
            vVar.GetLocation(),
            "redefinition of variable ", sName
        );
        y_vDriver.PrintNote(
            pPrevious->GetLocation(),
            "previous definition is here"
        );
        return;
    }
    vVar.GetTypeName()->AcceptVisitor(*this);
    vVar.SetType(x_ty);
}

void FieldVisitor::Visit(TypeName &vTypeName) noexcept {
    switch (vTypeName.GetId()) {
    case TypeId::kVoid:
        x_ty = {VoidType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kInt:
        x_ty = {IntType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kBool:
        x_ty = {BoolType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kString:
        x_ty = {StringType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kClass: {
        auto pClass = x_pstClass->Lookup(vTypeName.GetName());
        if (!pClass) {
            Y_Reject();
            y_vDriver.PrintError(
                vTypeName.GetLocation(),
                "unknown class ", vTypeName.GetName()
            );
            x_ty = {VoidType::vInstance, 0};
            break;
        }
        x_ty = {*pClass, vTypeName.GetDimension()};
        break;
    }
    }
}

}

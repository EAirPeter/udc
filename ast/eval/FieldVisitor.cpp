#include "../../Driver.hpp"
#include "../../ScopeHelper.hpp"
#include "../All.hpp"
#include "FieldVisitor.hpp"

namespace udc::ast::eval {

void FieldVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pstClass, &vProg.GetClassTable());
    for (auto &upClass : vProg.GetClasses())
        if (!upClass->GetBaseName())
            upClass->AcceptVisitor(*this);
}

void FieldVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pstFn, &vClass.GetFnTable());
    ENTER_SCOPE(x_pstVar, &vClass.GetVarTable());
    for (auto &upField : vClass.GetFields())
        upField->AcceptVisitor(*this);
    for (auto &pDerived : vClass.GetDeriveds())
        pDerived->AcceptVisitor(*this);
}

void FieldVisitor::Visit(FnDef &vFn) noexcept {
    vFn.GetTypeName()->AcceptVisitor(*this);
    vFn.SetType(x_ty);
    ENTER_SCOPE(x_pstVar, &vFn.GetVarTable());
    ENTER_SCOPE(x_bPar, true);
    for (auto &upPar : vFn.GetPars())
        upPar->AcceptVisitor(*this);
    auto &sName = vFn.GetName();
    auto pPrevious = x_pstFn->LookupInParent(sName);
    if (pPrevious) {
        if (vFn.IsStatic()) {
            Y_RjRedefinition(vFn.GetLocation(), "function", sName, pPrevious->GetLocation());
            return;
        }
        if (pPrevious->IsStatic()) {
            Y_RjOverride(vFn.GetLocation(), sName, pPrevious->GetLocation());
            return;
        }
        if (pPrevious->GetType() != vFn.GetType()) {
            Y_RjOverride(vFn.GetLocation(), sName, pPrevious->GetLocation());
            return;
        }
        if (pPrevious->GetPars().size() != vFn.GetPars().size()) {
            Y_RjOverride(vFn.GetLocation(), sName, pPrevious->GetLocation());
            return;
        }
        std::size_t i = 0;
        for (auto &upPar : vFn.GetPars())
            if (upPar->GetType() != pPrevious->GetPars()[i]->GetType()) {
                Y_RjOverride(vFn.GetLocation(), sName, pPrevious->GetLocation());
                return;
            }
        // TODO: vtable
    }
    pPrevious = x_pstFn->Add(sName, &vFn);
    if (pPrevious) {
        Y_RjRedefinition(vFn.GetLocation(), "function", sName, pPrevious->GetLocation());
        return;
    }
}

void FieldVisitor::Visit(VarDef &vVar) noexcept {
    vVar.GetTypeName()->AcceptVisitor(*this);
    vVar.SetType(x_ty);
    auto &sName = vVar.GetName();
    auto pPrevious = x_bPar ? x_pstVar->Add(sName, &vVar) : x_pstVar->AddNoOverride(sName, &vVar);
    if (pPrevious) {
        Y_RjRedefinition(vVar.GetLocation(), "variable", sName, pPrevious->GetLocation());
        return;
    }
}

void FieldVisitor::Visit(TypeName &vTypeName) noexcept {
    switch (vTypeName.GetId()) {
    case TypeId::kVoid:
        if (vTypeName.GetDimension()) {
            Y_RjIllegalType(vTypeName.GetLocation(), {VoidType::vInstance, vTypeName.GetDimension()});
            x_ty = Type::tyVoid;
            return;
        }
        x_ty = Type::tyVoid;
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
            Y_RjNotFound(vTypeName.GetLocation(), "class", vTypeName.GetName());
            x_ty = Type::tyVoid;
            break;
        }
        x_ty = {*pClass, vTypeName.GetDimension()};
        break;
    }
    }
}

}

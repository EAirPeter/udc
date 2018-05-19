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
    ENTER_SCOPE(x_pVars, &vClass.GetVars());
    ENTER_SCOPE(x_pstVar, &vClass.GetVarTable());
    if (x_pstVf)
        vClass.GetVfTable().SetParent(*x_pstVf);
    ENTER_SCOPE(x_pstVf, &vClass.GetVfTable());
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
    auto pPrevStatic = x_pstFn->Lookup(sName);
    if (pPrevStatic) {
        if (vFn.IsStatic())
            Y_RjRedefinition(vFn.GetLocation(), "static function", sName, pPrevStatic->GetLocation());
        else
            Y_RjOverride(vFn.GetLocation(), sName, pPrevStatic->GetLocation());
        return;
    }
    auto pPrevVirt = x_pstVf->LookupInParent(sName);
    if (vFn.IsStatic()) {
        if (pPrevVirt) {
            Y_RjOverride(vFn.GetLocation(), sName, pPrevStatic->GetLocation());
            return;
        }
        pPrevStatic = x_pstFn->AddNoOverride(sName, &vFn);
        assert(!pPrevStatic);
    }
    else {
        if (pPrevVirt) {
            if (pPrevVirt->GetType() != vFn.GetType()) {
                Y_RjOverride(vFn.GetLocation(), sName, pPrevVirt->GetLocation());
                return;
            }
            if (pPrevVirt->GetPars().size() != vFn.GetPars().size()) {
                Y_RjOverride(vFn.GetLocation(), sName, pPrevVirt->GetLocation());
                return;
            }
            std::size_t i = 0;
            for (auto &upPar : vFn.GetPars())
                if (upPar->GetType() != pPrevVirt->GetPars()[i]->GetType()) {
                    Y_RjOverride(vFn.GetLocation(), sName, pPrevVirt->GetLocation());
                    return;
                }
        }
        pPrevVirt = x_pstVf->Add(sName, &vFn);
        if (pPrevVirt) {
            Y_RjRedefinition(vFn.GetLocation(), "function", sName, pPrevVirt->GetLocation());
            return;
        }
        vFn.SetIdx(x_pstVf->IndexOf(sName));
    }
}

void FieldVisitor::Visit(VarDef &vVar) noexcept {
    vVar.GetTypeName()->AcceptVisitor(*this);
    vVar.SetType(x_ty);
    VarDef *pPrevious = nullptr;
    auto &sName = vVar.GetName();
    if (x_bPar) {
        auto pPrevious = x_pstVar->Add(sName, &vVar);
        if (pPrevious) {
            Y_RjRedefinition(vVar.GetLocation(), "variable", sName, pPrevious->GetLocation());
            return;
        }
    }
    else {
        auto pPrevious = x_pstVar->AddNoOverride(sName, &vVar);
        if (pPrevious) {
            Y_RjRedefinition(vVar.GetLocation(), "variable", sName, pPrevious->GetLocation());
            return;
        }
        vVar.SetIdx(x_pVars->size());
        x_pVars->emplace_back(&vVar);
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

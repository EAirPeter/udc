#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../Driver.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "FnGenVisitor.hpp"

namespace udc::cg {

FnGenVisitor::FnGenVisitor(Driver &drv) noexcept : VisitorBase(drv) {}

void FnGenVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    ENTER_SCOPE(x_plvClassIdx, vProg.GetLvClassIdx());
    for (auto &upClass : vProg.GetClasses())
        if (!upClass->GetBase())
            upClass->AcceptVisitor(*this);
}

void FnGenVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        upField->AcceptVisitor(*this);
    // emit vtable, #VTable.Name
    // index, vfn0, vfn1, ...
    {
        auto ty = llvm::ArrayType::get(y_drv.tyVoidPtr, (std::uint64_t) vClass.GetVfTable().GetVec().size() + 1);
        auto gv = llvm::cast<llvm::GlobalVariable>(
            x_plvMod->getOrInsertGlobal(std::string {"#VTable."} + vClass.GetName(), ty)
        );
        gv->setAlignment(y_drv.lvDataLayout.getPrefTypeAlignment(ty));
        gv->setConstant(true);
        gv->setDSOLocal(true);
        gv->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
        llvm::SmallVector<llvm::Constant *, 64> vec;
        {
            // emit class index
            auto i8p = llvm::ConstantExpr::getBitCast(x_plvClassIdx, y_drv.tyI8Ptr);
            auto ciIdx = llvm::ConstantInt::get(y_drv.tySize, vClass.GetIdx() * y_drv.uPtrSize);
            vec.emplace_back(llvm::ConstantExpr::getGetElementPtr(y_drv.tyI8, i8p, {ciIdx}));
        }
        for (auto &pFn : vClass.GetVfTable().GetVec())
            vec.emplace_back(llvm::ConstantExpr::getBitCast(pFn->GetLvFn(), y_drv.tyVoidPtr));
        gv->setInitializer(llvm::ConstantArray::get(ty, vec));
        vClass.SetLvVTable(gv);
    }
    for (auto &pDerived : vClass.GetDeriveds())
        pDerived->AcceptVisitor(*this);
}

void FnGenVisitor::Visit(FnDef &vFn) noexcept {
    // fwd decl fn, #CName.FName
    {
        llvm::SmallVector<llvm::Type *, 64> vec;
        if (!vFn.IsStatic())
            vec.emplace_back(vFn.GetClass().GetType().GetLvType()->getPointerTo());
        for (auto &upPar : vFn.GetPars())
            vec.emplace_back(upPar->GetType().GetLvType());
        auto ty = llvm::FunctionType::get(vFn.GetType().GetLvType(), vec, false);
        auto fn = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction(
            std::string {'#'} + x_pClass->GetName() + '.' + vFn.GetName(),
            ty
        ));
        fn->setCallingConv(llvm::CallingConv::Fast);
        fn->setDSOLocal(true);
        fn->setLinkage(llvm::GlobalValue::PrivateLinkage);
        vFn.SetLvFn(fn);
    }
}

}

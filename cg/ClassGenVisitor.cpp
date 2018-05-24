#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../Driver.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "../ast/eval/Type.hpp"
#include "ClassGenVisitor.hpp"

namespace udc::cg {

ClassGenVisitor::ClassGenVisitor(Driver &drv) noexcept : VisitorBase(drv) {}

void ClassGenVisitor::Visit(Program &vProg) noexcept {
    vProg.GetUplvMod() = std::make_unique<llvm::Module>("poi?", y_drv.lvCtx);
    auto plvMod = vProg.GetLvMod();
    plvMod->setDataLayout(y_drv.lvDataLayout);
    plvMod->setTargetTriple(y_drv.sTriple);
    auto &vTyReg = vProg.GetTyReg();
    vTyReg.tyNull.SetLvType(y_drv.tyI8Ptr);
    vTyReg.tyVoid.SetLvType(y_drv.tyVoid);
    vTyReg.tyInt.SetLvType(y_drv.tyI32);
    vTyReg.tyBool.SetLvType(y_drv.tyI1);
    vTyReg.tyString.SetLvType(y_drv.tyI8Ptr);
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    // emit RTTI, #ClassIdx
    {
        auto ty = llvm::ArrayType::get(y_drv.tyVoidPtr, (std::uint64_t) vProg.GetClasses().size());
        auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal("#ClassIdx", ty));
        gv->setAlignment(y_drv.lvDataLayout.getPrefTypeAlignment(ty));
        gv->setConstant(true);
        gv->setDSOLocal(true);
        gv->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto i8p = llvm::ConstantExpr::getPointerCast(gv, y_drv.tyI8Ptr);
        llvm::SmallVector<llvm::Constant *, 64> vec;
        for (auto &upClass : vProg.GetClasses()) {
            auto pBase = upClass->GetBase();
            auto idxBase = pBase ? pBase->GetIdx() : ~std::size_t {};
            if (~idxBase) {
                auto ciIdx = llvm::ConstantInt::get(y_drv.tySize, idxBase * y_drv.uPtrSize);
                vec.emplace_back(llvm::ConstantExpr::getGetElementPtr(y_drv.tyI8, i8p, ciIdx));
            }
            else {
                vec.emplace_back(llvm::ConstantPointerNull::get(y_drv.tyVoidPtr));
            }
            upClass->SetLvType(llvm::StructType::create(y_drv.lvCtx, std::string {"#Class."} + upClass->GetName()));
            upClass->GetType().SetLvType(upClass->GetLvType()->getPointerTo());
        }
        gv->setInitializer(llvm::ConstantArray::get(ty, vec));
        vProg.SetLvClassIdx(gv);
    }
    x_pTyReg->MakeArrays();
    for (auto &upClass : vProg.GetClasses())
        if (!upClass->GetBase())
            upClass->AcceptVisitor(*this);
}

void ClassGenVisitor::Visit(ClassDef &vClass) noexcept {
    // emit Type definition, #Class.Name
    // #Class.Name {ptr, ...} for root classes
    // #Class.Name {base, ...} for derived classes
    {
        llvm::SmallVector<llvm::Type *, 64> vec;
        auto pBase = vClass.GetBase();
        if (pBase)
            vec.emplace_back(pBase->GetLvType());
        else
            vec.emplace_back(y_drv.tyVoidPtr);
        for (auto pVar : vClass.GetVars())
            vec.emplace_back(pVar->GetType().GetLvType());
        vClass.GetLvType()->setBody(vec);
    }
    for (auto &pDerived : vClass.GetDeriveds())
        pDerived->AcceptVisitor(*this);
}

}

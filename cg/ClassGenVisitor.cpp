#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "CGContext.hpp"
#include "ClassGenVisitor.hpp"

namespace udc::cg {

ClassGenVisitor::ClassGenVisitor(CGContext &ctx) noexcept : x_ctx(ctx) {}

void ClassGenVisitor::Visit(Program &vProg) noexcept {
    vProg.GetUplvMod() = std::make_unique<llvm::Module>(vProg.GetInputPath().stem().string(), x_ctx.lvCtx);
    auto plvMod = vProg.GetLvMod();
    plvMod->setSourceFileName(vProg.GetInputPath().string());
    plvMod->setDataLayout(x_ctx.lvDataLayout);
    plvMod->setTargetTriple(x_ctx.sTriple);
    auto &vTyReg = vProg.GetTyReg();
    vTyReg.tyNull.SetLvType(x_ctx.tyI8Ptr);
    vTyReg.tyVoid.SetLvType(x_ctx.tyVoid);
    vTyReg.tyInt.SetLvType(x_ctx.tyI32);
    vTyReg.tyBool.SetLvType(x_ctx.tyI1);
    vTyReg.tyString.SetLvType(x_ctx.tyI8Ptr);
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    // emit RTTI, #ClassIdx
    {
        auto ty = llvm::ArrayType::get(x_ctx.tyVoidPtr, (std::uint64_t) vProg.GetClasses().size());
        auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal("#ClassIdx", ty));
        gv->setAlignment(x_ctx.lvDataLayout.getPrefTypeAlignment(ty));
        gv->setConstant(true);
        gv->setDSOLocal(true);
        gv->setLinkage(llvm::GlobalValue::InternalLinkage);
        auto i8p = llvm::ConstantExpr::getPointerCast(gv, x_ctx.tyI8Ptr);
        llvm::SmallVector<llvm::Constant *, 64> vec;
        for (auto &upClass : vProg.GetClasses()) {
            auto pBase = upClass->GetBase();
            auto idxBase = pBase ? pBase->GetIdx() : ~std::size_t {};
            if (~idxBase) {
                auto ciIdx = llvm::ConstantInt::get(x_ctx.tySize, idxBase * x_ctx.uPtrSize);
                vec.emplace_back(llvm::ConstantExpr::getGetElementPtr(x_ctx.tyI8, i8p, ciIdx));
            }
            else {
                vec.emplace_back(llvm::ConstantPointerNull::get(x_ctx.tyVoidPtr));
            }
            upClass->SetLvType(llvm::StructType::create(x_ctx.lvCtx, std::string {"#Class."} + upClass->GetName()));
            upClass->GetType().SetLvType(upClass->GetLvType()->getPointerTo());
        }
        gv->setInitializer(llvm::ConstantArray::get(ty, vec));
        vProg.SetLvClassIdx(gv);
    }
    x_pTyReg->MakeArrays(x_ctx);
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
            vec.emplace_back(x_ctx.tyVoidPtr);
        for (auto pVar : vClass.GetVars())
            vec.emplace_back(pVar->GetType().GetLvType());
        vClass.GetLvType()->setBody(vec);
    }
    for (auto &pDerived : vClass.GetDeriveds())
        pDerived->AcceptVisitor(*this);
}

}

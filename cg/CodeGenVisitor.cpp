#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "../Driver.hpp"
#include "../Print.hpp"
#include "../ScopeHelper.hpp"
#include "../ast/All.hpp"
#include "CodeGenVisitor.hpp"

namespace udc::cg {

namespace {

char f_aBuf[4096];

template<class ...tArgs>
inline const char *FmtStr(const char *pszFormat, tArgs &&...vArgs) noexcept {
    std::snprintf(f_aBuf, sizeof(f_aBuf), pszFormat, std::forward<tArgs>(vArgs)...);
    return f_aBuf;
}

}

CodeGenVisitor::CodeGenVisitor(Driver &drv) noexcept : VisitorBase(drv) {}

inline llvm::Constant *CodeGenVisitor::X_EmitStringLiteral(const char *psz) noexcept {
    assert(x_plvMod);
    auto it = x_mapStrLits.find(psz);
    if (it != x_mapStrLits.end())
        return it->second;
    auto len = std::strlen(psz) + 1;
    auto ty = llvm::ArrayType::get(y_drv.tyI8, len);
    auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal(FmtStr("#StrLit.%zu", x_mapStrLits.size()), ty));
    gv->setAlignment(1);
    gv->setConstant(true);
    gv->setDSOLocal(true);
    gv->setLinkage(llvm::GlobalValue::PrivateLinkage);
    llvm::SmallVector<llvm::Constant *, 64> vec;
    for (std::size_t i = 0; i < len; ++i)
        vec.emplace_back(llvm::ConstantInt::get(y_drv.tyI8, psz[i]));
    gv->setInitializer(llvm::ConstantArray::get(ty, vec));
    auto res = llvm::ConstantExpr::getPointerCast(gv, y_drv.tyI8Ptr);
    x_mapStrLits.emplace(std::piecewise_construct, std::forward_as_tuple(psz), std::forward_as_tuple(res));
    return res;
}

void CodeGenVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    ENTER_SCOPE(x_plvMod, vProg.GetLvMod());
    ENTER_SCOPE(x_plvClassIdx, vProg.GetLvClassIdx());
    // decl CRT
    {
        // extern void *calloc(size_t num, size_t size);
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tySize, y_drv.tySize}, false);
        x_plvCrtCalloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("calloc", ty));
        x_plvCrtCalloc->setDSOLocal(true);
        x_plvCrtCalloc->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvCrtCalloc->setReturnDoesNotAlias();
        if (llvm::verifyFunction(*x_plvCrtCalloc, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // __declspec(noreturn) extern void exit(int exit_code);
        auto ty = llvm::FunctionType::get(y_drv.tyVoid, {y_drv.tyI32}, false);
        x_plvCrtExit = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("exit", ty));
        x_plvCrtExit->setDSOLocal(true);
        x_plvCrtExit->setDoesNotReturn();
        x_plvCrtExit->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtExit, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern int getchar(void);
        auto ty = llvm::FunctionType::get(y_drv.tyI32, false);
        x_plvCrtGetchar = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("getchar", ty));
        x_plvCrtGetchar->setDSOLocal(true);
        x_plvCrtGetchar->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtGetchar, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern int printf(const char *format, ...);
        auto ty = llvm::FunctionType::get(y_drv.tyI32, {y_drv.tyI8Ptr}, true);
        x_plvCrtPrintf = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("printf", ty));
        x_plvCrtPrintf->setDSOLocal(true);
        x_plvCrtPrintf->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtPrintf, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern int puts(const char *str);
        auto ty = llvm::FunctionType::get(y_drv.tyI32, {y_drv.tyI8Ptr}, false);
        x_plvCrtPuts = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("puts", ty));
        x_plvCrtPuts->setDSOLocal(true);
        x_plvCrtPuts->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtPuts, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern void *realloc(void *ptr, size_t new_size);
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tyVoidPtr, y_drv.tySize}, false);
        x_plvCrtRealloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("realloc", ty));
        x_plvCrtRealloc->setDSOLocal(true);
        x_plvCrtRealloc->setLinkage(llvm::GlobalValue::ExternalLinkage);
        x_plvCrtRealloc->setReturnDoesNotAlias();
        if (llvm::verifyFunction(*x_plvCrtRealloc, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern int scanf(const char *format, ...);
        auto ty = llvm::FunctionType::get(y_drv.tyI32, {y_drv.tyI8Ptr}, true);
        x_plvCrtScanf = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("scanf", ty));
        x_plvCrtScanf->setDSOLocal(true);
        x_plvCrtScanf->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtScanf, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // extern int strcmp(const char *lhs, const char *rhs);
        auto ty = llvm::FunctionType::get(y_drv.tyI32, {y_drv.tyI8Ptr, y_drv.tyI8Ptr}, false);
        x_plvCrtStrcmp = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("strcmp", ty));
        x_plvCrtStrcmp->setDSOLocal(true);
        x_plvCrtStrcmp->setLinkage(llvm::GlobalValue::ExternalLinkage);
        if (llvm::verifyFunction(*x_plvCrtStrcmp, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    // emit RTL
    {
        // __declspec(noreturn) void __fastcall #Rtl.Fatal(const char *pszMsg) {
        //     puts(pszMsg);
        //     exit(-1);
        // }
        auto ty = llvm::FunctionType::get(y_drv.tyVoid, {y_drv.tyI8Ptr}, false);
        x_plvRtlFatal = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.Fatal", ty));
        x_plvRtlFatal->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlFatal->setDSOLocal(true);
        x_plvRtlFatal->setDoesNotReturn();
        x_plvRtlFatal->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto msg = x_plvRtlFatal->arg_begin();
        msg->setName("pszMsg");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlFatal);
        x_lvBld.SetInsertPoint(bb);
        x_lvBld.CreateCall(x_plvCrtPuts, {msg});
        x_lvBld.CreateCall(x_plvCrtExit, {llvm::ConstantInt::get(y_drv.tyI32, -1)});
        x_lvBld.CreateUnreachable();
        if (llvm::verifyFunction(*x_plvRtlFatal, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // __declspec(noalias) void *__fastcall #Rtl.Alloc(size_t cb) {
        //     void *ptr = calloc(1, cb);
        //     if (!ptr)
        //         #Rtl.Fatal("[FATAL] Failed to allocate memory.");
        //     return ptr;
        // }
        auto sfAlloc = X_EmitStringLiteral("[FATAL] Failed to allocate memory.");
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tySize}, false);
        x_plvRtlAlloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.Alloc", ty));
        x_plvRtlAlloc->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlAlloc->setDSOLocal(true);
        x_plvRtlAlloc->setLinkage(llvm::GlobalValue::PrivateLinkage);
        x_plvRtlAlloc->setReturnDoesNotAlias();
        auto cb = x_plvRtlAlloc->arg_begin();
        cb->setName("cb");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlAlloc);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlAlloc);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlAlloc);
        x_lvBld.SetInsertPoint(bb);
        auto ptr = x_lvBld.CreateCall(x_plvCrtCalloc, {llvm::ConstantInt::get(y_drv.tySize, 1), cb});
        x_lvBld.CreateCondBr(x_lvBld.CreateICmpEQ(ptr, y_drv.ptrVoidNull), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfAlloc});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        x_lvBld.CreateRet(ptr);
        if (llvm::verifyFunction(*x_plvRtlAlloc, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // __declspec(noalias) void *__fastcall #Rtl.ReAlloc(void *ptr, size_t cb) {
        //     void *res = realloc(ptr, cb);
        //     if (!res)
        //         #Rtl.Fatal("[FATAL] Failed to allocate memory.");
        //     return res;
        // }
        auto sfAlloc = X_EmitStringLiteral("[FATAL] Failed to allocate memory.");
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tyVoidPtr, y_drv.tySize}, false);
        x_plvRtlReAlloc = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.ReAlloc", ty));
        x_plvRtlReAlloc->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlReAlloc->setDSOLocal(true);
        x_plvRtlReAlloc->setLinkage(llvm::GlobalValue::PrivateLinkage);
        x_plvRtlReAlloc->setReturnDoesNotAlias();
        auto ptr = x_plvRtlReAlloc->arg_begin();
        auto cb = x_plvRtlReAlloc->arg_begin() + 1;
        ptr->setName("ptr");
        cb->setName("cb");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlReAlloc);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlReAlloc);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlReAlloc);
        x_lvBld.SetInsertPoint(bb);
        auto res = x_lvBld.CreateCall(x_plvCrtRealloc, {ptr, cb});
        x_lvBld.CreateCondBr(x_lvBld.CreateICmpEQ(res, y_drv.ptrVoidNull), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfAlloc});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        x_lvBld.CreateRet(res);
        if (llvm::verifyFunction(*x_plvRtlReAlloc, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // __declspec(noalias) void *__fastcall #Rtl.AllocArray(int nLen, size_t cbOff, size_t cbElem) {
        //     if (nLen < 0)
        //         #Rtl.Fatal("[FATAL] Array length cannot be less than 0.");
        //     return #Rtl.Alloc(cbOff + cbElem * ((size_t) (unsigned) nLen));
        // }
        auto sfArrayLen = X_EmitStringLiteral("[FATAL] Array length cannot be less than 0.");
        auto ty = llvm::FunctionType::get(y_drv.tyVoidPtr, {y_drv.tyI32, y_drv.tySize, y_drv.tySize}, false);
        x_plvRtlAllocArray = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.AllocArray", ty));
        x_plvRtlAllocArray->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlAllocArray->setDSOLocal(true);
        x_plvRtlAllocArray->setLinkage(llvm::GlobalValue::PrivateLinkage);
        x_plvRtlAllocArray->setReturnDoesNotAlias();
        auto len = x_plvRtlAllocArray->arg_begin();
        auto cbOff = x_plvRtlAllocArray->arg_begin() + 1;
        auto cbElem = x_plvRtlAllocArray->arg_begin() + 2;
        len->setName("nLen");
        cbOff->setName("cbOff");
        cbElem->setName("cbElem");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlAllocArray);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlAllocArray);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlAllocArray);
        x_lvBld.SetInsertPoint(bb);
        auto zero = llvm::ConstantInt::get(y_drv.tyI32, 0);
        x_lvBld.CreateCondBr(x_lvBld.CreateICmpSLT(len, zero), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfArrayLen});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        auto count = x_lvBld.CreateIntCast(len, y_drv.tySize, false);
        auto cbArray = x_lvBld.CreateNSWMul(cbElem, count);
        auto cbTotal = x_lvBld.CreateNSWAdd(cbOff, cbArray);
        auto ptr = x_lvBld.CreateCall(x_plvRtlAlloc, {cbTotal});
        x_lvBld.CreateRet(ptr);
        if (llvm::verifyFunction(*x_plvRtlAllocArray, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // bool __fastcall #Rtl.InstanceOf(void *who, void *idx) {
        //     void *src = *(void **) who;
        //     while (src && src != idx)
        //         src = *(void **) src;
        //     return src == idx;
        // }
        auto ty = llvm::FunctionType::get(y_drv.tyI1, {y_drv.tyVoidPtr, y_drv.tyVoidPtr}, false);
        x_plvRtlInstanceOf = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.InstanceOf", ty));
        x_plvRtlInstanceOf->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlInstanceOf->setDSOLocal(true);
        x_plvRtlInstanceOf->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto who = x_plvRtlInstanceOf->arg_begin();
        auto idx = x_plvRtlInstanceOf->arg_begin() + 1;
        who->setName("who");
        idx->setName("idx");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlInstanceOf);
        auto bbWhCond = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-cond.0", x_plvRtlInstanceOf);
        auto bbWhBody = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-body.0", x_plvRtlInstanceOf);
        auto bbWhPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-post.0", x_plvRtlInstanceOf);
        x_lvBld.SetInsertPoint(bb);
        auto src = x_lvBld.CreateAlloca(y_drv.tyVoidPtr, nullptr, "src");
        auto next = x_lvBld.CreatePointerCast(who, y_drv.tyVoidPtrPtr);
        x_lvBld.CreateStore(x_lvBld.CreateLoad(next), src);
        x_lvBld.CreateBr(bbWhCond);
        x_lvBld.SetInsertPoint(bbWhCond);
        auto srcv = x_lvBld.CreateLoad(src);
        auto c1 = x_lvBld.CreateICmpNE(srcv, y_drv.ptrVoidNull);
        auto c2 = x_lvBld.CreateICmpNE(srcv, idx);
        x_lvBld.CreateCondBr(x_lvBld.CreateAnd(c1, c2), bbWhBody, bbWhPost);
        x_lvBld.SetInsertPoint(bbWhBody);
        srcv = x_lvBld.CreateLoad(src);
        next = x_lvBld.CreatePointerCast(srcv, y_drv.tyVoidPtrPtr);
        x_lvBld.CreateStore(x_lvBld.CreateLoad(next), src);
        x_lvBld.CreateBr(bbWhCond);
        x_lvBld.SetInsertPoint(bbWhPost);
        srcv = x_lvBld.CreateLoad(src);
        x_lvBld.CreateRet(x_lvBld.CreateICmpEQ(srcv, idx));
        if (llvm::verifyFunction(*x_plvRtlInstanceOf, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // void __fastcall #Rtl.CheckCast(void *who, void *idx) {
        //     if (!#Rtl.InstanceOf(who, idx))
        //         #Rtl.Fatal("[FATAL] Invalid cast.");
        // }
        auto sfCast = X_EmitStringLiteral("[FATAL] Invalid cast.");
        auto ty = llvm::FunctionType::get(y_drv.tyVoid, {y_drv.tyVoidPtr, y_drv.tyVoidPtr}, false);
        x_plvRtlCheckCast = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.CheckCast", ty));
        x_plvRtlCheckCast->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlCheckCast->setDSOLocal(true);
        x_plvRtlCheckCast->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto who = x_plvRtlCheckCast->arg_begin();
        auto idx = x_plvRtlCheckCast->arg_begin() + 1;
        who->setName("who");
        idx->setName("idx");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlCheckCast);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlCheckCast);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlCheckCast);
        x_lvBld.SetInsertPoint(bb);
        x_lvBld.CreateCondBr(x_lvBld.CreateCall(x_plvRtlInstanceOf, {who, idx}), bbIfPost, bbIfThen);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfCast});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        x_lvBld.CreateRetVoid();
        if (llvm::verifyFunction(*x_plvRtlCheckCast, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // void __fastcall #Rtl.CheckBound(int len, int idx) {
        //     if (idx < 0 || idx >= len)
        //         #Rtl.Fatal("[FATAL] Array access out of bound.");
        // }
        auto sfArrayIdx = X_EmitStringLiteral("[FATAL] Array access out of bound.");
        auto ty = llvm::FunctionType::get(y_drv.tyVoid, {y_drv.tyI32, y_drv.tyI32}, false);
        x_plvRtlCheckBound = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.CheckBound", ty));
        x_plvRtlCheckBound->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlCheckBound->setDSOLocal(true);
        x_plvRtlCheckBound->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto len = x_plvRtlCheckBound->arg_begin();
        auto idx = x_plvRtlCheckBound->arg_begin() + 1;
        len->setName("len");
        idx->setName("idx");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlCheckBound);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlCheckBound);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlCheckBound);
        x_lvBld.SetInsertPoint(bb);
        auto c1 = x_lvBld.CreateICmpSLT(idx, llvm::ConstantInt::get(y_drv.tyI32, 0));
        auto c2 = x_lvBld.CreateICmpSGE(idx, len);
        x_lvBld.CreateCondBr(x_lvBld.CreateOr(c1, c2), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfArrayIdx});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        x_lvBld.CreateRetVoid();
        if (llvm::verifyFunction(*x_plvRtlCheckBound, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // int __fastcall #Rtl.ReadInteger() {
        //     int val;
        //     if (scanf("%d", &val) != 1)
        //         #Rtl.Fatal("[FATAL] Invalid integer.");
        //     return val;
        // }
        auto sfInvalidInt = X_EmitStringLiteral("[FATAL] Invalid integer.");
        auto fmt = X_EmitStringLiteral("%d");
        auto ty = llvm::FunctionType::get(y_drv.tyI32, false);
        x_plvRtlReadInteger = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.ReadInteger", ty));
        x_plvRtlReadInteger->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlReadInteger->setDSOLocal(true);
        x_plvRtlReadInteger->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlReadInteger);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlReadInteger);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlReadInteger);
        x_lvBld.SetInsertPoint(bb);
        auto val = x_lvBld.CreateAlloca(y_drv.tyI32, nullptr, "val");
        auto res = x_lvBld.CreateCall(x_plvCrtScanf, {fmt, val});
        x_lvBld.CreateCondBr(x_lvBld.CreateICmpNE(res, llvm::ConstantInt::get(y_drv.tyI32, 1)), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        x_lvBld.CreateCall(x_plvRtlFatal, {sfInvalidInt});
        x_lvBld.CreateUnreachable();
        x_lvBld.SetInsertPoint(bbIfPost);
        x_lvBld.CreateRet(x_lvBld.CreateLoad(val));
        if (llvm::verifyFunction(*x_plvRtlReadInteger, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // char *__fastcall #Rtl.ReadLine() {
        //     size_t len = 0;
        //     size_t cap = 256;
        //     char *buf = #Rtl.ReAlloc(nullptr, cap);
        //     int ch = getchar();
        //     while (ch > 0 && ch != '\n') {
        //         register int tmp = len + 1;
        //         if (tmp >= cap) {
        //             cap <<= 1;
        //             buf = #Rtl.ReAlloc(buf, cap);
        //         }
        //         buf[len] = (char) ch;
        //         len = tmp;
        //         ch = getchar();
        //     }
        //     buf[len] = '\0';
        //     return buf;
        // }
        auto ty = llvm::FunctionType::get(y_drv.tyI8Ptr, false);
        x_plvRtlReadLine = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.ReadLine", ty));
        x_plvRtlReadLine->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlReadLine->setDSOLocal(true);
        x_plvRtlReadLine->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlReadLine);
        auto bbWhCond = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-cond.0", x_plvRtlReadLine);
        auto bbWhBody = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-body.0", x_plvRtlReadLine);
        auto bbIfThen = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-then.0", x_plvRtlReadLine);
        auto bbIfPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#if-post.0", x_plvRtlReadLine);
        auto bbWhPost = llvm::BasicBlock::Create(y_drv.lvCtx, "#while-post.0", x_plvRtlReadLine);
        x_lvBld.SetInsertPoint(bb);
        auto i1 = llvm::ConstantInt::get(y_drv.tySize, 1);
        auto i256 = llvm::ConstantInt::get(y_drv.tySize, 256);
        auto len = x_lvBld.CreateAlloca(y_drv.tySize, nullptr, "len");
        auto cap = x_lvBld.CreateAlloca(y_drv.tySize, nullptr, "cap");
        auto buf = x_lvBld.CreateAlloca(y_drv.tyI8Ptr, nullptr, "buf");
        auto ch = x_lvBld.CreateAlloca(y_drv.tyI32, nullptr, "ch");
        x_lvBld.CreateStore(llvm::ConstantInt::get(y_drv.tySize, 0), len);
        x_lvBld.CreateStore(i256, cap);
        x_lvBld.CreateStore(x_lvBld.CreateCall(x_plvRtlReAlloc, {y_drv.ptrVoidNull, i256}), buf);
        x_lvBld.CreateStore(x_lvBld.CreateCall(x_plvCrtGetchar), ch);
        x_lvBld.CreateBr(bbWhCond);
        x_lvBld.SetInsertPoint(bbWhCond);
        auto chv = x_lvBld.CreateLoad(ch);
        auto c1 = x_lvBld.CreateICmpSGT(chv, llvm::ConstantInt::get(y_drv.tyI32, 0));
        auto c2 = x_lvBld.CreateICmpNE(chv, llvm::ConstantInt::get(y_drv.tyI32, '\n'));
        x_lvBld.CreateCondBr(x_lvBld.CreateAnd(c1, c2), bbWhBody, bbWhPost);
        x_lvBld.SetInsertPoint(bbWhBody);
        auto lenv = x_lvBld.CreateLoad(len);
        auto tmp = x_lvBld.CreateAdd(lenv, i1);
        auto capv = static_cast<llvm::Value *>(x_lvBld.CreateLoad(cap));
        x_lvBld.CreateCondBr(x_lvBld.CreateICmpUGE(tmp, capv), bbIfThen, bbIfPost);
        x_lvBld.SetInsertPoint(bbIfThen);
        capv = x_lvBld.CreateShl(capv, i1);
        x_lvBld.CreateStore(capv, cap);
        auto bufv = x_lvBld.CreateLoad(buf);
        x_lvBld.CreateStore(x_lvBld.CreateCall(x_plvRtlReAlloc, {bufv, capv}), buf);
        x_lvBld.CreateBr(bbIfPost);
        x_lvBld.SetInsertPoint(bbIfPost);
        bufv = x_lvBld.CreateLoad(buf);
        auto chp = x_lvBld.CreateGEP(bufv, lenv);
        x_lvBld.CreateStore(x_lvBld.CreateIntCast(chv, y_drv.tyI8, false), chp);
        x_lvBld.CreateStore(tmp, len);
        x_lvBld.CreateStore(x_lvBld.CreateCall(x_plvCrtGetchar), ch);
        x_lvBld.CreateBr(bbWhCond);
        x_lvBld.SetInsertPoint(bbWhPost);
        lenv = x_lvBld.CreateLoad(len);
        bufv = x_lvBld.CreateLoad(buf);
        chp = x_lvBld.CreateGEP(bufv, lenv);
        x_lvBld.CreateStore(llvm::ConstantInt::get(y_drv.tyI8, '\0'), chp);
        x_lvBld.CreateRet(bufv);
        if (llvm::verifyFunction(*x_plvRtlReadLine, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    {
        // int __fastcall #Rtl.StrCmp(const char *lhs, const char *rhs) {
        //     return strcmp(lhs, rhs);
        // }
        auto ty = llvm::FunctionType::get(y_drv.tyI32, {y_drv.tyI8Ptr, y_drv.tyI8Ptr}, false);
        x_plvRtlStrCmp = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("#Rtl.StrCmp", ty));
        x_plvRtlStrCmp->setCallingConv(llvm::CallingConv::Fast);
        x_plvRtlStrCmp->setDSOLocal(true);
        x_plvRtlStrCmp->setLinkage(llvm::GlobalValue::PrivateLinkage);
        auto lhs = x_plvRtlStrCmp->arg_begin();
        auto rhs = x_plvRtlStrCmp->arg_begin() + 1;
        lhs->setName("lhs");
        rhs->setName("rhs");
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", x_plvRtlStrCmp);
        x_lvBld.SetInsertPoint(bb);
        x_lvBld.CreateRet(x_lvBld.CreateCall(x_plvCrtStrcmp, {lhs, rhs}));
        if (llvm::verifyFunction(*x_plvRtlStrCmp, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
    // emit bool strlit
    {
        auto sFalse = X_EmitStringLiteral("false");
        auto sTrue = X_EmitStringLiteral("true");
        auto ty = llvm::ArrayType::get(y_drv.tyI8Ptr, 2);
        auto gv = llvm::cast<llvm::GlobalVariable>(x_plvMod->getOrInsertGlobal("#BoolStr", ty));
        gv->setAlignment(y_drv.lvDataLayout.getPrefTypeAlignment(ty));
        gv->setConstant(true);
        gv->setDSOLocal(true);
        gv->setInitializer(llvm::ConstantArray::get(ty, {sFalse, sTrue}));
        gv->setLinkage(llvm::GlobalValue::PrivateLinkage);
        x_plvBoolStr = gv;
    }
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
    // emit main function
    {
        // int main(void) {
        //     #Class.Main.main();
        //     return 0;
        // }
        auto ty = llvm::FunctionType::get(y_drv.tyI32, false);
        auto fn = llvm::cast<llvm::Function>(x_plvMod->getOrInsertFunction("main", ty));
        fn->setDSOLocal(true);
        fn->setLinkage(llvm::GlobalValue::ExternalLinkage);
        auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", fn);
        x_lvBld.SetInsertPoint(bb);
        auto pmain = vProg.GetMain();
        if (pmain)
            x_lvBld.CreateCall(pmain->GetLvFn());
        else
            PrintWarning(vProg.GetLocation(), "no entry point found");
        x_lvBld.CreateRet(llvm::ConstantInt::get(y_drv.tyI32, 0));
        if (llvm::verifyFunction(*fn, &llvm::errs())) {
            Y_Reject();
            return;
        }
    }
}

void CodeGenVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        if (auto pFn = dynamic_cast<FnDef *>(upField.get()))
            pFn->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(FnDef &vFn) noexcept {
    llvm::Function *fn = vFn.GetLvFn();
    auto bb = llvm::BasicBlock::Create(y_drv.lvCtx, "#fn-entry", fn);
    x_lvBld.SetInsertPoint(bb);
    for (auto &upPar : vFn.GetPars())
        upPar->AcceptVisitor(*this);
    auto arg = fn->arg_begin();
    if (!vFn.IsStatic())
        ++arg;
    for (auto &upPar : vFn.GetPars()) {
        auto ptr = upPar->GetLvPtr();
        x_lvBld.CreateStore(arg++, ptr);
    }
    ENTER_SCOPE(x_uForStmtCnt, 0);
    ENTER_SCOPE(x_uIfStmtCnt, 0);
    ENTER_SCOPE(x_uWhileStmtCnt, 0);
    if (vFn.IsStatic())
        vFn.GetBody()->AcceptVisitor(*this);
    else {
        ENTER_SCOPE(x_plvThis, fn->arg_begin());
        vFn.GetBody()->AcceptVisitor(*this);
    }
    if (!x_lvBld.GetInsertBlock()->getTerminator()) {
        if (fn->getReturnType()->isVoidTy())
            x_lvBld.CreateRetVoid();
        else {
            Y_RjNoReturnVal(vFn.GetLocation(), vFn.GetType(), vFn.GetName());
            return;
        }
    }
    if (llvm::verifyFunction(*fn, &llvm::errs())) {
        Y_Reject();
        return;
    }
}

void CodeGenVisitor::Visit(VarDef &vVar) noexcept {
    assert(!vVar.IsField());
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(vVar.GetLocation(), "unreachable code");
        return;
    }
    auto ptr = x_lvBld.CreateAlloca(vVar.GetType().GetLvType(), nullptr, vVar.GetName());
    vVar.SetLvPtr(ptr);
}

void CodeGenVisitor::Visit(BlockStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    for (auto &upItem : stmt.GetItems())
        upItem->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(BreakStmt &stmt) noexcept {
    assert(x_plvPost);
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    x_lvBld.CreateBr(x_plvPost);
}

void CodeGenVisitor::Visit(ExprStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    if (stmt.GetExpr())
        stmt.GetExpr()->AcceptVisitor(*this);
}

void CodeGenVisitor::Visit(ForStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#for-cond.%u", x_uForStmtCnt));
    auto bbBody = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#for-body.%u", x_uForStmtCnt));
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#for-post.%u", x_uForStmtCnt));
    ++x_uForStmtCnt;
    if (stmt.GetInit())
        stmt.GetInit()->AcceptVisitor(*this);
    bbCond->insertInto(fn);
    x_lvBld.CreateBr(bbCond);
    x_lvBld.SetInsertPoint(bbCond);
    if (stmt.GetCond()) {
        stmt.GetCond()->AcceptVisitor(*this);
        x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    }
    else
        x_lvBld.CreateBr(bbBody);
    bbBody->insertInto(fn);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        if (stmt.GetIncr())
            PrintWarning(stmt.GetIncr()->GetLocation(), "unreachable code");
    }
    else {
        if (stmt.GetIncr())
            stmt.GetIncr()->AcceptVisitor(*this);
        x_lvBld.CreateBr(bbCond);
    }
    bbPost->insertInto(fn);
    if (llvm::pred_empty(bbPost))
        bbPost->eraseFromParent();
    else
        x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(IfStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbThen = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#if-then.%u", x_uIfStmtCnt));
    auto bbElse = stmt.GetElse() ?
        llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#if-else.%u", x_uIfStmtCnt)) :
        nullptr;
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#if-post.%u", x_uIfStmtCnt));
    ++x_uIfStmtCnt;
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbThen, stmt.GetElse() ? bbElse : bbPost);
    bbThen->insertInto(fn);
    x_lvBld.SetInsertPoint(bbThen);
    stmt.GetThen()->AcceptVisitor(*this);
    if (!x_lvBld.GetInsertBlock()->getTerminator())
        x_lvBld.CreateBr(bbPost);
    if (stmt.GetElse()) {
        bbElse->insertInto(fn);
        x_lvBld.SetInsertPoint(bbElse);
        stmt.GetElse()->AcceptVisitor(*this);
        if (!x_lvBld.GetInsertBlock()->getTerminator())
            x_lvBld.CreateBr(bbPost);
    }
    bbPost->insertInto(fn);
    if (llvm::pred_empty(bbPost))
        bbPost->eraseFromParent();
    else
        x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(PrintStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    llvm::SmallVector<llvm::Value *, 64> vec {nullptr};
    llvm::SmallString<256> fmt;
    for (auto &upArg : stmt.GetArgs()) {
        upArg->AcceptVisitor(*this);
        auto val = x_plvRet;
        if (upArg->GetType() == x_pTyReg->tyInt) {
            fmt += "%d";
            vec.emplace_back(val);
        }
        else if (upArg->GetType() == x_pTyReg->tyBool) {
            fmt += "%s";
            val = x_lvBld.CreateIntCast(val, y_drv.tySize, false);
            val = x_lvBld.CreateGEP(x_plvBoolStr, {llvm::ConstantInt::get(y_drv.tySize, 0), val});
            vec.emplace_back(x_lvBld.CreateLoad(val));
        }
        else if (upArg->GetType() == x_pTyReg->tyString) {
            fmt += "%s";
            vec.emplace_back(val);
        }
    }
    vec[0] = X_EmitStringLiteral(fmt.c_str());
    x_lvBld.CreateCall(x_plvCrtPrintf, vec);
}

void CodeGenVisitor::Visit(ReturnStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        x_lvBld.CreateRet(x_lvBld.CreatePointerCast(
            x_plvRet, x_lvBld.GetInsertBlock()->getParent()->getReturnType()
        ));
    }
    else {
        x_lvBld.CreateRetVoid();
    }
}

void CodeGenVisitor::Visit(WhileStmt &stmt) noexcept {
    if (x_lvBld.GetInsertBlock()->getTerminator()) {
        PrintWarning(stmt.GetLocation(), "unreachable code");
        return;
    }
    auto fn = x_lvBld.GetInsertBlock()->getParent();
    auto bbCond = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#while-cond.%u", x_uWhileStmtCnt));
    auto bbBody = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#while-body.%u", x_uWhileStmtCnt));
    auto bbPost = llvm::BasicBlock::Create(y_drv.lvCtx, FmtStr("#while-post.%u", x_uWhileStmtCnt));
    ++x_uWhileStmtCnt;
    x_lvBld.CreateBr(bbCond);
    bbCond->insertInto(fn);
    x_lvBld.SetInsertPoint(bbCond);
    stmt.GetCond()->AcceptVisitor(*this);
    x_lvBld.CreateCondBr(x_plvRet, bbBody, bbPost);
    bbBody->insertInto(fn);
    x_lvBld.SetInsertPoint(bbBody);
    ENTER_SCOPE(x_plvPost, bbPost);
    stmt.GetBody()->AcceptVisitor(*this);
    if (!x_lvBld.GetInsertBlock()->getTerminator())
        x_lvBld.CreateBr(bbCond);
    bbPost->insertInto(fn);
    x_lvBld.SetInsertPoint(bbPost);
}

void CodeGenVisitor::Visit(AssignExpr &expr) noexcept {
    expr.GetRhs()->AcceptVisitor(*this);
    auto val = x_plvRet;
    expr.GetLhs()->SetGenPtr(true);
    expr.GetLhs()->AcceptVisitor(*this);
    auto ptr = x_plvRet;
    x_lvBld.CreateStore(x_lvBld.CreateBitOrPointerCast(val, expr.GetLhs()->GetType().GetLvType()), ptr);
    x_plvRet = val;
}

void CodeGenVisitor::Visit(BinaryExpr &expr) noexcept {
    expr.GetLhs()->AcceptVisitor(*this);
    auto lhs = x_plvRet;
    expr.GetRhs()->AcceptVisitor(*this);
    auto rhs = x_plvRet;
    switch (expr.GetOp()) {
    case BinOp::kIor:
        x_plvRet = x_lvBld.CreateOr(lhs, rhs);
        break;
    case BinOp::kAnd:
        x_plvRet = x_lvBld.CreateAnd(lhs, rhs);
        break;
    case BinOp::kEqu:
        if (expr.GetLhs()->GetType() == x_pTyReg->tyString) {
            lhs = x_lvBld.CreateCall(x_plvRtlStrCmp, {lhs, rhs});
            rhs = llvm::ConstantInt::get(y_drv.tyI32, 0);
        }
        x_plvRet = x_lvBld.CreateICmpEQ(lhs, rhs);
        break;
    case BinOp::kNeq:
        if (expr.GetLhs()->GetType() == x_pTyReg->tyString) {
            lhs = x_lvBld.CreateCall(x_plvRtlStrCmp, {lhs, rhs});
            rhs = llvm::ConstantInt::get(y_drv.tyI32, 0);
        }
        x_plvRet = x_lvBld.CreateICmpNE(lhs, rhs);
        break;
    case BinOp::kLes:
        x_plvRet = x_lvBld.CreateICmpSLT(lhs, rhs);
        break;
    case BinOp::kGre:
        x_plvRet = x_lvBld.CreateICmpSGT(lhs, rhs);
        break;
    case BinOp::kLeq:
        x_plvRet = x_lvBld.CreateICmpSLE(lhs, rhs);
        break;
    case BinOp::kGeq:
        x_plvRet = x_lvBld.CreateICmpSGE(lhs, rhs);
        break;
    case BinOp::kAdd:
        x_plvRet = x_lvBld.CreateNSWAdd(lhs, rhs);
        break;
    case BinOp::kSub:
        x_plvRet = x_lvBld.CreateNSWSub(lhs, rhs);
        break;
    case BinOp::kMul:
        x_plvRet = x_lvBld.CreateNSWMul(lhs, rhs);
        break;
    case BinOp::kDiv:
        x_plvRet = x_lvBld.CreateSDiv(lhs, rhs);
        break;
    case BinOp::kMod:
        x_plvRet = x_lvBld.CreateSRem(lhs, rhs);
        break;
    }
}

void CodeGenVisitor::Visit(CallExpr &expr) noexcept {
    if (expr.GetExpr())
        expr.GetExpr()->AcceptVisitor(*this);
    else
        x_plvRet = x_plvThis;
    auto who = x_plvRet;
    if (expr.IsArrayLength())
        x_plvRet = x_lvBld.CreateLoad(x_lvBld.CreateStructGEP(who, 0));
    else {
        auto &vFn = expr.GetFn();
        llvm::SmallVector<llvm::Value *, 64> vec;
        if (!vFn.IsStatic())
            vec.emplace_back(who);
        auto itPar = expr.GetFn().GetPars().begin();
        for (auto &upArg : expr.GetArgs()) {
            upArg->AcceptVisitor(*this);
            vec.emplace_back(x_lvBld.CreateBitOrPointerCast(x_plvRet, (*itPar)->GetType().GetLvType()));
            ++itPar;
        }
        if (vFn.IsStatic())
            x_plvRet = x_lvBld.CreateCall(vFn.GetLvFn(), vec);
        else {
            // virtual call
            // fn** vtable = *(fn ***) who;
            // fn* pfn = vtable[idx + 1];
            // pfn(args...);
            auto ty = vFn.GetLvFn()->getFunctionType();
            auto typpp = ty->getPointerTo()->getPointerTo()->getPointerTo();
            auto vtable = x_lvBld.CreateLoad(x_lvBld.CreatePointerCast(who, typpp));
            auto idx = llvm::ConstantInt::get(y_drv.tySize, vFn.GetIdx() + 1);
            auto pfn = x_lvBld.CreateLoad(x_lvBld.CreateGEP(vtable, idx));
            x_plvRet = x_lvBld.CreateCall(ty, pfn, vec);
        }
    }
}

void CodeGenVisitor::Visit(CastExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    if (!expr.GetType().Accepts(expr.GetExpr()->GetType())) {
        // dynamic cast
        auto who = x_lvBld.CreatePointerCast(x_plvRet, y_drv.tyVoidPtr);
        auto i8p = llvm::ConstantExpr::getPointerCast(x_plvClassIdx, y_drv.tyI8Ptr);
        auto iidx = static_cast<const ClassDef *>(&expr.GetType().GetElemType())->GetIdx();
        auto ciIdx = llvm::ConstantInt::get(y_drv.tySize, iidx * y_drv.uPtrSize);
        auto idx = llvm::ConstantExpr::getGetElementPtr(y_drv.tyI8, i8p, ciIdx);
        x_lvBld.CreateCall(x_plvRtlCheckCast, {who, idx});
    }
    // static cast
    x_plvRet = x_lvBld.CreatePointerCast(x_plvRet, expr.GetType().GetLvType());
}

void CodeGenVisitor::Visit(NewArrayExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto len = x_plvRet;
    auto ty = llvm::cast<llvm::PointerType>(expr.GetType().GetLvType());
    auto tyElem = x_pTyReg->DecDim(expr.GetType()).GetLvType();
    auto cbOff = llvm::ConstantInt::get(y_drv.tySize, y_drv.lvDataLayout.getTypeAllocSize(ty->getElementType()));
    auto cbElem = llvm::ConstantInt::get(y_drv.tySize, y_drv.lvDataLayout.getTypeAllocSize(tyElem));
    auto ptr = x_lvBld.CreatePointerCast(x_lvBld.CreateCall(x_plvRtlAllocArray, {len, cbOff, cbElem}), ty);
    x_lvBld.CreateStore(len, x_lvBld.CreateStructGEP(ptr, 0));
    x_plvRet = ptr;
}

void CodeGenVisitor::Visit(NewClassExpr &expr) noexcept {
    auto ty = llvm::cast<llvm::PointerType>(expr.GetType().GetLvType());
    auto cb = llvm::ConstantInt::get(y_drv.tySize, y_drv.lvDataLayout.getTypeAllocSize(ty->getElementType()));
    auto ptr = x_lvBld.CreatePointerCast(x_lvBld.CreateCall(x_plvRtlAlloc, {cb}), ty);
    auto vtable = x_lvBld.CreatePointerCast(ptr, y_drv.tyVoidPtrPtr);
    auto pClass = static_cast<const ClassDef *>(&expr.GetType().GetElemType());
    x_lvBld.CreateStore(llvm::ConstantExpr::getPointerCast(pClass->GetLvVTable(), y_drv.tyVoidPtr), vtable);
    x_plvRet = ptr;
}

void CodeGenVisitor::Visit(UnaryExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    switch (expr.GetOp()) {
    case UnaOp::kNeg:
        x_plvRet = x_lvBld.CreateNSWNeg(x_plvRet);
        break;
    case UnaOp::kNot:
        x_plvRet = x_lvBld.CreateNot(x_plvRet);
        break;
    }
}

void CodeGenVisitor::Visit(ArrayAccess &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto ptr = x_plvRet;
    expr.GetSub()->AcceptVisitor(*this);
    auto idx = x_plvRet;
    auto len = x_lvBld.CreateLoad(x_lvBld.CreateStructGEP(ptr, 0));
    x_lvBld.CreateCall(x_plvRtlCheckBound, {len, idx});
    idx = x_lvBld.CreateIntCast(idx, y_drv.tySize, false);
    ptr = x_lvBld.CreateGEP(
        ptr,
        {llvm::ConstantInt::get(y_drv.tySize, 0), llvm::ConstantInt::get(y_drv.tyI32, 1), idx}
    );
    x_plvRet = expr.IsGenPtr() ? ptr : x_lvBld.CreateLoad(ptr);
}

void CodeGenVisitor::Visit(InstanceOf &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto who = x_lvBld.CreatePointerCast(x_plvRet, y_drv.tyVoidPtr);
    auto i8p = llvm::ConstantExpr::getPointerCast(x_plvClassIdx, y_drv.tyI8Ptr);
    auto ciIdx = llvm::ConstantInt::get(y_drv.tySize, expr.GetClass().GetIdx() * y_drv.uPtrSize);
    auto idx = llvm::ConstantExpr::getGetElementPtr(y_drv.tyI8, i8p, ciIdx);
    x_plvRet = x_lvBld.CreateCall(x_plvRtlInstanceOf, {who, idx});
}

void CodeGenVisitor::Visit(ReadInteger &expr) noexcept {
    x_plvRet = x_lvBld.CreateCall(x_plvRtlReadInteger);
}

void CodeGenVisitor::Visit(ReadLine &expr) noexcept {
    x_plvRet = x_lvBld.CreateCall(x_plvRtlReadLine);
}

void CodeGenVisitor::Visit(This &expr) noexcept {
    x_plvRet = x_plvThis;
}

void CodeGenVisitor::Visit(VarAccess &expr) noexcept {
    auto pClass = (const ClassDef *) x_pClass;
    auto who = static_cast<llvm::Value *>(x_plvThis);
    if (expr.GetExpr()) {
        expr.GetExpr()->AcceptVisitor(*this);
        pClass = static_cast<const ClassDef *>(&expr.GetExpr()->GetType().GetElemType());
        who = x_plvRet;
    }
    if (expr.GetVar().IsField()) {
        llvm::SmallVector<llvm::Value *, 64> vec {llvm::ConstantInt::get(y_drv.tySize, 0)};
        while (pClass != &expr.GetVar().GetClass()) {
            pClass = pClass->GetBase();
            vec.emplace_back(llvm::ConstantInt::get(y_drv.tyI32, 0));
        }
        vec.emplace_back(llvm::ConstantInt::get(y_drv.tyI32, expr.GetVar().GetIdx() + 1));
        who = x_lvBld.CreateGEP(who, vec);
    }
    else
        who = expr.GetVar().GetLvPtr();
    assert(who);
    x_plvRet = expr.IsGenPtr() ? who : x_lvBld.CreateLoad(who);
}

void CodeGenVisitor::Visit(BoolLit &expr) noexcept {
    x_plvRet = expr.GetVal() ? llvm::ConstantInt::getTrue(y_drv.tyI1) : llvm::ConstantInt::getFalse(y_drv.tyI1);
}

void CodeGenVisitor::Visit(IntLit &expr) noexcept {
    x_plvRet = llvm::ConstantInt::get(y_drv.tyI32, expr.GetVal());
}

void CodeGenVisitor::Visit(NullLit &expr) noexcept {
    x_plvRet = y_drv.ptrVoidNull;
}

void CodeGenVisitor::Visit(StrLit &expr) noexcept {
    x_plvRet = X_EmitStringLiteral(expr.GetVal().c_str());
}

}

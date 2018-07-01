#include <llvm/IR/Constants.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

#include "CGContext.hpp"

namespace udc::cg {

namespace {
inline std::string X_InitTargetsAndGetTriple() {
    static bool bInited = false;
    if (!bInited) {
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();
        bInited = true;
    }
    return llvm::sys::getDefaultTargetTriple();
}

inline const llvm::Target *X_GetTarget(const std::string &sTriple) {
    std::string sError;
    auto pTarget = llvm::TargetRegistry::lookupTarget(sTriple, sError);
    if (!pTarget)
        throw std::runtime_error(sError);
    return pTarget;
}

inline llvm::TargetMachine *X_GetTargetMachine(const llvm::Target *plvTarget, const std::string &sTriple) {
    auto pTargetMachine = plvTarget->createTargetMachine(sTriple, {}, {}, {}, {});
    if (!pTargetMachine)
        throw std::runtime_error("Failed to create target machine");
    return pTargetMachine;
}

}

CGContext::CGContext() :
    sTriple(X_InitTargetsAndGetTriple()),
    plvTarget(X_GetTarget(sTriple)),
    uplvTargetMachine(X_GetTargetMachine(plvTarget, sTriple)),
    lvDataLayout(uplvTargetMachine->createDataLayout()),
    uPtrSize(lvDataLayout.getPointerSize()),
    tyVoid(llvm::Type::getVoidTy(lvCtx)),
    tyI1(llvm::Type::getInt1Ty(lvCtx)),
    tyI8(llvm::Type::getInt8Ty(lvCtx)),
    tyI32(llvm::Type::getInt32Ty(lvCtx)),
    tySize(lvDataLayout.getIntPtrType(lvCtx)),
    tyI1Ptr(tyI1->getPointerTo()),
    tyI8Ptr(tyI8->getPointerTo()),
    tyI32Ptr(tyI32->getPointerTo()),
    tyI8PtrPtr(tyI8Ptr->getPointerTo()),
    ptrVoidNull(llvm::ConstantPointerNull::get(tyVoidPtr))
{}

}

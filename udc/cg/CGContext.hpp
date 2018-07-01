#ifndef UDC_CG_CG_CONTEXT_HPP_
#define UDC_CG_CG_CONTEXT_HPP_

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>

namespace llvm {
class ConstantPointerNull;
class Target;
class TargetMachine;
}

namespace udc::cg {

class CGContext {
public:
    llvm::LLVMContext lvCtx;
    const std::string sTriple;
    const llvm::Target *const plvTarget;
    const std::unique_ptr<llvm::TargetMachine> uplvTargetMachine;
    const llvm::DataLayout lvDataLayout;

    const unsigned uPtrSize;
    llvm::Type *const tyVoid;
    llvm::IntegerType *const tyI1;
    llvm::IntegerType *const tyI8;
    llvm::IntegerType *const tyI32;
    llvm::IntegerType *const tySize;
    llvm::PointerType *const tyI1Ptr;
    union {
        llvm::PointerType *const tyI8Ptr;
        llvm::PointerType *const tyVoidPtr;
    };
    llvm::PointerType *const tyI32Ptr;
    union {
        llvm::PointerType *const tyI8PtrPtr;
        llvm::PointerType *const tyVoidPtrPtr;
    };
    llvm::ConstantPointerNull *const ptrVoidNull;

    CGContext();
};

}

#endif

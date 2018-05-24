#ifndef UDC_DRIVER_HPP_
#define UDC_DRIVER_HPP_

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>

#include "Location.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"

#include "ast/Fwd.hpp"

namespace llvm{
class Target;
class TargetMachine;
}

namespace udc {

class Driver {
private:
    std::string X_InitTriple();
    const llvm::Target *X_InitTarget();
    llvm::TargetMachine *X_InitTargetMachine();
    llvm::DataLayout X_InitDataLayout();

public:
    llvm::LLVMContext lvCtx;
    const std::string sTriple;
    const llvm::Target *const plvTarget;
    llvm::TargetMachine *const plvTargetMachine;
    const llvm::DataLayout lvDataLayout;

public:
    const unsigned uPtrSize = lvDataLayout.getPointerSize();
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

public:
    Driver();
    ~Driver();

public:
    int Parse();
    void PrintSymbols();
    void SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept;
    
private:
    Scanner x_vScanner;
    Parser x_vParser;
    std::unique_ptr<ast::Program> x_upProg;

};

}

#endif

#include <llvm/IR/Module.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>

#include "Driver.hpp"
#include "ast/eval/ClassNameVisitor.hpp"
#include "ast/eval/InheritVisitor.hpp"
#include "ast/eval/FieldVisitor.hpp"
#include "ast/eval/TypeVisitor.hpp"
#include "cg/ClassGenVisitor.hpp"
#include "cg/CodeGenVisitor.hpp"
#include "cg/FnGenVisitor.hpp"

namespace udc {

inline std::string Driver::X_InitTriple() {
    return llvm::sys::getDefaultTargetTriple();
}

inline const llvm::Target *Driver::X_InitTarget() {
    std::string sError;
    auto pTarget = llvm::TargetRegistry::lookupTarget(sTriple, sError);
    if (!pTarget)
        throw std::runtime_error(sError);
    return pTarget;
}

inline llvm::TargetMachine *Driver::X_InitTargetMachine() {
    return plvTarget->createTargetMachine(sTriple, {}, {}, {}, {});
}

inline llvm::DataLayout Driver::X_InitDataLayout() {
    return plvTargetMachine->createDataLayout();
}

Driver::Driver() :
    sTriple(X_InitTriple()),
    plvTarget(X_InitTarget()),
    plvTargetMachine(X_InitTargetMachine()),
    lvDataLayout(X_InitDataLayout()),
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
    ptrVoidNull(llvm::ConstantPointerNull::get(tyVoidPtr)),
    x_vScanner(*this, false),
    x_vParser(*this, x_vScanner)
{}

Driver::~Driver() {}

int Driver::Parse() {
    if (x_vParser.parse())
        return 1;
    
    ast::eval::ClassNameVisitor visClassName(*this);
    x_upProg->AcceptVisitor(visClassName);
    if (visClassName.IsRejected())
        return 2;

    ast::eval::InheritVisitor visInherit(*this);
    x_upProg->AcceptVisitor(visInherit);
    if (visInherit.IsRejected())
        return 3;

    ast::eval::FieldVisitor visField(*this);
    x_upProg->AcceptVisitor(visField);
    if (visField.IsRejected())
        return 4;

    ast::eval::TypeVisitor visType(*this);
    x_upProg->AcceptVisitor(visType);
    if (visType.IsRejected())
        return 5;
    
    cg::ClassGenVisitor visClassGen(*this);
    x_upProg->AcceptVisitor(visClassGen);
    if (visClassGen.IsRejected())
        return 6;

    cg::FnGenVisitor visFnGen(*this);
    x_upProg->AcceptVisitor(visFnGen);
    if (visFnGen.IsRejected())
        return 7;

    cg::CodeGenVisitor visCodeGen(*this);
    x_upProg->AcceptVisitor(visCodeGen);
    if (visCodeGen.IsRejected())
        return 8;

    x_upProg->GetLvMod()->print(llvm::outs(), nullptr);

    return 0;
}

void Driver::PrintSymbols() {
    x_vScanner.SetPrint(true);
    while (x_vScanner.NextToken().token() != TokenId::kEnd);
    x_vScanner.SetPrint(false);
}

void Driver::SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept {
    x_upProg = std::move(upProg);
}

}

#include <filesystem>
#include <fstream>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>

#include "Driver.hpp"
#include "Parser.hpp"
#include "Print.hpp"
#include "Scanner.hpp"
#include "ast/Program.hpp"
#include "ast/eval/ClassNameVisitor.hpp"
#include "ast/eval/InheritVisitor.hpp"
#include "ast/eval/FieldVisitor.hpp"
#include "ast/eval/TypeVisitor.hpp"
#include "cg/CGContext.hpp"
#include "cg/ClassGenVisitor.hpp"
#include "cg/CodeGenVisitor.hpp"
#include "cg/FnGenVisitor.hpp"

namespace udc {

Driver::Driver(const Options &vOpts) noexcept : Options(vOpts) {}

Driver::~Driver() {}

int Driver::Run(const std::filesystem::path &paInput) {
    if (!(uFlags & (kSymbol | kAST | kLLVM | kAssembly | kCompile)))
        return 0;
    
    std::ifstream isInput(paInput);
    if (!isInput.is_open())
        throw std::runtime_error("Failed to open input file");

    Scanner vScanner(isInput);
    if (uFlags & kSymbol)
        vScanner.SetPrint(true);

    if (!(uFlags & (kAST | kLLVM | kAssembly | kCompile))) {
        while (vScanner.NextToken().token() != Parser::token::kEnd);
        return 0;
    }

    Parser vParser(*this, vScanner, paInput, paInput.string());
    if (vParser.parse())
        return 1;

    if (uFlags & kAST)
        std::cout << *x_upProg << std::endl;
    
    if (!(uFlags & (kLLVM | kAssembly | kCompile)))
        return 0;

    ast::eval::ClassNameVisitor visClassName;
    x_upProg->AcceptVisitor(visClassName);
    if (visClassName.IsRejected())
        return 2;

    ast::eval::InheritVisitor visInherit;
    x_upProg->AcceptVisitor(visInherit);
    if (visInherit.IsRejected())
        return 3;

    ast::eval::FieldVisitor visField;
    x_upProg->AcceptVisitor(visField);
    if (visField.IsRejected())
        return 4;

    ast::eval::TypeVisitor visType;
    x_upProg->AcceptVisitor(visType);
    if (visType.IsRejected())
        return 5;
    
    if (!x_upCgCtx)
        x_upCgCtx = std::make_unique<cg::CGContext>();

    cg::ClassGenVisitor visClassGen(*x_upCgCtx);
    x_upProg->AcceptVisitor(visClassGen);
    if (visClassGen.IsRejected())
        return 6;

    cg::FnGenVisitor visFnGen(*x_upCgCtx);
    x_upProg->AcceptVisitor(visFnGen);
    if (visFnGen.IsRejected())
        return 7;

    cg::CodeGenVisitor visCodeGen(*x_upCgCtx, uFlags & kOptimiz);
    x_upProg->AcceptVisitor(visCodeGen);
    if (visCodeGen.IsRejected())
        return 8;

    if (uFlags & kLLVM) {
        auto pa = paInput;
        pa.replace_extension(".ll");
        std::error_code ec;
        llvm::raw_fd_ostream os(pa.string(), ec, llvm::sys::fs::F_None);
        x_upProg->GetLvMod()->print(os, nullptr);
    }
    
    if (uFlags & kAssembly) {
        auto pa = paInput;
        pa.replace_extension(".s");
        std::error_code ec;
        llvm::raw_fd_ostream os(pa.string(), ec, llvm::sys::fs::F_None);
        llvm::legacy::PassManager pm;
        if (x_upCgCtx->uplvTargetMachine->addPassesToEmitFile(
            pm, os, llvm::TargetMachine::CGFT_AssemblyFile
        )) {
            std::cerr << "error: The target machine cannot emit assembly file" << std::endl;
            return 9;
        }
        pm.run(*x_upProg->GetLvMod());
    }

    if (uFlags & kCompile) {
        auto pa = paInput;
        pa.replace_extension(".o");
        std::error_code ec;
        llvm::raw_fd_ostream os(pa.string(), ec, llvm::sys::fs::F_None);
        llvm::legacy::PassManager pm;
        if (x_upCgCtx->uplvTargetMachine->addPassesToEmitFile(
            pm, os, llvm::TargetMachine::CGFT_ObjectFile
        )) {
            std::cerr << "error: The target machine cannot emit object file" << std::endl;
            return 10;
        }
        pm.run(*x_upProg->GetLvMod());
    }
    
    return 0;
}

void Driver::SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept {
    x_upProg = std::move(upProg);
}

}

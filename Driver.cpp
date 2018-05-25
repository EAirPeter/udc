#include <filesystem>
#include <fstream>

#include "Driver.hpp"
#include "Parser.hpp"
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
    if (!(uFlags & (kSymbol | kAST | kAssembly | kCompile)))
        return 0;
    
    std::ifstream isInput(paInput);
    if (!isInput.is_open())
        throw std::runtime_error("Failed to open input file");

    Scanner vScanner(isInput);
    if (uFlags & kSymbol)
        vScanner.SetPrint(true);

    if (!(uFlags & (kAST | kAssembly | kCompile))) {
        while (vScanner.NextToken().token() != Parser::token::kEnd);
        return 0;
    }

    Parser vParser(*this, vScanner);
    if (vParser.parse())
        return 1;

    if (uFlags & kAST)
        std::cout << *x_upProg << std::endl;
    
    if (!(uFlags & (kAssembly | kCompile)))
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

    cg::CodeGenVisitor visCodeGen(*x_upCgCtx);
    x_upProg->AcceptVisitor(visCodeGen);
    if (visCodeGen.IsRejected())
        return 8;

    x_upProg->GetLvMod()->print(llvm::outs(), nullptr);

    return 0;
}

void Driver::SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept {
    x_upProg = std::move(upProg);
}

}

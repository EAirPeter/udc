#include "Driver.hpp"
#include "ast/eval/ClassNameVisitor.hpp"
#include "ast/eval/InheritVisitor.hpp"
#include "ast/eval/FieldVisitor.hpp"
#include "ast/eval/TypeVisitor.hpp"

namespace udc {

Driver::Driver() : x_vScanner(*this, false), x_vParser(*this, x_vScanner) {}

int Driver::Parse() {
    if (x_vParser.parse())
        return 1;
    auto &vProg = x_oProg->get();
    if (x_oProg)
        std::cout << *x_oProg << std::endl;
    
    ast::eval::ClassNameVisitor visClassName(*this);
    vProg.AcceptVisitor(visClassName);
    if (visClassName.IsRejected())
        return 2;

    ast::eval::InheritVisitor visInherit(*this);
    vProg.AcceptVisitor(visInherit);
    if (visInherit.IsRejected())
        return 3;

    ast::eval::FieldVisitor visStatic(*this);
    vProg.AcceptVisitor(visStatic);
    if (visInherit.IsRejected())
        return 4;

    ast::eval::TypeVisitor visType(*this);
    vProg.AcceptVisitor(visType);
    if (visType.IsRejected())
        return 5;

    return 0;
}

void Driver::PrintSymbols() {
    x_vScanner.SetPrint(true);
    while (x_vScanner.NextToken().token() != TokenId::kEnd);
    x_vScanner.SetPrint(false);
}

void Driver::SetProgram(ast::Program &vProg) noexcept {
    x_oProg = vProg;
}

}

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
    if (x_upProg)
        std::cout << *x_upProg << std::endl;
    
    ast::eval::ClassNameVisitor visClassName(*this);
    x_upProg->AcceptVisitor(visClassName);
    if (visClassName.IsRejected())
        return 2;

    ast::eval::InheritVisitor visInherit(*this);
    x_upProg->AcceptVisitor(visInherit);
    if (visInherit.IsRejected())
        return 3;

    ast::eval::FieldVisitor visStatic(*this);
    x_upProg->AcceptVisitor(visStatic);
    if (visInherit.IsRejected())
        return 4;

    ast::eval::TypeVisitor visType(*this);
    x_upProg->AcceptVisitor(visType);
    if (visType.IsRejected())
        return 5;

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

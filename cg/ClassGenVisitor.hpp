#ifndef UDC_CG_CLASS_GEN_VISITOR_HPP_
#define UDC_CG_CLASS_GEN_VISITOR_HPP_

#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Module;
}

namespace udc::cg {

using namespace udc::ast;
using namespace udc::ast::eval;

class ClassGenVisitor : public VisitorBase {
public:
    ClassGenVisitor(Driver &drv) noexcept;
    virtual inline ~ClassGenVisitor() = default;

public:
    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;

private:
    llvm::Module *x_plvMod = nullptr;
    ast::eval::TypeRegistry *x_pTyReg = nullptr;
};

}

#endif

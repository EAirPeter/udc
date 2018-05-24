#ifndef UDC_CG_FN_GEN_VISITOR_HPP_
#define UDC_CG_FN_GEN_VISITOR_HPP_

#include "../ast/Fwd.hpp"
#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Module;
}

namespace udc {
class Driver;
}

namespace udc::cg {

using namespace udc::ast;
using namespace udc::ast::eval;

class FnGenVisitor : public VisitorBase {
public:
    FnGenVisitor(Driver &drv) noexcept;
    virtual inline ~FnGenVisitor() = default;

public:
    virtual void Visit(Program &vProg) noexcept override;

    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;

private:
    llvm::Module *x_plvMod = nullptr;
    ClassDef *x_pClass = nullptr;
    llvm::GlobalVariable *x_plvClassIdx = nullptr;
};

}

#endif

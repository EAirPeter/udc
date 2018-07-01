#ifndef UDC_CG_FN_GEN_VISITOR_HPP_
#define UDC_CG_FN_GEN_VISITOR_HPP_

#include "../ast/Fwd.hpp"
#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Module;
}

namespace udc::cg {
class CGContext;

using namespace udc::ast;

class FnGenVisitor final : public eval::VisitorBase {
public:
    FnGenVisitor(CGContext &ctx) noexcept;
    virtual inline ~FnGenVisitor() = default;

public:
    virtual void Visit(Program &vProg) noexcept override;

    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;

private:
    CGContext &x_ctx;
    llvm::Module *x_plvMod = nullptr;
    ClassDef *x_pClass = nullptr;
    llvm::Constant *x_plvClassIdx = nullptr;
};

}

#endif

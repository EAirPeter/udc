#ifndef UDC_CG_CLASS_GEN_VISITOR_HPP_
#define UDC_CG_CLASS_GEN_VISITOR_HPP_

#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Module;
}

namespace udc::cg {
class CGContext;

using namespace udc::ast;

class ClassGenVisitor final : public eval::VisitorBase {
public:
    ClassGenVisitor(CGContext &ctx) noexcept;
    virtual inline ~ClassGenVisitor() = default;

public:
    virtual void Visit(Program &vProg) noexcept override;
    virtual void Visit(ClassDef &vClass) noexcept override;

private:
    CGContext &x_ctx;
    llvm::Module *x_plvMod = nullptr;
    eval::TypeRegistry *x_pTyReg = nullptr;
};

}

#endif

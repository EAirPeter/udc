#ifndef UDC_AST_WHILE_STMT_HPP_
#define UDC_AST_WHILE_STMT_HPP_

#include <memory>

#include "NodeBase.hpp"

namespace udc::ast {

class WhileStmt : public NodeBase {
public:
    WhileStmt(
        const location &loc,
        std::unique_ptr<ExprBase> &&upCond,
        std::unique_ptr<NodeBase> &&upBody
    ) noexcept;
    virtual ~WhileStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetCond() const noexcept {
        return x_upCond.get();
    }

    inline NodeBase *GetBody() const noexcept {
        return x_upBody.get();
    }

private:
    std::unique_ptr<ExprBase> x_upCond;
    std::unique_ptr<NodeBase> x_upBody;
};

}

#endif

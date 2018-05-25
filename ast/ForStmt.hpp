#ifndef UDC_AST_FOR_STMT_HPP_
#define UDC_AST_FOR_STMT_HPP_

#include <memory>

#include "NodeBase.hpp"

namespace udc::ast {

class ForStmt : public NodeBase {
public:
    ForStmt(
        const location &loc,
        std::unique_ptr<ExprBase> &&upInit,
        std::unique_ptr<ExprBase> &&upCond,
        std::unique_ptr<ExprBase> &&upIncr,
        std::unique_ptr<NodeBase> &&upBody
    ) noexcept;
    virtual ~ForStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetInit() const noexcept {
        return x_upInit.get();
    }

    inline ExprBase *GetCond() const noexcept {
        return x_upCond.get();
    }

    inline ExprBase *GetIncr() const noexcept {
        return x_upIncr.get();
    }

    inline NodeBase *GetBody() const noexcept {
        return x_upBody.get();
    }

private:
    std::unique_ptr<ExprBase> x_upInit;
    std::unique_ptr<ExprBase> x_upCond;
    std::unique_ptr<ExprBase> x_upIncr;
    std::unique_ptr<NodeBase> x_upBody;

};

}

#endif

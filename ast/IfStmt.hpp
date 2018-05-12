#ifndef UDC_AST_IF_STMT_HPP_
#define UDC_AST_IF_STMT_HPP_

#include <memory>

#include "NodeBase.hpp"

namespace udc::ast {

class IfStmt : public NodeBase {
public:
    IfStmt(
        const Location &vLocation,
        std::unique_ptr<ExprBase> &&upCond,
        std::unique_ptr<NodeBase> &&upThen,
        std::unique_ptr<NodeBase> &&upElse
    ) noexcept;
    virtual ~IfStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetCond() const noexcept {
        return x_upCond.get();
    }

    inline NodeBase *GetThen() const noexcept {
        return x_upThen.get();
    }

    inline NodeBase *GetElse() const noexcept {
        return x_upElse.get();
    }

private:
    std::unique_ptr<ExprBase> x_upCond;
    std::unique_ptr<NodeBase> x_upThen;
    std::unique_ptr<NodeBase> x_upElse;
};

}

#endif

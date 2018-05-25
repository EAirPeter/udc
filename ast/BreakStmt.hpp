#ifndef UDC_AST_BREAK_STMT_HPP_
#define UDC_AST_BREAK_STMT_HPP_

#include "NodeBase.hpp"

namespace udc::ast {

class BreakStmt : public NodeBase {
public:
    BreakStmt(const location &loc) noexcept;
    virtual ~BreakStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

};

}

#endif

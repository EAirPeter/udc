#ifndef UDC_AST_PRINT_STMT_HPP_
#define UDC_AST_PRINT_STMT_HPP_

#include <memory>
#include <vector>

#include "NodeBase.hpp"

namespace udc::ast {

class PrintStmt final : public NodeBase {
public:
    PrintStmt(const location &loc, std::vector<std::unique_ptr<ExprBase>> &&vecArgs) noexcept;
    virtual ~PrintStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr const std::vector<std::unique_ptr<ExprBase>> &GetArgs() const noexcept {
        return x_vecArgs;
    }

private:
    std::vector<std::unique_ptr<ExprBase>> x_vecArgs;
};

}

#endif

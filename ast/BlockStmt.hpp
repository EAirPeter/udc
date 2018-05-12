#ifndef UDC_AST_BLOCK_STMT_HPP_
#define UDC_AST_BLOCK_STMT_HPP_

#include <memory>
#include <vector>

#include "NodeBase.hpp"

namespace udc::ast {

class BlockStmt : public NodeBase {
public:
    BlockStmt(const Location &vLocation, std::vector<std::unique_ptr<NodeBase>> &&vecItems) noexcept;
    virtual ~BlockStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr const std::vector<std::unique_ptr<NodeBase>> &GetItems() const noexcept {
        return x_vecItems;
    }

private:
    std::vector<std::unique_ptr<NodeBase>> x_vecItems;
};

}

#endif

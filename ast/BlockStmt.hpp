#ifndef UDC_AST_BLOCK_STMT_HPP_
#define UDC_AST_BLOCK_STMT_HPP_

#include <memory>
#include <vector>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class BlockStmt : public Base, public IStatement {
public:
    BlockStmt(const Location &vLocation, std::vector<std::unique_ptr<IBlockItem>> &&vecItems) noexcept;
    virtual ~BlockStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::vector<std::unique_ptr<IBlockItem>> x_vecItems;
};

}

#endif

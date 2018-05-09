#ifndef UDC_AST_PRINT_STMT_HPP_
#define UDC_AST_PRINT_STMT_HPP_

#include <vector>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class PrintStmt : public Base, public IStatement {
public:
    PrintStmt(const Location &vLocation, std::vector<std::unique_ptr<IExpression>> &&vecArgs) noexcept;
    virtual ~PrintStmt();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::vector<std::unique_ptr<IExpression>> x_vecArgs;
};

}

#endif

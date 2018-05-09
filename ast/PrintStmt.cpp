#include "../IndentHelper.hpp"
#include "PrintStmt.hpp"

namespace udc::ast {

PrintStmt::PrintStmt(const Location &vLocation, std::vector<std::unique_ptr<IExpression>> &&vecArgs) noexcept :
    Base(vLocation), x_vecArgs(std::move(vecArgs))
{}

PrintStmt::~PrintStmt() {}

void PrintStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "Print(";
    bool bFirst = true;
    for (auto &&upArg : x_vecArgs) {
        if (bFirst)
            bFirst = false;
        else
            os << ", ";
        os << *upArg;
    }
    os << ')';
}

}

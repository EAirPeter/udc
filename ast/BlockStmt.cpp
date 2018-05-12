#include "BlockStmt.hpp"

namespace udc::ast {

BlockStmt::BlockStmt(const Location &vLocation, std::vector<std::unique_ptr<NodeBase>> &&vecItems) noexcept :
    NodeBase(vLocation), x_vecItems(std::move(vecItems))
{}

BlockStmt::~BlockStmt() {}

void BlockStmt::Print(std::ostream &os, std::uint32_t cIndent) const {
    bool bFirst = true;
    for (auto &&upItem : x_vecItems) {
        if (bFirst)
            bFirst = false;
        else
            os << std::endl;
        upItem->Print(os, cIndent);
    }
}

}

#ifndef UDC_AST_NODE_BASE_HPP_
#define UDC_AST_NODE_BASE_HPP_

#include "eval/VisitorBase.hpp"

namespace udc::ast {

class NodeBase {
public:
    NodeBase(const location &loc) noexcept;
    NodeBase(const NodeBase &) = delete;
    NodeBase(NodeBase &&) = delete;
    virtual inline ~NodeBase();

    NodeBase &operator =(const NodeBase &) = delete;
    NodeBase &operator =(NodeBase &&) = delete;

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const = 0;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept = 0;

public:
    inline const location &GetLocation() const noexcept {
        return *x_upLoc.get();
    }

private:
    const std::unique_ptr<const location> x_upLoc;
};

inline std::ostream &operator <<(std::ostream &os, const NodeBase &ast) {
    ast.Print(os, 0);
    return os;
}

}

#endif

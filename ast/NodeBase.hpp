#ifndef UDC_AST_NODE_BASE_HPP_
#define UDC_AST_NODE_BASE_HPP_

#include "../Location.hpp"
#include "eval/VisitorBase.hpp"

namespace udc::ast {

class NodeBase {
public:
    inline NodeBase(const Location &vLocation) noexcept : x_vLocation(vLocation) {}
    NodeBase(const NodeBase &) = delete;
    NodeBase(NodeBase &&) = delete;
    virtual inline ~NodeBase() = default;

    NodeBase &operator =(const NodeBase &) = delete;
    NodeBase &operator =(NodeBase &&) = delete;

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const = 0;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept {
        vis.Visit(*this);
    }

public:
    constexpr const Location &GetLocation() const noexcept {
        return x_vLocation;
    }

private:
    const Location x_vLocation;
};

inline std::ostream &operator <<(std::ostream &os, const NodeBase &ast) {
    ast.Print(os, 0);
    return os;
}

}

#endif

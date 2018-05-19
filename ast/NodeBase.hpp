#ifndef UDC_AST_NODE_BASE_HPP_
#define UDC_AST_NODE_BASE_HPP_

#include "../Location.hpp"
#include "eval/VisitorBase.hpp"

namespace udc {
class Driver;
}

namespace udc::ast {

class NodeBase {
public:
    inline NodeBase(Driver &drv, const Location &vLoc) noexcept : y_drv(drv), x_vLocation(vLoc) {}
    NodeBase(const NodeBase &) = delete;
    NodeBase(NodeBase &&) = delete;
    virtual inline ~NodeBase() = default;

    NodeBase &operator =(const NodeBase &) = delete;
    NodeBase &operator =(NodeBase &&) = delete;

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const = 0;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept = 0;

public:
    constexpr const Location &GetLocation() const noexcept {
        return x_vLocation;
    }

protected:
    Driver &y_drv;

private:
    const Location x_vLocation;
};

inline std::ostream &operator <<(std::ostream &os, const NodeBase &ast) {
    ast.Print(os, 0);
    return os;
}

}

#endif

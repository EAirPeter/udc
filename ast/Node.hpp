#ifndef UDC_AST_NODE_HPP_
#define UDC_AST_NODE_HPP_

#include "../Parser.hpp"

#include <ostream>

namespace udc::ast {

class Node {
public:
    Node(const Location &vLocation) noexcept;
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    virtual ~Node();

    Node &operator =(const Node &) = delete;
    Node &operator =(Node &&) = delete;

public:
    inline Location GetLocation() const noexcept {
        return x_vLocation;
    }

    void Print(std::ostream &os, uint32_t cIndent) const;

protected:
    virtual void Y_Print(std::ostream &os, uint32_t cIndent) const = 0;

private:
    const Location x_vLocation;
};

inline std::ostream &operator <<(std::ostream &os, const Node &ast) {
    ast.Print(os, 0);
    return os;
}

}

#endif

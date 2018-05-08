#include "Node.hpp"

namespace udc::ast {

Node::Node(const Location &vLocation) noexcept :
    x_vLocation {vLocation}
{}

Node::~Node() {}

void Node::Print(std::ostream &os, uint32_t cIndent) const {
    while (cIndent--)
        os << "  ";
    Y_Print(os, cIndent);
}

}

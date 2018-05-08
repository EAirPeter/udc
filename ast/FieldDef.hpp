#ifndef UDC_AST_FIELD_DEF_HPP_
#define UDC_AST_FIELD_DEF_HPP_

#include "Node.hpp"

namespace udc::ast {

class FieldDef : public virtual Node {
public:
    using Node::Node;
    virtual ~FieldDef();
};

}

#endif

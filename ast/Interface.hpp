#ifndef UDC_AST_INTERFACE_HPP_
#define UDC_AST_INTERFACE_HPP_

#ifdef _MSC_VER
// cross-delegation
#pragma warning(disable: 4250)
#endif

#include <cstdint>
#include <iosfwd>

#include "../Location.hpp"
#include "Fwd.hpp"

namespace udc::ast {

struct INode {
    virtual ~INode() = default;
    virtual Location GetLocation() const noexcept = 0;
    virtual void Print(std::ostream &os, std::uint32_t cIndent) const = 0;
};

struct IType : virtual INode {
    virtual ~IType() = default;
    virtual std::string GetName() const noexcept = 0;
};

struct IDefinition : virtual INode {
    virtual ~IDefinition() = default;
};

struct IBlockItem : virtual INode {
    virtual ~IBlockItem() = default;
};

struct IExpression : virtual INode {
    virtual ~IExpression() = default;
};

struct IStatement : virtual IBlockItem {
    virtual ~IStatement() = default;
};

struct ILiteral : virtual IExpression {
    virtual ~ILiteral() = default;
};

struct ILValue : virtual IExpression {
    virtual ~ILValue() = default;
};

inline std::ostream &operator <<(std::ostream &os, const INode &ast) {
    ast.Print(os, 0);
    return os;
}

}

#endif

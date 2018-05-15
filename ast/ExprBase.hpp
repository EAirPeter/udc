#ifndef UDC_AST_EXPR_BASE_HPP_
#define UDC_AST_EXPR_BASE_HPP_

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class ExprBase : public NodeBase {
public:
    inline ExprBase(const Location &vLocation, bool bLvalue = false) : NodeBase(vLocation), x_bLvalue(bLvalue) {}
    virtual inline ~ExprBase() = default;

    constexpr const eval::Type &GetType() const noexcept {
        return x_vType;
    }

    constexpr void SetType(const eval::Type &vType) noexcept {
        x_vType = vType;
    }

    constexpr bool IsLvalue() const noexcept {
        return x_bLvalue;
    }

private:
    eval::Type x_vType;
    bool x_bLvalue;
};

}

#endif

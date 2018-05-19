#ifndef UDC_AST_EXPR_BASE_HPP_
#define UDC_AST_EXPR_BASE_HPP_

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class ExprBase : public NodeBase {
public:
    inline ExprBase(Driver &drv, const Location &loc, bool bLvalue = false) noexcept :
        NodeBase(drv, loc), x_bLvalue(bLvalue)
    {}
    virtual inline ~ExprBase() = default;

    constexpr const eval::Type &GetType() const noexcept {
        return *x_pty;
    }

    constexpr void SetType(const eval::Type &ty) noexcept {
        x_pty = &ty;
    }

    constexpr bool IsLvalue() const noexcept {
        return x_bLvalue;
    }

private:
    const eval::Type *x_pty = nullptr;
    bool x_bLvalue;
};

}

#endif

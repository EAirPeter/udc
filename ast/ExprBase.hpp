#ifndef UDC_AST_EXPR_BASE_HPP_
#define UDC_AST_EXPR_BASE_HPP_

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class ExprBase : public NodeBase {
public:
    inline ExprBase(const location &loc, bool bLvalue = false) noexcept :
        NodeBase(loc), x_bLvalue(bLvalue)
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

    constexpr bool IsGenPtr() const noexcept {
        return x_bGenPtr;
    }

    constexpr void SetGenPtr(bool bGenPtr) noexcept {
        x_bGenPtr = bGenPtr;
    }

private:
    const eval::Type *x_pty = nullptr;
    bool x_bGenPtr = false;
    bool x_bLvalue;
};

}

#endif

#ifndef UDC_AST_EXPR_BASE_HPP_
#define UDC_AST_EXPR_BASE_HPP_

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class ExprBase : public NodeBase {
public:
    using NodeBase::NodeBase;
    virtual inline ~ExprBase() = default;

    constexpr const eval::Type &GetType() const noexcept {
        return x_vType;
    }

    constexpr void SetType(const eval::Type &vType) noexcept {
        x_vType = vType;
    }

private:
    eval::Type x_vType;
};

}

#endif

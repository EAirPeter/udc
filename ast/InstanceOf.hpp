#ifndef UDC_AST_INSTANCE_OF_HPP_
#define UDC_AST_INSTANCE_OF_HPP_

#include <memory>

#include "ExprBase.hpp"

namespace udc::ast {

class InstanceOf : public ExprBase {
public:
    InstanceOf(const Location &vLocation, std::unique_ptr<ExprBase> &&upExpr, std::string &&sName) noexcept;
    virtual ~InstanceOf();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline ExprBase *GetExpr() const noexcept {
        return x_upExpr.get();
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr const ClassDef &GetClass() const noexcept {
        return *x_pClass;
    }

    constexpr void SetClass(const ClassDef &vClass) noexcept {
        x_pClass = &vClass;
    }

private:
    std::unique_ptr<ExprBase> x_upExpr;
    std::string x_sName;
    const ClassDef *x_pClass;
};

}

#endif

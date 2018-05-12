#ifndef UDC_AST_STR_LIT_HPP_
#define UDC_AST_STR_LIT_HPP_

#include "ExprBase.hpp"

namespace udc::ast {

class StrLit : public ExprBase {
public:
    StrLit(const Location &vLocation, std::string &&sVal) noexcept;
    virtual ~StrLit();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr const std::string &GetVal() const noexcept {
        return x_sVal;
    }

private:
    std::string x_sVal;
};

}

#endif

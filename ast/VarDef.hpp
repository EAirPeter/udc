#ifndef UDC_AST_VAR_DEF_HPP_
#define UDC_AST_VAR_DEF_HPP_

#include <memory>

#include "NodeBase.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class VarDef : public NodeBase {
public:
    VarDef(const Location &vLocation, std::unique_ptr<TypeName> &&upType, std::string &&sName) noexcept;
    virtual ~VarDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    inline TypeName *GetTypeName() const noexcept {
        return x_upType.get();
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr const eval::Type &GetType() const noexcept {
        return x_vType;
    }

    constexpr void SetType(const eval::Type &vType) noexcept {
        x_vType = vType;
    }

private:
    std::unique_ptr<TypeName> x_upType;
    std::string x_sName;
    eval::Type x_vType;
};

}

#endif

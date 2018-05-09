#ifndef UDC_AST_VAR_DEF_HPP_
#define UDC_AST_VAR_DEF_HPP_

#include <memory>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class VarDef : public Base, public virtual IBlockItem, public virtual IDefinition {
public:
    VarDef(const Location &vLocation, std::unique_ptr<IType> &&upType, std::string &&sName) noexcept;
    virtual ~VarDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::unique_ptr<IType> x_upType;
    std::string x_sName;
};

}

#endif

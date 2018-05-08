#ifndef UDC_AST_VAR_DEF_HPP_
#define UDC_AST_VAR_DEF_HPP_

#include "FieldDef.hpp"

namespace udc::ast {

class Type;

class VarDef : public virtual FieldDef {
public:
    VarDef(const Location &vLocation, std::unique_ptr<Type> &&upType, std::string &&sName) noexcept;
    virtual ~VarDef();
protected:
    virtual void Y_Print(std::ostream &os, uint32_t cIndent) const override;
private:
    std::unique_ptr<Type> x_upType;
    std::string x_sName;
};

}

#endif

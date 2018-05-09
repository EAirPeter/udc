#ifndef UDC_AST_FN_DEF_HPP_
#define UDC_AST_FN_DEF_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class FnDef : public Base, public virtual IDefinition {
public:
    FnDef(
        const Location &vLocation,
        bool bStatic,
        std::unique_ptr<IType> &&upType,
        std::string &&sName,
        std::vector<std::unique_ptr<VarDef>> &&vecPars,
        std::unique_ptr<BlockStmt> &&upBody
    ) noexcept;
    virtual ~FnDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    bool x_bStatic;
    std::unique_ptr<IType> x_upType;
    std::string x_sName;
    std::vector<std::unique_ptr<VarDef>> x_vecPars;
    std::unique_ptr<BlockStmt> x_upBody;
};

}

#endif

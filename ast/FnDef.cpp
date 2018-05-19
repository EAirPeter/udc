#include "../IndentHelper.hpp"
#include "BlockStmt.hpp"
#include "FnDef.hpp"
#include "TypeName.hpp"
#include "VarDef.hpp"

namespace udc::ast {

FnDef::FnDef(
    const Location &vLocation,
    bool bStatic,
    std::unique_ptr<TypeName> &&upType,
    std::string &&sName,
    std::vector<std::unique_ptr<VarDef>> &&vecPars,
    std::unique_ptr<BlockStmt> &&upBody
) noexcept :
    NodeBase(vLocation),
    x_bStatic(bStatic),
    x_upType(std::move(upType)),
    x_sName(std::move(sName)),
    x_vecPars(std::move(vecPars)),
    x_upBody(std::move(upBody)),
    x_stVar(false),
    x_idx(~std::size_t {})
{}

FnDef::~FnDef() {}

void FnDef::Print(std::ostream &os, std::uint32_t cIndent) const {
    if (IsStatic())
        os << Indent(cIndent) << "static " << *x_upType;
    else
        x_upType->Print(os, cIndent);
    os << ' ' << x_sName << '(';
    bool bFirst = true;
    for (auto &&upVar : x_vecPars) {
        if (bFirst)
            bFirst = false;
        else
            os << ", ";
        os << *upVar;
    }
    os << ')' << std::endl;
    x_upBody->Print(os, cIndent + 1);
}

}

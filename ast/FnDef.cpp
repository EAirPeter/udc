#include "BlockStmt.hpp"
#include "FnDef.hpp"
#include "VarDef.hpp"

namespace udc::ast {

FnDef::FnDef(
    const Location &vLocation,
    bool bStatic,
    std::unique_ptr<IType> &&upType,
    std::string &&sName,
    std::vector<std::unique_ptr<VarDef>> &&vecPars,
    std::unique_ptr<BlockStmt> &&upBody
) noexcept :
    Base(vLocation),
    x_bStatic(bStatic),
    x_sName(std::move(sName)),
    x_vecPars(std::move(vecPars)),
    x_upBody(std::move(upBody))
{}

FnDef::~FnDef() {}

void FnDef::Print(std::ostream &os, std::uint32_t cIndent) const {
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

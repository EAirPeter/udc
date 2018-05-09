#include "../IndentHelper.hpp"
#include "ClassDef.hpp"

namespace udc::ast {

ClassDef::ClassDef(
    const Location &vLocation,
    std::string &&sName,
    std::optional<std::string> &&soBase,
    std::vector<std::unique_ptr<IDefinition>> &&vecFields
) noexcept :
    Base(vLocation),
    x_sName(std::move(sName)),
    x_soBase(std::move(soBase)),
    x_vecFields(std::move(vecFields))
{}

ClassDef::~ClassDef() {}

void ClassDef::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << "class " << x_sName;
    if (x_soBase)
        os << " extends " << *x_soBase;
    for (auto &&upField : x_vecFields) {
        os << std::endl;
        upField->Print(os, cIndent + 1);
    }
}

}

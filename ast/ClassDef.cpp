#include "ClassDef.hpp"

namespace udc::ast {

ClassDef::ClassDef(
    const Location &vLocation,
    std::string &&sName,
    std::optional<std::string> &&soBase,
    std::vector<std::unique_ptr<FieldDef>> &&vecFields
) noexcept :
    Node(vLocation),
    x_sName(std::move(sName),
    x_soBase(std::move(soBase)),
    x_vecFields(std::move(vecFields))
{}

ClassDef::~ClassDef() {}

void ClassDef::Y_Print(std::ostream &os, uint32_t cIndent) const {
    os << "class " << x_sName;
    if (x_soBase)
        os << " extends" << *x_soBase;
    os << std::endl;
    for (auto &&upField : x_vecFields) {
        upField->Print(os, cIndent + 1);
        os << std::endl;
    }
}

}

#include "../IndentHelper.hpp"
#include "TypeName.hpp"

namespace udc::ast {

TypeName::TypeName(Driver &drv, const Location &loc, TypeId vId) noexcept :
    NodeBase(drv, loc), x_vId(vId), x_sName(), x_cDimension(0)
{}

TypeName::TypeName(Driver &drv, const Location &loc, std::string &&sName) noexcept :
    NodeBase(drv, loc), x_vId(TypeId::kClass), x_sName(std::move(sName)), x_cDimension(0)
{}

TypeName::~TypeName() {}

namespace {

constexpr const char *TypeNameNtmbs(TypeId vId) {
    constexpr const char *aName[] {
        "void", "int", "bool", "string", "class"
    };
    return aName[(int) vId];
}

}

void TypeName::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << TypeNameNtmbs(x_vId);
    if (x_vId == TypeId::kClass)
        os << ' ' << x_sName;
    for (auto i = x_cDimension; i; --i)
        os << "[]";
}

}

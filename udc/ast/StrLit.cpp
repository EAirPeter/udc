#include "../IndentHelper.hpp"
#include "StrLit.hpp"

namespace udc::ast {

StrLit::StrLit(const location &loc, std::string &&sVal) noexcept :
    ExprBase(loc), x_sVal(std::move(sVal))
{}

StrLit::~StrLit() {}

void StrLit::Print(std::ostream &os, std::uint32_t cIndent) const {
    os << Indent(cIndent) << '\"';
    for (auto ch : x_sVal) {
        switch (ch) {
        case '\"':
            os << "\\\"";
            break;
        case '\\':
            os << "\\\\";
            break;
        case '\t':
            os << "\\t";
            break;
        case '\n':
            os << "\\n";
            break;
        default:
            os << ch;
            break;
        }
    }
    os << '\"';
}

}

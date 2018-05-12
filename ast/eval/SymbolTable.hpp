#ifndef UDC_AST_EVAL_SYMBOL_TABLE_HPP_
#define UDC_AST_EVAL_SYMBOL_TABLE_HPP_

#include <unordered_map>

#include "../Fwd.hpp"

namespace udc::ast::eval {

namespace _impl_SymbolTable {

template<class tSymbol>
class SymbolTable {
public:
    inline SymbolTable(bool bOverridable) noexcept : x_bOverridable(bOverridable), x_pParent(nullptr) {}

    constexpr void SetParent(const SymbolTable &st) {
        x_pParent = &st;
    }

    tSymbol *Lookup(const std::string &sName) const noexcept {
        auto it = x_map.find(sName);
        return it != x_map.end() ? it->second.get() : x_pParent ? x_pParent->Lookup(sName) : nullptr;
    }

    bool Add(const std::string &sName, tSymbol *pSymbol) noexcept {
        if (x_pParent && !x_pParent->x_bOverridable) {
            if (x_pParent->Lookup(sName))
                return false;
        }
        auto [it, res] = x_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(sName),
            std::forward_as_tuple(pSymbol)
        );
        (void) it;
        return res;
    }

private:
    bool x_bOverridable;
    const SymbolTable *x_pParent;
    std::unordered_map<std::string, tSymbol *> x_map;
};

}

using ClassTable = _impl_SymbolTable::SymbolTable<ClassDef>;
using VarTable = _impl_SymbolTable::SymbolTable<VarDef>;
using FnTable = _impl_SymbolTable::SymbolTable<FnDef>;

}

#endif

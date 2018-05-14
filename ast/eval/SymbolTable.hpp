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

    inline tSymbol *LookupUntilOverridable(const std::string &sName) const noexcept {
        if (x_bOverridable)
            return nullptr;
        auto it = x_map.find(sName);
        return it != x_map.end() ? it->second : nullptr;
    }

    inline tSymbol *LookupInParent(const std::string &sName) const noexcept {
        return x_pParent ? x_pParent->Lookup(sName) : nullptr;
    }

    inline tSymbol *Lookup(const std::string &sName) const noexcept {
        auto it = x_map.find(sName);
        return it != x_map.end() ? it->second : LookupInParent(sName);
    }
    
    tSymbol *AddNoOverride(const std::string &sName, tSymbol *pSymbol) noexcept {
        auto pPrevious = LookupInParent(sName);
        if (pPrevious)
            return pPrevious;
        auto [it, res] = x_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(sName),
            std::forward_as_tuple(pSymbol)
        );
        return res ? nullptr : it->second;
    }

    tSymbol *Add(const std::string &sName, tSymbol *pSymbol) noexcept {
        auto pPrevious = x_pParent ? x_pParent->LookupUntilOverridable(sName) : nullptr;
        if (pPrevious)
            return pPrevious;
        auto [it, res] = x_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(sName),
            std::forward_as_tuple(pSymbol)
        );
        return res ? nullptr : it->second;
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

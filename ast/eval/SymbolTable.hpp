#ifndef UDC_AST_EVAL_SYMBOL_TABLE_HPP_
#define UDC_AST_EVAL_SYMBOL_TABLE_HPP_

#include <unordered_map>

#include "../Fwd.hpp"

namespace udc::ast::eval {

template<class tSymPtr, tSymPtr kNull>
class SymbolTable {
public:
    inline SymbolTable(bool bOverridable) noexcept : x_bOverridable(bOverridable), x_pParent(nullptr) {}

    constexpr void SetParent(const SymbolTable &st) noexcept {
        x_pParent = &st;
    }

    inline tSymPtr LookupUntilOverridable(const std::string &sName) const noexcept {
        if (x_bOverridable)
            return kNull;
        auto it = x_map.find(sName);
        return it != x_map.end() ? it->second : kNull;
    }

    inline tSymPtr LookupInParent(const std::string &sName) const noexcept {
        return x_pParent ? x_pParent->Lookup(sName) : kNull;
    }

    inline tSymPtr Lookup(const std::string &sName) const noexcept {
        auto it = x_map.find(sName);
        return it != x_map.end() ? it->second : LookupInParent(sName);
    }
    
    tSymPtr AddNoOverride(const std::string &sName, tSymPtr pSymbol) noexcept {
        auto pPrevious = LookupInParent(sName);
        if (pPrevious != kNull)
            return pPrevious;
        auto [it, res] = x_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(sName),
            std::forward_as_tuple(pSymbol)
        );
        return res ? kNull : it->second;
    }

    tSymPtr Add(const std::string &sName, tSymPtr pSymbol) noexcept {
        auto pPrevious = x_pParent ? x_pParent->LookupUntilOverridable(sName) : kNull;
        if (pPrevious != kNull)
            return pPrevious;
        auto [it, res] = x_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(sName),
            std::forward_as_tuple(pSymbol)
        );
        return res ? kNull : it->second;
    }

private:
    bool x_bOverridable;
    const SymbolTable *x_pParent;
    std::unordered_map<std::string, tSymPtr> x_map;
};

using ClassTable = SymbolTable<ClassDef *, nullptr>;
using FnTable = SymbolTable<FnDef *, nullptr>;
using VarTable = SymbolTable<VarDef *, nullptr>;

}

#endif

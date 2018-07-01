#ifndef UDC_AST_EVAL_VIRT_FN_TABLE_HPP_
#define UDC_AST_EVAL_VIRT_FN_TABLE_HPP_

#include "Fwd.hpp"
#include "SymbolTable.hpp"

namespace udc::ast::eval {

class VfTable {
public:
    inline void SetParent(const VfTable &vft) {
        x_vec = vft.x_vec;
        x_st.SetParent(vft.x_st);
    }

    inline FnDef *LookupInParent(const std::string &sName) const noexcept {
        auto idx = x_st.LookupInParent(sName);
        return ~idx ? x_vec[idx] : nullptr;
    }

    inline FnDef *Lookup(const std::string &sName) const noexcept {
        auto idx = x_st.Lookup(sName);
        return ~idx ? x_vec[idx] : nullptr;
    }

    inline std::size_t IndexOf(const std::string &sName) const noexcept {
        return x_st.Lookup(sName);
    }
    
    inline FnDef *At(std::size_t idx) const noexcept {
        return ~idx ? x_vec[idx] : nullptr;
    }

    inline FnDef *Add(const std::string &sName, FnDef *pSymbol) noexcept {
        auto idx = x_st.LookupInParent(sName);
        if (!~idx) {
            idx = x_st.Add(sName, x_vec.size());
            if (~idx)
                return x_vec[idx];
        }
        if (~idx)
            x_vec[idx] = pSymbol;
        else
            x_vec.emplace_back(pSymbol);
        return nullptr;
    }

    constexpr const std::vector<FnDef *> &GetVec() const noexcept {
        return x_vec;
    }

private:
    std::vector<FnDef *> x_vec;
    SymbolTable<std::size_t, ~std::size_t {}> x_st {true};
};

}

#endif

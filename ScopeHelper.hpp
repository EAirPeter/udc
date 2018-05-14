#ifndef UDC_SCOPE_HELPER_HPP_
#define UDC_SCOPE_HELPER_HPP_

#include <utility>

#include "Common.hpp"

namespace udc {

template<class tVal>
class ScopeHelper {
public:
    constexpr ScopeHelper(tVal &vVal, tVal &&vOld) noexcept : x_vVal(vVal), x_vOld(vOld) {}
    ScopeHelper(const ScopeHelper &) = delete;
    ScopeHelper(ScopeHelper &&) = delete;
    inline ~ScopeHelper() {
        x_vVal = std::move(x_vOld);
    }

private:
    tVal &x_vVal;
    tVal x_vOld;
};

}

#define ENTER_SCOPE(val_, new_) ScopeHelper UNI_NAME (val_, std::exchange(val_, new_))

#endif

#ifndef UDC_AST_EVAL_TYPE_REGISTRY_HPP_
#define UDC_AST_EVAL_TYPE_REGISTRY_HPP_

#include <unordered_map>

#include "../../Common.hpp"
#include "Type.hpp"

namespace udc {
class Driver;
}

namespace udc::ast::eval {

class TypeRegistry {
public:
    TypeRegistry(Driver &drv) noexcept;
    const Type &Put(const INonArrayType &vType) noexcept;
    const Type &Get(const INonArrayType &vType, std::size_t cDim = 0) noexcept;
    const Type &DecDim(const Type &ty) const noexcept;
    const Type &IncDim(const Type &ty) noexcept;
    void MakeArrays() noexcept;

private:
    Driver &x_drv;
    std::unordered_map<const INonArrayType *, Type> x_maps[kDimMax + 1];
public:
    const Type &tyNull;
    const Type &tyVoid;
    const Type &tyInt;
    const Type &tyBool;
    const Type &tyString;
};

}

#endif

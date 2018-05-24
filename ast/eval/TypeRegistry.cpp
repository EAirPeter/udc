#include <cassert>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

#include "TypeRegistry.hpp"

namespace udc::ast::eval {

TypeRegistry::TypeRegistry(Driver &drv) noexcept :
    x_drv(drv),
    tyNull(Put(NullType::vInstance)),
    tyVoid(Put(VoidType::vInstance)),
    tyInt(Put(IntType::vInstance)),
    tyBool(Put(BoolType::vInstance)),
    tyString(Put(StringType::vInstance))
{}

const Type &TypeRegistry::Put(const INonArrayType &vType) noexcept {
    auto [it, res] = x_maps[0].emplace(
        std::piecewise_construct,
        std::forward_as_tuple(&vType),
        std::forward_as_tuple(vType, 0)
    );
    assert(res);
    return it->second;
}

const Type &TypeRegistry::Get(const INonArrayType &vType, std::size_t cDim) noexcept {
    assert(cDim <= kDimMax);
    auto it = x_maps[cDim].find(&vType);
    if (it != x_maps[cDim].end())
        return it->second;
    auto &ty = x_maps[cDim].emplace(
        std::piecewise_construct,
        std::forward_as_tuple(&vType),
        std::forward_as_tuple(vType, cDim)
    ).first->second;
    if (cDim) {
        it = x_maps[--cDim].find(&vType);
        while (it == x_maps[cDim].end()) {
            assert(cDim);
            x_maps[cDim].emplace(
                std::piecewise_construct,
                std::forward_as_tuple(&vType),
                std::forward_as_tuple(vType, cDim)
            );
            it = x_maps[--cDim].find(&vType);
        }
    }
    return ty;
}

const Type &TypeRegistry::DecDim(const Type &ty) const noexcept {
    assert(ty.GetDimension() && ty.GetDimension() <= kDimMax);
    return x_maps[ty.GetDimension() - 1].at(&ty.GetElemType());
}

const Type &TypeRegistry::IncDim(const Type &ty) noexcept {
    auto &vType = ty.GetElemType();
    auto cDim = ty.GetDimension();
    assert(cDim < kDimMax && x_maps[cDim].find(&vType) != x_maps[cDim].end());
    return x_maps[cDim + 1].emplace(
        std::piecewise_construct,
        std::forward_as_tuple(&vType),
        std::forward_as_tuple(vType, cDim)
    ).first->second;
}

void TypeRegistry::MakeLvTypes() noexcept {
    for (std::size_t i = 1; i <= kDimMax; ++i)
        for (auto &[_, ty] : x_maps[i])
            ty.SetLvType(DecDim(ty).GetLvType()->getPointerTo());
}

}

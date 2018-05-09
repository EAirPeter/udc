#include "Base.hpp"

namespace udc::ast {

Base::Base(const Location &vLocation) noexcept :
    x_vLocation {vLocation}
{}

Base::~Base() {}

Location Base::GetLocation() const noexcept {
    return x_vLocation;
}

}

#include "../Location.hpp"
#include "NodeBase.hpp"

namespace udc::ast {

NodeBase::NodeBase(const location &loc) noexcept :
    x_upLoc(std::make_unique<const location>(loc))
{}

NodeBase::~NodeBase() {}

}

#include "Type.hpp"

namespace udc::ast::eval {

namespace _impl_BuiltinType {

const NullType NullType::vInstance {};
const VoidType VoidType::vInstance {};
const IntType IntType::vInstance {};
const BoolType BoolType::vInstance {};
const StringType StringType::vInstance {};

}

}

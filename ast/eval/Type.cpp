#include "Type.hpp"

namespace udc::ast::eval {

namespace _impl_BuiltinType {

/*
template class BuiltinType<BuiltinTypeId::kNull>;
template class BuiltinType<BuiltinTypeId::kVoid>;
template class BuiltinType<BuiltinTypeId::kInt>;
template class BuiltinType<BuiltinTypeId::kBool>;
template class BuiltinType<BuiltinTypeId::kString>;
*/

const NullType NullType::vInstance {};
const VoidType VoidType::vInstance {};
const IntType IntType::vInstance {};
const BoolType BoolType::vInstance {};
const StringType StringType::vInstance {};

}

}

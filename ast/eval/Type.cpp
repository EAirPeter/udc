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

const Type Type::tyNull(NullType::vInstance);
const Type Type::tyVoid(VoidType::vInstance);
const Type Type::tyInt(IntType::vInstance);
const Type Type::tyBool(BoolType::vInstance);
const Type Type::tyString(StringType::vInstance);

}

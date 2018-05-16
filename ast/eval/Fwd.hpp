#ifndef UDC_AST_EVAL_FWD_HPP_
#define UDC_AST_EVAL_FWD_HPP_

#include "../Fwd.hpp"

namespace udc::ast::eval {

class VisitorBase;

struct INonArrayType;
class Type;

namespace _impl_BuiltinType {
enum class BuiltinTypeId {
    kNull,
    kVoid,
    kInt,
    kBool,
    kString,
};
template<BuiltinTypeId vId>
class BuiltinType;
}

using NullType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kNull>;
using VoidType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kVoid>;
using IntType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kInt>;
using BoolType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kBool>;
using StringType = _impl_BuiltinType::BuiltinType<_impl_BuiltinType::BuiltinTypeId::kString>;

template<class tSymPtr, tSymPtr kNull>
class SymbolTable;

using ClassTable = SymbolTable<ClassDef *, nullptr>;
using FnTable = SymbolTable<FnDef *, nullptr>;
using VarTable = SymbolTable<VarDef *, nullptr>;

class VfTable;

}

#endif

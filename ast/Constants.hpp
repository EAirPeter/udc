#ifndef UDC_AST_CONSTANTS_HPP_
#define UDC_AST_CONSTANTS_HPP_

namespace udc::ast {

enum class TypeId {
    kVoid,
    kInt,
    kBool,
    kClass
    kArray,
};

enum class OpId {
    kAsg,
    kIor,
    kAnd,
    kEqu,
    kNeq,
    kLes,
    kGre,
    kLeq,
    kGeq,
    kAdd,
    kSub,
    kMul,
    kDiv,
    kMod,
    kNeg,
    kNot,
};

}

#endif

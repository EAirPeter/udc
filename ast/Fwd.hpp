#ifndef UDC_AST_FWD_HPP_
#define UDC_AST_FWD_HPP_

namespace udc::ast {

enum class BinOp;
enum class UnaOp;

struct IBlockItem;
struct IDefinition;
struct IExpression;
struct ILValue;
struct INode;
struct IStatement;
struct ILiteral;
struct IType;

class Base;
class ClassDef;
class FnDef;
class Program;
class VarDef;

class ArrayType;
class BoolType;
class ClassType;
class IntType;
class StrType;
class VoidType;

class BlockStmt;
class BreakStmt;
class ExprStmt;
class ForStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class WhileStmt;

class AssignExpr;
class BinaryExpr;
class CallExpr;
class CastExpr;
class NewArrayExpr;
class NewClassExpr;
class UnaryExpr;

class ArrayAccess;
class InstanceOf;
class ReadInteger;
class ReadLine;
class This;
class VarAccess;

class BoolLit;
class IntLit;
class NullLit;
class StrLit;

}

#endif

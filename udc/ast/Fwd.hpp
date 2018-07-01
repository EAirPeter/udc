#ifndef UDC_AST_FWD_HPP_
#define UDC_AST_FWD_HPP_

namespace udc::ast {

enum class TypeId;
enum class BinOp;
enum class UnaOp;

class ExprBase;
class NodeBase;

class ClassDef;
class FnDef;
class Program;
class TypeName;
class VarDef;

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

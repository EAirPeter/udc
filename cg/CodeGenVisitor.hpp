#ifndef UDC_CG_CODE_GEN_VISITOR_HPP_
#define UDC_CG_CODE_GEN_VISITOR_HPP_

#include <unordered_map>

#include <llvm/IR/IRBuilder.h>

#include "../ast/Fwd.hpp"
#include "../ast/eval/VisitorBase.hpp"

namespace llvm {
class Argument;
class BasicBlock;
class Module;
class Value;
}

namespace udc {
class Driver;
}

namespace udc::cg {

using namespace udc::ast;
using namespace udc::ast::eval;

class CodeGenVisitor : public VisitorBase {
public:
    CodeGenVisitor(Driver &drv) noexcept;
    virtual inline ~CodeGenVisitor() = default;

public:
    virtual void Visit(Program &vProg) noexcept override;

    virtual void Visit(ClassDef &vClass) noexcept override;
    virtual void Visit(FnDef &vFn) noexcept override;
    virtual void Visit(VarDef &vVar) noexcept override;
    
    virtual void Visit(BlockStmt &stmt) noexcept override;
    virtual void Visit(BreakStmt &stmt) noexcept override;
    virtual void Visit(ExprStmt &stmt) noexcept override;
    virtual void Visit(ForStmt &stmt) noexcept override;
    virtual void Visit(IfStmt &stmt) noexcept override;
    virtual void Visit(PrintStmt &stmt) noexcept override;
    virtual void Visit(ReturnStmt &stmt) noexcept override;
    virtual void Visit(WhileStmt &stmt) noexcept override;
    
    virtual void Visit(AssignExpr &expr) noexcept override;
    virtual void Visit(BinaryExpr &expr) noexcept override;
    virtual void Visit(CallExpr &expr) noexcept override;
    virtual void Visit(CastExpr &expr) noexcept override;
    virtual void Visit(NewArrayExpr &expr) noexcept override;
    virtual void Visit(NewClassExpr &expr) noexcept override;
    virtual void Visit(UnaryExpr &expr) noexcept override;

    virtual void Visit(ArrayAccess &expr) noexcept override;
    virtual void Visit(InstanceOf &expr) noexcept override;
    virtual void Visit(ReadInteger &expr) noexcept override;
    virtual void Visit(ReadLine &expr) noexcept override;
    virtual void Visit(This &expr) noexcept override;
    virtual void Visit(VarAccess &expr) noexcept override;

    virtual void Visit(BoolLit &expr) noexcept override;
    virtual void Visit(IntLit &expr) noexcept override;
    virtual void Visit(NullLit &expr) noexcept override;
    virtual void Visit(StrLit &expr) noexcept override;
    
private:
    llvm::Constant *X_EmitStringLiteral(const char *psz) noexcept;

private:
    llvm::IRBuilder<> x_lvBld {y_drv.lvCtx};
    llvm::Function *x_plvRtlPrintf = nullptr;
    llvm::Function *x_plvRtlAlloc = nullptr;
    llvm::Function *x_plvRtlReAlloc = nullptr;
    llvm::Function *x_plvRtlAllocArray = nullptr;
    llvm::Function *x_plvRtlInstanceOf = nullptr;
    llvm::Function *x_plvRtlCheckCast = nullptr;
    llvm::Function *x_plvRtlCheckBound = nullptr;
    llvm::Function *x_plvRtlReadInteger = nullptr;
    llvm::Function *x_plvRtlReadLine = nullptr;
    llvm::Function *x_plvRtlStrCmp = nullptr;
    TypeRegistry *x_pTyReg = nullptr;
    llvm::Module *x_plvMod = nullptr;
    llvm::Constant *x_plvClassIdx = nullptr;
    llvm::Constant *x_plvBoolStr = nullptr;
    ClassDef *x_pClass = nullptr;
    llvm::Argument *x_plvThis = nullptr;
    llvm::BasicBlock *x_plvPost = nullptr;
    llvm::Value *x_plvRet = nullptr;
    std::unordered_map<std::string, llvm::Constant *> x_mapStrLits {};
    unsigned x_uForStmtCnt = 0;
    unsigned x_uIfStmtCnt = 0;
    unsigned x_uWhileStmtCnt = 0;
};

}

#endif

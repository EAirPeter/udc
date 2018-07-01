#ifndef UDC_AST_EVAL_VISITOR_BASE_HPP_
#define UDC_AST_EVAL_VISITOR_BASE_HPP_

#include <string>

#include "Fwd.hpp"

namespace udc {
class location;
}

namespace udc::ast::eval {

class VisitorBase {
public:
    VisitorBase() noexcept = default;
    VisitorBase(const VisitorBase &) = delete;
    VisitorBase(VisitorBase &&) = delete;
    virtual ~VisitorBase() = default;

    constexpr bool IsRejected() const noexcept {
        return x_bRejected;
    }

protected:
    constexpr void Y_Reject() noexcept {
        x_bRejected = true;
    }

public:
    virtual inline void Visit(Program &) noexcept {}

    virtual inline void Visit(ClassDef &) noexcept {}
    virtual inline void Visit(FnDef &) noexcept {}
    virtual inline void Visit(VarDef &) noexcept {}

    virtual inline void Visit(BlockStmt &) noexcept {}
    virtual inline void Visit(BreakStmt &) noexcept {}
    virtual inline void Visit(ExprStmt &) noexcept {}
    virtual inline void Visit(ForStmt &) noexcept {}
    virtual inline void Visit(IfStmt &) noexcept {}
    virtual inline void Visit(PrintStmt &) noexcept {}
    virtual inline void Visit(ReturnStmt &) noexcept {}
    virtual inline void Visit(WhileStmt &) noexcept {}

    virtual inline void Visit(AssignExpr &) noexcept {}
    virtual inline void Visit(BinaryExpr &) noexcept {}
    virtual inline void Visit(CallExpr &) noexcept {}
    virtual inline void Visit(CastExpr &) noexcept {}
    virtual inline void Visit(NewArrayExpr &) noexcept {}
    virtual inline void Visit(NewClassExpr &) noexcept {}
    virtual inline void Visit(UnaryExpr &) noexcept {}

    virtual inline void Visit(ArrayAccess &) noexcept {}
    virtual inline void Visit(InstanceOf &) noexcept {}
    virtual inline void Visit(ReadInteger &) noexcept {}
    virtual inline void Visit(ReadLine &) noexcept {}
    virtual inline void Visit(This &) noexcept {}
    virtual inline void Visit(VarAccess &) noexcept {}

    virtual inline void Visit(BoolLit &) noexcept {}
    virtual inline void Visit(IntLit &) noexcept {}
    virtual inline void Visit(NullLit &) noexcept {}
    virtual inline void Visit(StrLit &) noexcept {}

    virtual inline void Visit(TypeName &) noexcept {}

protected:
    void Y_RjLocalShadow(const location &loc, const std::string &sName, const location &vPrevious) noexcept;
    void Y_RjDimTooLarge(const location &loc) noexcept;
    void Y_RjNonStaticVar(const location &loc, const std::string &sName) noexcept;
    void Y_RjNonStaticCall(const location &loc, const std::string &sName) noexcept;
    void Y_RjThisInStatic(const location &loc, const std::string &sName) noexcept;
    void Y_RjOverride(const location &loc, const std::string &sName, const location &vPrevious) noexcept;
    void Y_RjInheritCycle(const location &loc) noexcept;
    void Y_RjVoidArray(const location &loc) noexcept;
    void Y_RjNotFound(const location &loc, const char *pszWhat, const std::string &sName) noexcept;
    void Y_RjTypeMissMatch(const location &loc, const Type &tyExpected, const Type &tyUnexpected) noexcept;
    void Y_RjNoConversion(const location &loc, const Type &tyTo, const Type &tyFrom) noexcept;
    void Y_RjNoComparison(const location &loc, const Type &tyLhs, const Type &tyRhs) noexcept;
    void Y_RjReturnForVoid(const location &loc, const std::string &sName) noexcept;
    void Y_RjNoReturnVal(const location &loc, const Type &tyExpected, const std::string &sName) noexcept;
    void Y_RjRedefinition(
        const location &loc, const char *pszWhat, 
        const std::string &sName, const location &vPrevious
    ) noexcept;
    void Y_RjIllegalBreak(const location &loc) noexcept;
    void Y_RjNotPrintable(const location &loc, const Type &ty) noexcept;
    void Y_RjAssignNonLval(const location &loc) noexcept;
    void Y_RjNotWhat(const location &loc, const char *pszWhat, const Type &ty) noexcept;
    void Y_RjArgNumber(
        const location &loc, const std::string &sName,
        std::size_t cExpected, std::size_t cUnexpected
    ) noexcept;
    void Y_RjArgType(
        const location &loc, const std::string &sName,
        std::size_t idx, const std::string &sArg,
        const Type &tyExpected, const Type &tyUnexpected
    ) noexcept;

private:
    bool x_bRejected = false;
};

}

#endif

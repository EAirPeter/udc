#include <cassert>

#include "../../ScopeHelper.hpp"
#include "../All.hpp"
#include "TypeVisitor.hpp"

namespace udc::ast::eval {

void TypeVisitor::Visit(Program &vProg) noexcept {
    ENTER_SCOPE(x_pTyReg, &vProg.GetTyReg());
    ENTER_SCOPE(x_pstClass, &vProg.GetClassTable());
    for (auto &upClass : vProg.GetClasses())
        upClass->AcceptVisitor(*this);
}

void TypeVisitor::Visit(ClassDef &vClass) noexcept {
    ENTER_SCOPE(x_pstFn, &vClass.GetFnTable());
    ENTER_SCOPE(x_pstVar, &vClass.GetVarTable());
    ENTER_SCOPE(x_pstVf, &vClass.GetVfTable());
    ENTER_SCOPE(x_pClass, &vClass);
    for (auto &upField : vClass.GetFields())
        if (auto pFn = dynamic_cast<FnDef *>(upField.get()))
            pFn->AcceptVisitor(*this);
}

void TypeVisitor::Visit(FnDef &vFn) noexcept {
    vFn.GetVarTable().SetParent(*x_pstVar);
    ENTER_SCOPE(x_pstVar, &vFn.GetVarTable());
    ENTER_SCOPE(x_pFn, &vFn);
    vFn.GetBody()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(VarDef &vVar) noexcept {
    auto &sName = vVar.GetName();
    auto pPrevious = x_pstVar->Add(sName, &vVar);
    if (pPrevious) {
        Y_RjRedefinition(vVar.GetLocation(), "variable", sName, pPrevious->GetLocation());
        return;
    }
    vVar.GetTypeName()->AcceptVisitor(*this);
    vVar.SetType(*x_pty);
}

void TypeVisitor::Visit(BlockStmt &stmt) noexcept {
    stmt.GetVarTable().SetParent(*x_pstVar);
    ENTER_SCOPE(x_pstVar, &stmt.GetVarTable());
    for (auto &upItem : stmt.GetItems())
        upItem->AcceptVisitor(*this);
}

void TypeVisitor::Visit(BreakStmt &stmt) noexcept {
    if (!x_pLoop) {
        Y_RjIllegalBreak(stmt.GetLocation());
        return;
    }
}

void TypeVisitor::Visit(ExprStmt &stmt) noexcept {
    if (stmt.GetExpr())
        stmt.GetExpr()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(ForStmt &stmt) noexcept {
    if (stmt.GetInit())
        stmt.GetInit()->AcceptVisitor(*this);
    if (stmt.GetCond()) {
        stmt.GetCond()->AcceptVisitor(*this);
        if (stmt.GetCond()->GetType() != x_pTyReg->tyBool) {
            Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), x_pTyReg->tyBool, stmt.GetCond()->GetType());
            return;
        }
    }
    if (stmt.GetIncr())
        stmt.GetIncr()->AcceptVisitor(*this);
    ENTER_SCOPE(x_pLoop, &stmt);
    stmt.GetBody()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(IfStmt &stmt) noexcept {
    stmt.GetCond()->AcceptVisitor(*this);
    if (stmt.GetCond()->GetType() != x_pTyReg->tyBool) {
        Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), x_pTyReg->tyBool, stmt.GetCond()->GetType());
        return;
    }
    stmt.GetThen()->AcceptVisitor(*this);
    if (stmt.GetElse())
        stmt.GetElse()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(PrintStmt &stmt) noexcept {
    for (auto &upArg : stmt.GetArgs()) {
        upArg->AcceptVisitor(*this);
        auto &ty = upArg->GetType();
        if (ty != x_pTyReg->tyInt && ty != x_pTyReg->tyBool && ty != x_pTyReg->tyString) {
            Y_RjNotPrintable(upArg->GetLocation(), upArg->GetType());
            return;
        }
    }
}

void TypeVisitor::Visit(ReturnStmt &stmt) noexcept {
    assert(x_pFn);
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        if (x_pFn->GetType() == x_pTyReg->tyVoid) {
            Y_RjReturnForVoid(stmt.GetExpr()->GetLocation(), x_pFn->GetName());
            return;
        }
        if (!x_pFn->GetType().Accepts(stmt.GetExpr()->GetType())) {
            Y_RjNoConversion(stmt.GetExpr()->GetLocation(), x_pFn->GetType(), stmt.GetExpr()->GetType());
            return;
        }
    }
    else {
        if (x_pFn->GetType() != x_pTyReg->tyVoid) {
            Y_RjNoReturnVal(stmt.GetLocation(), x_pFn->GetType(), x_pFn->GetName());
            return;
        }
    }
}

void TypeVisitor::Visit(WhileStmt &stmt) noexcept {
    stmt.GetCond()->AcceptVisitor(*this);
    if (stmt.GetCond()->GetType() != x_pTyReg->tyBool) {
        Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), x_pTyReg->tyBool, stmt.GetCond()->GetType());
        return;
    }
    ENTER_SCOPE(x_pLoop, &stmt);
    stmt.GetBody()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(AssignExpr &expr) noexcept {
    expr.GetLhs()->AcceptVisitor(*this);
    expr.GetRhs()->AcceptVisitor(*this);
    auto &tyLhs = expr.GetLhs()->GetType();
    auto &tyRhs = expr.GetRhs()->GetType();
    expr.SetType(tyLhs);
    if (!expr.GetLhs()->IsLvalue()) {
        Y_RjAssignNonLval(expr.GetLhs()->GetLocation());
        return;
    }
    if (!tyLhs.Accepts(tyRhs)) {
        Y_RjNoConversion(expr.GetLocation(), tyLhs, tyRhs);
        return;
    }
}

void TypeVisitor::Visit(BinaryExpr &expr) noexcept {
    expr.GetLhs()->AcceptVisitor(*this);
    expr.GetRhs()->AcceptVisitor(*this);
    auto &tyLhs = expr.GetLhs()->GetType();
    auto &tyRhs = expr.GetRhs()->GetType();
    switch (expr.GetOp()) {
    case BinOp::kIor:
        expr.SetType(x_pTyReg->tyBool);
    case BinOp::kAnd:
        if (tyLhs != x_pTyReg->tyBool) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyBool, tyLhs);
            return;
        }
        if (tyRhs != x_pTyReg->tyBool) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyBool, tyRhs);
            return;
        }
        break;
    case BinOp::kEqu:
    case BinOp::kNeq:
        expr.SetType(x_pTyReg->tyBool);
        if (!tyLhs.Accepts(tyRhs) && !tyRhs.Accepts(tyLhs)) {
            Y_RjNoComparison(expr.GetLocation(), tyLhs, tyRhs);
            return;
        }
        break;
    case BinOp::kLes:
    case BinOp::kGre:
    case BinOp::kLeq:
    case BinOp::kGeq:
        expr.SetType(x_pTyReg->tyBool);
        if (tyLhs != x_pTyReg->tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyInt, tyLhs);
            return;
        }
        if (tyRhs != x_pTyReg->tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyInt, tyRhs);
            return;
        }
        break;
    case BinOp::kAdd:
    case BinOp::kSub:
    case BinOp::kMul:
    case BinOp::kDiv:
    case BinOp::kMod:
        expr.SetType(x_pTyReg->tyInt);
        if (tyLhs != x_pTyReg->tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyInt, tyLhs);
            return;
        }
        if (tyRhs != x_pTyReg->tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), x_pTyReg->tyInt, tyRhs);
            return;
        }
        break;
    }
}

void TypeVisitor::Visit(CallExpr &expr) noexcept {
    auto pstFn = (const FnTable *) x_pstFn;
    auto pstVf = (const VfTable *) x_pstVf;
    bool bArray = false;
    bool bStatic = false;
    if (expr.GetExpr()) {
        if (auto pVarAcc = dynamic_cast<VarAccess *>(expr.GetExpr()); pVarAcc && !pVarAcc->GetExpr()) {
            // check if it is a class name
            auto pClass = x_pstClass->Lookup(pVarAcc->GetName());
            if (pClass) {
                pstFn = &pClass->GetFnTable();
                pstVf = &pClass->GetVfTable();
                bStatic = true;
            }
        }
        if (!bStatic) {
            expr.GetExpr()->AcceptVisitor(*this);
            auto &tyExpr = expr.GetExpr()->GetType();
            if (tyExpr.IsArray())
                bArray = true;
            else {
                auto pClass = dynamic_cast<const ClassDef *>(&tyExpr.GetElemType());
                if (!pClass) {
                    Y_RjNotWhat(expr.GetExpr()->GetLocation(), "a class or array", tyExpr);
                    return;
                }
                pstFn = &pClass->GetFnTable();
                pstVf = &pClass->GetVfTable();
            }
        }
    }
    else {
        assert(x_pFn);
        bStatic = x_pFn->IsStatic();
    }
    if (bArray) {
        if (expr.GetName() != "length") {
            Y_RjNotFound(expr.GetLocation(), "function", expr.GetName());
            return;
        }
        expr.SetType(x_pTyReg->tyInt);
        if (expr.GetArgs().size()) {
            Y_RjArgNumber(expr.GetLocation(), "length", 0, expr.GetArgs().size());
            return;
        }
    }
    else {
        auto pFn = pstFn->Lookup(expr.GetName());
        if (!pFn)
            pFn = pstVf->Lookup(expr.GetName());
        if (!pFn) {
            Y_RjNotFound(expr.GetLocation(), "function", expr.GetName());
            return;
        }
        expr.SetType(pFn->GetType());
        expr.SetFn(*pFn);
        if (bStatic && !pFn->IsStatic()) {
            Y_RjNonStaticCall(expr.GetLocation(), pFn->GetName());
            return;
        }
        if (expr.GetArgs().size() != pFn->GetPars().size()) {
            Y_RjArgNumber(expr.GetLocation(), pFn->GetName(), pFn->GetPars().size(), expr.GetArgs().size());
            return;
        }
        std::size_t i = 0;
        for (auto &upArg : expr.GetArgs()) {
            auto &upPar = pFn->GetPars()[i];
            upArg->AcceptVisitor(*this);
            if (!upPar->GetType().Accepts(upArg->GetType())) {
                Y_RjArgType(
                    upArg->GetLocation(),
                    pFn->GetName(), i + 1, upPar->GetName(),
                    upPar->GetType(), upArg->GetType()
                );
                return;
            }
            ++i;
        }
    }
}

void TypeVisitor::Visit(CastExpr &expr) noexcept {
    auto pClass = x_pstClass->Lookup(expr.GetName());
    if (!pClass) {
        Y_RjNotFound(expr.GetLocation(), "class", expr.GetName());
        return;
    }
    expr.SetType(x_pTyReg->Get(*pClass));
    expr.GetExpr()->AcceptVisitor(*this);
    if (!expr.GetType().Accepts(expr.GetExpr()->GetType()) && !expr.GetExpr()->GetType().Accepts(expr.GetType())) {
        Y_RjNoConversion(expr.GetLocation(), expr.GetType(), expr.GetExpr()->GetType());
        return;
    }
}

void TypeVisitor::Visit(NewArrayExpr &expr) noexcept {
    expr.GetTypeName()->AcceptVisitor(*this);
    if (x_pty->GetElemType() == VoidType::vInstance) {
        Y_RjVoidArray(expr.GetLocation());
        return;
    }
    if (x_pty->GetDimension() >= kDimMax) {
        Y_RjDimTooLarge(expr.GetLocation());
        return;
    }
    expr.SetType(x_pTyReg->IncDim(*x_pty));
    expr.GetExpr()->AcceptVisitor(*this);
    if (expr.GetExpr()->GetType() != x_pTyReg->tyInt) {
        Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), x_pTyReg->tyInt, expr.GetExpr()->GetType());
        return;
    }
}

void TypeVisitor::Visit(NewClassExpr &expr) noexcept {
    auto pClass = x_pstClass->Lookup(expr.GetName());
    if (!pClass) {
        Y_RjNotFound(expr.GetLocation(), "class", expr.GetName());
        return;
    }
    expr.SetType(pClass->GetType());
}

void TypeVisitor::Visit(UnaryExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto &ty = expr.GetExpr()->GetType();
    switch (expr.GetOp()) {
    case UnaOp::kNeg:
        expr.SetType(x_pTyReg->tyInt);
        if (ty != x_pTyReg->tyInt) {
            Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), x_pTyReg->tyInt, ty);
            return;
        }
        break;
    case UnaOp::kNot:
        expr.SetType(x_pTyReg->tyBool);
        if (ty != x_pTyReg->tyBool) {
            Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), x_pTyReg->tyBool, ty);
            return;
        }
        break;
    }
}

void TypeVisitor::Visit(ArrayAccess &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto &tyExpr = expr.GetExpr()->GetType();
    if (!tyExpr.IsArray()) {
        Y_RjNotWhat(expr.GetExpr()->GetLocation(), "an array", tyExpr);
        return;
    }
    expr.SetType(x_pTyReg->DecDim(tyExpr));
    expr.GetSub()->AcceptVisitor(*this);
    auto &tySub = expr.GetSub()->GetType();
    if (tySub != x_pTyReg->tyInt) {
        Y_RjTypeMissMatch(expr.GetSub()->GetLocation(), x_pTyReg->tyInt, tySub);
        return;
    }
}

void TypeVisitor::Visit(InstanceOf &expr) noexcept {
    expr.SetType(x_pTyReg->tyBool);
    auto pClass = x_pstClass->Lookup(expr.GetName());
    if (!pClass) {
        Y_RjNotFound(expr.GetLocation(), "class", expr.GetName());
        return;
    }
    expr.SetClass(*pClass);
    expr.GetExpr()->AcceptVisitor(*this);
    auto &ty = expr.GetExpr()->GetType();
    if (ty.IsArray() || !dynamic_cast<const ClassDef *>(&ty.GetElemType())) {
        Y_RjNotWhat(expr.GetLocation(), "a class", ty);
        return;
    }
}

void TypeVisitor::Visit(ReadInteger &expr) noexcept {
    expr.SetType(x_pTyReg->tyInt);
}

void TypeVisitor::Visit(ReadLine &expr) noexcept {
    expr.SetType(x_pTyReg->tyString);
}

void TypeVisitor::Visit(This &expr) noexcept {
    assert(x_pClass);
    assert(x_pFn);
    expr.SetType(x_pClass->GetType());
    if (x_pFn->IsStatic()) {
        Y_RjThisInStatic(expr.GetLocation(), x_pFn->GetName());
        return;
    }
}

void TypeVisitor::Visit(VarAccess &expr) noexcept {
    assert(x_pFn);
    auto pstVar = (const VarTable *) x_pstVar;
    bool bStatic = false;
    if (expr.GetExpr()) {
        expr.GetExpr()->AcceptVisitor(*this);
        auto &tyExpr = expr.GetExpr()->GetType();
        auto pClass = tyExpr.IsArray() ? nullptr : dynamic_cast<const ClassDef *>(&tyExpr.GetElemType());
        if (!pClass) {
            Y_RjNotWhat(expr.GetExpr()->GetLocation(), "a class", tyExpr);
            return;
        }
        pstVar = &pClass->GetVarTable();
    }
    else if (x_pFn->IsStatic())
        bStatic = true;
    auto pVar = pstVar->Lookup(expr.GetName());
    if (!pVar) {
        Y_RjNotFound(expr.GetLocation(), "variable", expr.GetName());
        return;
    }
    expr.SetType(pVar->GetType());
    expr.SetVar(*pVar);
    if (bStatic && pVar->IsField()) {
        Y_RjNonStaticVar(expr.GetLocation(), expr.GetName());
        return;
    }
}

void TypeVisitor::Visit(BoolLit &expr) noexcept {
    expr.SetType(x_pTyReg->tyBool);
}

void TypeVisitor::Visit(IntLit &expr) noexcept {
    expr.SetType(x_pTyReg->tyInt);
}

void TypeVisitor::Visit(NullLit &expr) noexcept {
    expr.SetType(x_pTyReg->tyNull);
}

void TypeVisitor::Visit(StrLit &expr) noexcept {
    expr.SetType(x_pTyReg->tyString);
}

void TypeVisitor::Visit(TypeName &vTypeName) noexcept {
    if (vTypeName.GetDimension() > kDimMax) {
        Y_RjDimTooLarge(vTypeName.GetLocation());
        x_pty = &x_pTyReg->tyVoid;
        return;
    }
    switch (vTypeName.GetId()) {
    case TypeId::kVoid:
        if (vTypeName.GetDimension()) {
            Y_RjVoidArray(vTypeName.GetLocation());
            x_pty = &x_pTyReg->tyVoid;
            return;
        }
        x_pty = &x_pTyReg->tyVoid;
        break;
    case TypeId::kInt:
        x_pty = &x_pTyReg->Get(IntType::vInstance, vTypeName.GetDimension());
        break;
    case TypeId::kBool:
        x_pty = &x_pTyReg->Get(BoolType::vInstance, vTypeName.GetDimension());
        break;
    case TypeId::kString:
        x_pty = &x_pTyReg->Get(StringType::vInstance, vTypeName.GetDimension());
        break;
    case TypeId::kClass: {
        auto pClass = x_pstClass->Lookup(vTypeName.GetName());
        if (!pClass) {
            Y_RjNotFound(vTypeName.GetLocation(), "class", vTypeName.GetName());
            x_pty = &x_pTyReg->tyVoid;
            break;
        }
        x_pty = &x_pTyReg->Get(*pClass, vTypeName.GetDimension());
        break;
    }
    }
}

}

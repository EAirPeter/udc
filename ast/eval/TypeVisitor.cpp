#include <cassert>

#include "../../ScopeHelper.hpp"
#include "../All.hpp"
#include "TypeVisitor.hpp"

namespace udc::ast::eval {

void TypeVisitor::Visit(Program &vProg) noexcept {
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
    vVar.SetType(x_ty);
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
        if (stmt.GetCond()->GetType() != Type::tyBool) {
            Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), Type::tyBool, stmt.GetCond()->GetType());
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
    if (stmt.GetCond()->GetType() != Type::tyBool) {
        Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), Type::tyBool, stmt.GetCond()->GetType());
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
        if (ty != Type::tyInt && ty != Type::tyBool && ty != Type::tyString) {
            Y_RjNotPrintable(upArg->GetLocation(), upArg->GetType());
            return;
        }
    }
}

void TypeVisitor::Visit(ReturnStmt &stmt) noexcept {
    assert(x_pFn);
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        if (x_pFn->GetType() == Type::tyVoid) {
            Y_RjReturnForVoid(stmt.GetExpr()->GetLocation(), x_pFn->GetName());
            return;
        }
        if (!x_pFn->GetType().Accepts(stmt.GetExpr()->GetType())) {
            Y_RjNoConversion(stmt.GetExpr()->GetLocation(), x_pFn->GetType(), stmt.GetExpr()->GetType());
            return;
        }
    }
    else {
        if (x_pFn->GetType() != Type::tyVoid) {
            Y_RjNoReturnVal(stmt.GetLocation(), x_pFn->GetType(), x_pFn->GetName());
            return;
        }
    }
}

void TypeVisitor::Visit(WhileStmt &stmt) noexcept {
    stmt.GetCond()->AcceptVisitor(*this);
    if (stmt.GetCond()->GetType() != Type::tyBool) {
        Y_RjTypeMissMatch(stmt.GetCond()->GetLocation(), Type::tyBool, stmt.GetCond()->GetType());
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
        expr.SetType(Type::tyBool);
    case BinOp::kAnd:
        if (tyLhs != Type::tyBool) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyBool, tyLhs);
            return;
        }
        if (tyRhs != Type::tyBool) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyBool, tyRhs);
            return;
        }
        break;
    case BinOp::kEqu:
    case BinOp::kNeq:
        expr.SetType(Type::tyBool);
        if (!tyLhs.Accepts(tyRhs) && !tyRhs.Accepts(tyLhs)) {
            Y_RjNoComparison(expr.GetLocation(), tyLhs, tyRhs);
            return;
        }
        break;
    case BinOp::kLes:
    case BinOp::kGre:
    case BinOp::kLeq:
    case BinOp::kGeq:
        expr.SetType(Type::tyBool);
        if (tyLhs != Type::tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyInt, tyLhs);
            return;
        }
        if (tyRhs != Type::tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyInt, tyRhs);
            return;
        }
        break;
    case BinOp::kAdd:
    case BinOp::kSub:
    case BinOp::kMul:
    case BinOp::kDiv:
    case BinOp::kMod:
        expr.SetType(Type::tyInt);
        if (tyLhs != Type::tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyInt, tyLhs);
            return;
        }
        if (tyRhs != Type::tyInt) {
            Y_RjTypeMissMatch(expr.GetLhs()->GetLocation(), Type::tyInt, tyRhs);
            return;
        }
        break;
    }
}

void TypeVisitor::Visit(CallExpr &expr) noexcept {
    auto pstFn = (const FnTable *) x_pstFn;
    auto pstVf = (const VfTable *) x_pstVf;
    bool bArray = false;
    if (expr.GetExpr()) {
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
    if (bArray) {
        if (expr.GetName() != "length") {
            Y_RjNotFound(expr.GetLocation(), "function", expr.GetName());
            return;
        }
        expr.SetType(Type::tyInt);
        if (expr.GetArgs().size()) {
            Y_RjArgNumber(expr.GetLocation(), "length", 0, expr.GetArgs().size());
            return;
        }
    }
    else {
        auto pFn = pstFn->Lookup(expr.GetName());
        auto idx = pstVf->IndexOf(expr.GetName());
        assert(!pFn || !~idx);
        if (!pFn)
            pFn = pstVf->At(idx);
        if (!pFn) {
            Y_RjNotFound(expr.GetLocation(), "function", expr.GetName());
            return;
        }
        expr.SetType(pFn->GetType());
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
        expr.SetCallee(*pFn, idx);
    }
}

void TypeVisitor::Visit(CastExpr &expr) noexcept {
    auto pClass = x_pstClass->Lookup(expr.GetName());
    if (!pClass) {
        Y_RjNotFound(expr.GetLocation(), "class", expr.GetName());
        return;
    }
    expr.SetType({*pClass, 0});
    expr.GetExpr()->AcceptVisitor(*this);
    if (!expr.GetType().Accepts(expr.GetExpr()->GetType()) && !expr.GetExpr()->GetType().Accepts(expr.GetType())) {
        Y_RjNoConversion(expr.GetLocation(), expr.GetType(), expr.GetExpr()->GetType());
        return;
    }
}

void TypeVisitor::Visit(NewArrayExpr &expr) noexcept {
    expr.GetTypeName()->AcceptVisitor(*this);
    expr.SetType({x_ty.GetElemType(), x_ty.GetDimension() + 1});
    if (x_ty.GetElemType() == VoidType::vInstance) {
        Y_RjIllegalType(expr.GetLocation(), expr.GetType());
        expr.SetType(Type::tyVoid);
        return;
    }
    expr.GetExpr()->AcceptVisitor(*this);
    if (expr.GetExpr()->GetType() != Type::tyInt) {
        Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), Type::tyInt, expr.GetExpr()->GetType());
        return;
    }
}

void TypeVisitor::Visit(NewClassExpr &expr) noexcept {
    auto pClass = x_pstClass->Lookup(expr.GetName());
    if (!pClass) {
        Y_RjNotFound(expr.GetLocation(), "class", expr.GetName());
        return;
    }
    expr.SetType({*pClass, 0});
}

void TypeVisitor::Visit(UnaryExpr &expr) noexcept {
    expr.GetExpr()->AcceptVisitor(*this);
    auto &ty = expr.GetExpr()->GetType();
    switch (expr.GetOp()) {
    case UnaOp::kNeg:
        expr.SetType(Type::tyInt);
        if (ty != Type::tyInt) {
            Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), Type::tyInt, ty);
            return;
        }
        break;
    case UnaOp::kNot:
        expr.SetType(Type::tyBool);
        if (ty != Type::tyBool) {
            Y_RjTypeMissMatch(expr.GetExpr()->GetLocation(), Type::tyBool, ty);
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
    expr.SetType({tyExpr.GetElemType(), tyExpr.GetDimension() - 1});
    expr.GetSub()->AcceptVisitor(*this);
    auto &tySub = expr.GetSub()->GetType();
    if (tySub != Type::tyInt) {
        Y_RjTypeMissMatch(expr.GetSub()->GetLocation(), Type::tyInt, tySub);
        return;
    }
}

void TypeVisitor::Visit(InstanceOf &expr) noexcept {
    expr.SetType(Type::tyBool);
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
    expr.SetType(Type::tyInt);
}

void TypeVisitor::Visit(ReadLine &expr) noexcept {
    expr.SetType(Type::tyString);
}

void TypeVisitor::Visit(This &expr) noexcept {
    assert(x_pClass);
    assert(x_pFn);
    expr.SetType(*x_pClass);
    if (x_pFn->IsStatic()) {
        Y_RJThisInStatic(expr.GetLocation(), x_pFn->GetName());
        return;
    }
}

void TypeVisitor::Visit(VarAccess &expr) noexcept {
    auto pstVar = (const VarTable *) x_pstVar;
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
    auto pVar = pstVar->Lookup(expr.GetName());
    if (!pVar) {
        Y_RjNotFound(expr.GetLocation(), "variable", expr.GetName());
        return;
    }
    expr.SetType(pVar->GetType());
}

void TypeVisitor::Visit(BoolLit &expr) noexcept {
    expr.SetType(Type::tyBool);
}

void TypeVisitor::Visit(IntLit &expr) noexcept {
    expr.SetType(Type::tyInt);
}

void TypeVisitor::Visit(NullLit &expr) noexcept {
    expr.SetType(Type::tyNull);
}

void TypeVisitor::Visit(StrLit &expr) noexcept {
    expr.SetType(Type::tyString);
}

void TypeVisitor::Visit(TypeName &vTypeName) noexcept {
    switch (vTypeName.GetId()) {
    case TypeId::kVoid:
        if (vTypeName.GetDimension()) {
            Y_RjIllegalType(vTypeName.GetLocation(), {VoidType::vInstance, vTypeName.GetDimension()});
            x_ty = Type::tyVoid;
            return;
        }
        x_ty = Type::tyVoid;
        break;
    case TypeId::kInt:
        x_ty = {IntType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kBool:
        x_ty = {BoolType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kString:
        x_ty = {StringType::vInstance, vTypeName.GetDimension()};
        break;
    case TypeId::kClass: {
        auto pClass = x_pstClass->Lookup(vTypeName.GetName());
        if (!pClass) {
            Y_RjNotFound(vTypeName.GetLocation(), "class", vTypeName.GetName());
            x_ty = Type::tyVoid;
            break;
        }
        x_ty = {*pClass, vTypeName.GetDimension()};
        break;
    }
    }
}

}

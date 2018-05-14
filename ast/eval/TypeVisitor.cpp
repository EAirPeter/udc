#include "../../Driver.hpp"
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
    for (auto &upField : vClass.GetFields())
        if (auto pFn = dynamic_cast<FnDef *>(upField.get()))
            pFn->AcceptVisitor(*this);
}

void TypeVisitor::Visit(FnDef &vFn) noexcept {
    vFn.GetVarTable().SetParent(*x_pstVar);
    ENTER_SCOPE(x_pstVar, &vFn.GetVarTable());
    for (auto &upPar : vFn.GetPars())
        upPar->AcceptVisitor(*this);
    ENTER_SCOPE(x_pFn, &vFn);
    vFn.GetBody()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(VarDef &vVar) noexcept {
    auto &sName = vVar.GetName();
    auto pPrevious = x_pstVar->Add(sName, &vVar);
    if (pPrevious) {
        Y_Reject();
        y_vDriver.PrintError(
            vVar.GetLocation(),
            "redefinition of variable ", sName
        );
        y_vDriver.PrintNote(
            pPrevious->GetLocation(),
            "previous definition is here"
        );
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
        Y_Reject();
        y_vDriver.PrintError(
            stmt.GetLocation(),
            "break statement not in loop statement"
        );
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
            Y_Reject();
            y_vDriver.PrintError(
                stmt.GetCond()->GetLocation(),
                "type ", Type::tyBool, " expected, but got type ", stmt.GetCond()->GetType()
            );
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
        Y_Reject();
        y_vDriver.PrintError(
            stmt.GetCond()->GetLocation(),
            "type ", Type::tyBool, " expected, but got type ", stmt.GetCond()->GetType()
        );
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
            Y_Reject();
            y_vDriver.PrintError(
                upArg->GetLocation(),
                "type ", Type::tyInt, ", ", Type::tyBool, " or ", Type::tyString,
                " expected, but got type ", upArg->GetType()
            );
        }
    }
}

void TypeVisitor::Visit(ReturnStmt &stmt) noexcept {
    assert(x_pFn);
    if (stmt.GetExpr()) {
        stmt.GetExpr()->AcceptVisitor(*this);
        if (x_pFn->GetType() == Type::tyVoid) {
            Y_Reject();
            y_vDriver.PrintError(
                stmt.GetExpr()->GetLocation(),
                "cannot return a value for ", x_pFn->GetType(), " function ", x_pFn->GetName()
            );
            y_vDriver.PrintNote(
                x_pFn->GetLocation(),
                "function ", x_pFn->GetName(), " is defined here"
            );
            return;
        }
        if (!x_pFn->GetType().Accepts(stmt.GetExpr()->GetType())) {
            Y_Reject();
            y_vDriver.PrintError(
                stmt.GetExpr()->GetLocation(),
                "cannot return object of type ", stmt.GetExpr()->GetType(),
                " for ", x_pFn->GetType(), " function ", x_pFn->GetName()
            );
            y_vDriver.PrintNote(
                x_pFn->GetLocation(),
                "function ", x_pFn->GetName(), " is defined here"
            );
            return;
        }
    }
    else {
        if (x_pFn->GetType() != Type::tyVoid) {
            Y_Reject();
            y_vDriver.PrintError(
                stmt.GetLocation(),
                x_pFn->GetType(), " function must return with a value"
            );
            y_vDriver.PrintNote(
                x_pFn->GetLocation(),
                "function ", x_pFn->GetName(), " is defined here"
            );
            return;
        }
    }
}

void TypeVisitor::Visit(WhileStmt &stmt) noexcept {
    stmt.GetCond()->AcceptVisitor(*this);
    if (stmt.GetCond()->GetType() != Type::tyBool) {
        Y_Reject();
        y_vDriver.PrintError(
            stmt.GetCond()->GetLocation(),
            "type ", Type::tyBool, " expected, but got type ", stmt.GetCond()->GetType()
        );
    }
    ENTER_SCOPE(x_pLoop, &stmt);
    stmt.GetBody()->AcceptVisitor(*this);
}

void TypeVisitor::Visit(TypeName &vTypeName) noexcept {
    switch (vTypeName.GetId()) {
    case TypeId::kVoid:
        x_ty = {VoidType::vInstance, vTypeName.GetDimension()};
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
            Y_Reject();
            y_vDriver.PrintError(
                vTypeName.GetLocation(),
                "unknown class ", vTypeName.GetName()
            );
            x_ty = Type::tyVoid;
            break;
        }
        x_ty = {*pClass, vTypeName.GetDimension()};
        break;
    }
    }
}

}

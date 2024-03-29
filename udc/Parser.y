%skeleton "lalr1-c++17.cc"
%require "3.0.4"
%locations

%code requires {

#include "ast/All.hpp"

namespace udc {
    class Driver;
    class Scanner;
}

#ifdef _MSC_VER
#pragma warning(push)
// warning C4065: switch statement contains 'default' but no 'case' labels
#pragma warning(disable: 4065)
// warning C4244: 'return': conversion from 'std::streamsize' to 'int', possible loss of data
#pragma warning(disable: 4244)
#endif
}

%code provides {
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

%code {
#include <utility>

#include "Driver.hpp"
#include "Print.hpp"
#include "Scanner.hpp"

#ifdef yylex
#undef yylex
#endif
#define yylex x_vScanner.NextToken
}

%defines
%define api.namespace {udc}
%define api.token.constructor
%define api.token.prefix {k}
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%define parser_class_name {Parser}

%parse-param {Driver &x_drv}
%parse-param {Scanner &x_vScanner}
%parse-param {const std::filesystem::path &x_paInput}
%parse-param {std::string x_sFile}

%initial-action {
    @$.begin.filename = @$.end.filename = &x_sFile;
}

%token End 0 "end-of-file"

%token KwBool           "bool"
%token KwBreak          "break"
%token KwClass          "class"
%token KwElse           "else"
%token KwExtends        "extends"
%token KwFor            "for"
%token KwIf             "if"
%token KwInt            "int"
%token KwNew            "new"
%token KwNull           "null"
%token KwReturn         "return"
%token KwString         "string"
%token KwThis           "this"
%token KwVoid           "void"
%token KwWhile          "while"
%token KwStatic         "static"
%token KwPrint          "Print"
%token KwReadInteger    "ReadInteger"
%token KwReadLine       "ReadLine"
%token KwInstanceOf     "instanceof"

%token <std::string>    Identifier  "identifier"
%token <int>            IntLit      "integer-literal"
%token <bool>           BoolLit     "bool-literal"
%token <std::string>    StrLit      "string-literal"

%token OpAdd "+"
%token OpSub "-"
%token OpMul "*"
%token OpDiv "/"
%token OpMod "%"
%token OpLes "<"
%token OpLeq "<="
%token OpGre ">"
%token OpGeq ">="
%token OpAsg "="
%token OpEqu "=="
%token OpNeq "!="
%token OpAnd "&&"
%token OpIor "||"
%token OpNot "!"

%token PuSem ";"
%token PuCom ","
%token PuDot "."
%token PuSqo "["
%token PuSqc "]"
%token PuRno "("
%token PuRnc ")"
%token PuCuo "{"
%token PuCuc "}"

%type <std::vector<std::unique_ptr<ast::ClassDef>>> ClassDefList
%type <std::unique_ptr<ast::ClassDef>> ClassDef
%type <std::vector<std::unique_ptr<ast::NodeBase>>> FieldListOpt BlockItemListOpt
%type <std::unique_ptr<ast::VarDef>> VariableDef Variable
%type <std::unique_ptr<ast::FnDef>> FunctionDef
%type <std::unique_ptr<ast::TypeName>> Type
%type <std::vector<std::unique_ptr<ast::VarDef>>> Formals VariableList
%type <std::unique_ptr<ast::BlockStmt>> BlockStmt
%type <std::unique_ptr<ast::NodeBase>> Stmt
%type <std::unique_ptr<ast::IfStmt>> IfStmt
%type <std::unique_ptr<ast::WhileStmt>> WhileStmt
%type <std::unique_ptr<ast::ForStmt>> ForStmt
%type <std::unique_ptr<ast::BreakStmt>> BreakStmt
%type <std::unique_ptr<ast::ReturnStmt>> ReturnStmt
%type <std::unique_ptr<ast::PrintStmt>> PrintStmt
%type <std::unique_ptr<ast::ExprBase>> Expr ExprOpt Call PrimaryExpr PrimaryNoNewArray
%type <std::unique_ptr<ast::ExprBase>> OrExpr AndExpr EquExpr RelExpr AddExpr MulExpr UnaryExpr
%type <std::unique_ptr<ast::ExprBase>> Literal LValue
%type <std::vector<std::unique_ptr<ast::ExprBase>>> Actuals ExprList

%precedence UnmatchedIf
%precedence "else"

%start Program

%%
Program:
    ClassDefList[cdl] {
        x_drv.SetProgram(std::make_unique<ast::Program>(@1, x_paInput, std::move($cdl)));
    };

ClassDefList:
    ClassDef[cd] {
        $$.emplace_back(std::move($cd));
    } |
    ClassDefList[cdl] ClassDef[cd] {
        $$ = std::move($cdl);
        $$.emplace_back(std::move($cd));
    };

ClassDef:
    "class" Identifier[id] "{" FieldListOpt[fl] "}" {
        $$ = std::make_unique<ast::ClassDef>(@1, std::move($id), std::nullopt, std::move($fl));
    } |
    "class" Identifier[id] "extends" Identifier[base] "{" FieldListOpt[fl] "}" {
        $$ = std::make_unique<ast::ClassDef>(@1, std::move($id), std::move($base), std::move($fl));
    };

FieldListOpt:
    %empty {} |
    FieldListOpt[fl] VariableDef[vd] {
        $$ = std::move($fl);
        $$.emplace_back(std::move($vd));
    } |
    FieldListOpt[fl] FunctionDef[fd] {
        $$ = std::move($fl);
        $$.emplace_back(std::move($fd));
    };

VariableDef:
    Variable[var] ";" {
        $$ = std::move($var);
    };

Variable:
    Type[ty] Identifier[id] {
        $$ = std::make_unique<ast::VarDef>(@1, std::move($ty), std::move($id));
    };

Type:
    "void" {
        $$ = std::make_unique<ast::TypeName>(@1, ast::TypeId::kVoid);
    } |
    "int" {
        $$ = std::make_unique<ast::TypeName>(@1, ast::TypeId::kInt);
    } |
    "bool" {
        $$ = std::make_unique<ast::TypeName>(@1, ast::TypeId::kBool);
    } |
    "string" {
        $$ = std::make_unique<ast::TypeName>(@1, ast::TypeId::kString);
    } |
    "class" Identifier[id] {
        $$ = std::make_unique<ast::TypeName>(@1, std::move($id));
    } |
    Type[ty] "[" "]" {
        $$ = std::move($ty);
        $$->AddDimension();
    };

FunctionDef:
    Type[ty] Identifier[id] "(" Formals[pars] ")" BlockStmt[body] {
        $$ = std::make_unique<ast::FnDef>(@1, false, std::move($ty), std::move($id), std::move($pars), std::move($body));
    } |
    "static" Type[ty] Identifier[id] "(" Formals[pars] ")" BlockStmt[body] {
        $$ = std::make_unique<ast::FnDef>(@1, true, std::move($ty), std::move($id), std::move($pars), std::move($body));
    };

Formals:
    %empty {} |
    VariableList[vl] {
        $$ = std::move($vl);
    };

VariableList:
    Variable[v] {
        $$.emplace_back(std::move($v));
    } |
    VariableList[vl] "," Variable[v] {
        $$ = std::move($vl);
        $$.emplace_back(std::move($v));
    };

BlockStmt:
    "{" BlockItemListOpt[bil] "}" {
        $$ = std::make_unique<ast::BlockStmt>(@1, std::move($bil));
    };

BlockItemListOpt:
    %empty {} |
    BlockItemListOpt[bil] VariableDef[vd] {
        $$ = std::move($bil);
        $$.emplace_back(std::move($vd));
    } |
    BlockItemListOpt[bil] Stmt[st] {
        $$ = std::move($bil);
        $$.emplace_back(std::move($st));
    };

Stmt:
    ";" {
        $$ = std::make_unique<ast::ExprStmt>(@1, nullptr);
    } |
    Expr[e] ";" {
        $$ = std::make_unique<ast::ExprStmt>(@1, std::move($e));
    } |
    IfStmt[st] {
        $$ = std::move($st);
    } |
    WhileStmt[st] {
        $$ = std::move($st);
    } |
    ForStmt[st] {
        $$ = std::move($st);
    } |
    BreakStmt[st] {
        $$ = std::move($st);
    } |
    ReturnStmt[st] {
        $$ = std::move($st);
    } |
    PrintStmt[st] {
        $$ = std::move($st);
    } |
    BlockStmt[st] {
        $$ = std::move($st);
    };

Expr:
    OrExpr[e] {
        $$ = std::move($e);
    } |
    LValue[l] "=" Expr[r] {
        $$ = std::make_unique<ast::AssignExpr>(@1, std::move($l), std::move($r));
    };

OrExpr:
    AndExpr[e] {
        $$ = std::move($e);
    } |
    OrExpr[l] "||" AndExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kIor, std::move($l), std::move($r));
    };

AndExpr:
    EquExpr[e] {
        $$ = std::move($e);
    } |
    AndExpr[l] "&&" EquExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kAnd, std::move($l), std::move($r));
    };

EquExpr:
    RelExpr[e] {
        $$ = std::move($e);
    } | 
    EquExpr[l] "==" RelExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kEqu, std::move($l), std::move($r));
    } |
    EquExpr[l] "!=" RelExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kNeq, std::move($l), std::move($r));
    };

RelExpr:
    AddExpr[e] {
        $$ = std::move($e);
    } |
    RelExpr[l] "<" AddExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kLes, std::move($l), std::move($r));
    } |
    RelExpr[l] ">" AddExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kGre, std::move($l), std::move($r));
    } |
    RelExpr[l] "<=" AddExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kLeq, std::move($l), std::move($r));
    } |
    RelExpr[l] ">=" AddExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kGeq, std::move($l), std::move($r));
    };

AddExpr:
    MulExpr[e] {
        $$ = std::move($e);
    } |
    AddExpr[l] "+" MulExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kAdd, std::move($l), std::move($r));
    } |
    AddExpr[l] "-" MulExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kSub, std::move($l), std::move($r));
    };

MulExpr:
    UnaryExpr[e] {
        $$ = std::move($e);
    } |
    MulExpr[l] "*" UnaryExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kMul, std::move($l), std::move($r));
    } |
    MulExpr[l] "/" UnaryExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kDiv, std::move($l), std::move($r));
    } |
    MulExpr[l] "%" UnaryExpr[r] {
        $$ = std::make_unique<ast::BinaryExpr>(@1, ast::BinOp::kMod, std::move($l), std::move($r));
    };

UnaryExpr:
    PrimaryExpr[e] {
        $$ = std::move($e);
    } |
    "-" UnaryExpr[e] {
        $$ = std::make_unique<ast::UnaryExpr>(@1, ast::UnaOp::kNeg, std::move($e));
    } |
    "!" UnaryExpr[e] {
        $$ = std::make_unique<ast::UnaryExpr>(@1, ast::UnaOp::kNot, std::move($e));
    } |
    "(" "class" Identifier[id] ")" UnaryExpr[e] {
        $$ = std::make_unique<ast::CastExpr>(@1, std::move($id), std::move($e));
    };

PrimaryExpr:
    PrimaryNoNewArray[e] {
        $$ = std::move($e);
    } |
    "new" Type[ty] "[" Expr[e] "]" {
        $$ = std::make_unique<ast::NewArrayExpr>(@1, std::move($ty), std::move($e));
    };

PrimaryNoNewArray:
    Literal[e] {
        $$ = std::move($e);
    } |
    "this" {
        $$ = std::make_unique<ast::This>(@1);
    } |
    LValue[e] {
        $$ = std::move($e);
    } |
    "(" Expr[e] ")" {
        $$ = std::move($e);
    } |
    "new" Identifier[id] "(" ")" {
        $$ = std::make_unique<ast::NewClassExpr>(@1, std::move($id));
    } |
    Call[e] {
        $$ = std::move($e);
    };

Literal:
    "null" {
        $$ = std::make_unique<ast::NullLit>(@1);
    } |
    IntLit[n] {
        $$ = std::make_unique<ast::IntLit>(@1, $n);
    } |
    BoolLit[b] {
        $$ = std::make_unique<ast::BoolLit>(@1, $b);
    } |
    StrLit[s] {
        $$ = std::make_unique<ast::StrLit>(@1, std::move($s));
    };

LValue:
    Identifier[id] {
        $$ = std::make_unique<ast::VarAccess>(@1, nullptr, std::move($id));
    } |
    PrimaryExpr[e] "." Identifier[id] {
        $$ = std::make_unique<ast::VarAccess>(@1, std::move($e), std::move($id));
    } |
    PrimaryNoNewArray[e] "[" Expr[sub] "]" {
        $$ = std::make_unique<ast::ArrayAccess>(@1, std::move($e), std::move($sub));
    };

Call:
    Identifier[id] "(" Actuals[args] ")" {
        $$ = std::make_unique<ast::CallExpr>(@1, nullptr, std::move($id), std::move($args));
    } |
    PrimaryExpr[e] "." Identifier[id] "(" Actuals[args] ")" {
        $$ = std::make_unique<ast::CallExpr>(@1, std::move($e), std::move($id), std::move($args));
    } |
    "ReadInteger" "(" ")" {
        $$ = std::make_unique<ast::ReadInteger>(@1);
    } |
    "ReadLine" "(" ")" {
        $$ = std::make_unique<ast::ReadLine>(@1);
    } |
    "instanceof" "(" Expr[e] "," Identifier[id] ")" {
        $$ = std::make_unique<ast::InstanceOf>(@1, std::move($e), std::move($id));
    };

Actuals:
    %empty {} |
    ExprList[el] {
        $$ = std::move($el);
    };

ExprList:
    Expr[e] {
        $$.emplace_back(std::move($e));
    } |
    ExprList[el] "," Expr[e] {
        $$ = std::move($el);
        $$.emplace_back(std::move($e));
    };

IfStmt:
    "if" "(" Expr[e] ")" Stmt[st] %prec UnmatchedIf {
        $$ = std::make_unique<ast::IfStmt>(@1, std::move($e), std::move($st), nullptr);
    } |
    "if" "(" Expr[e] ")" Stmt[st] "else" Stmt[est] {
        $$ = std::make_unique<ast::IfStmt>(@1, std::move($e), std::move($st), std::move($est));
    };

WhileStmt:
    "while" "(" Expr[e] ")" Stmt[st] {
        $$ = std::make_unique<ast::WhileStmt>(@1, std::move($e), std::move($st));
    };

ForStmt:
    "for" "(" ExprOpt[e1] ";" ExprOpt[e2] ";" ExprOpt[e3] ")" Stmt[st] {
        $$ = std::make_unique<ast::ForStmt>(@1, std::move($e1), std::move($e2), std::move($e3), std::move($st));
    };

ExprOpt:
    %empty {} |
    Expr[e] {
        $$ = std::move($e);
    };

BreakStmt:
    "break" ";" {
        $$ = std::make_unique<ast::BreakStmt>(@1);
    };

ReturnStmt:
    "return" ExprOpt[e] ";" {
        $$ = std::make_unique<ast::ReturnStmt>(@1, std::move($e));
    };

PrintStmt:
    "Print" "(" ExprList[el] ")" ";" {
        $$ = std::make_unique<ast::PrintStmt>(@1, std::move($el));
    }

%%

void ::udc::Parser::error(const location_type &loc, const std::string &sMsg) {
    ::udc::PrintError(loc, sMsg);
}

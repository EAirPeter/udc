%{
#ifdef __GNUC__
// warning: comparison of integer expressions of different signedness: ‘int’ and ‘yy_size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
// if ((int) ((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif


#ifdef _MSC_VER
// warning C4244: 'return': conversion from 'std::streamsize' to 'int', possible loss of data
#pragma warning(disable: 4244)
#endif

#include "Driver.hpp"
#include "Scanner.hpp"

#include <string>

#define CONCAT_(a_, b_) a_ ## b_
#define CONCAT(a_, b_) a_ ## b_

#define MkComment() (x_bPrint ? [] (const Location &l, std::string s) { \
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) \
        s.pop_back(); \
    std::cout << l << ": Comment[" << s << "]" << std::endl; \
}(x_vLocation, YYText()) : (void) 0)

#define MkEnd() (x_bPrint ? [] (const Location &l) { \
    std::cout << l << ": EndOfFile" << std::endl; \
    return ::udc::Parser::make_End(l); \
}(x_vLocation) : ::udc::Parser::make_End(x_vLocation))

#define MkKeyword(type_) (x_bPrint ? [] (const Location &l, const std::string &s) { \
    std::cout << l << ": Keyword[" << s << "]" << std::endl; \
    return ::udc::Parser::CONCAT(make_Kw, type_)(l); \
}(x_vLocation, YYText()) : ::udc::Parser::CONCAT(make_Kw, type_)(x_vLocation))

#define MkIdentifier(val_) (x_bPrint ? [] (const Location &l, const std::string &s) { \
    std::cout << l << ": Identifier[" << s << "]" << std::endl; \
    return ::udc::Parser::make_Identifier(s, l); \
}(x_vLocation, (val_)) : ::udc::Parser::make_Identifier((val_), x_vLocation))

#define MkBoolLit(val_) (x_bPrint ? [] (const Location &l, const std::string &s, bool b) { \
    std::cout << l << ": BoolLit[" << s << "]" << std::endl; \
    return ::udc::Parser::make_BoolLit(b, l); \
}(x_vLocation, YYText(), (val_)) : ::udc::Parser::make_BoolLit((val_), x_vLocation))

#define MkIntLit(val_) (x_bPrint ? [] (const Location &l, const std::string &s, int n) { \
    std::cout << l << ": IntLit[" << s << "]" << std::endl; \
    return ::udc::Parser::make_IntLit(n, l); \
}(x_vLocation, YYText(), (val_)) : ::udc::Parser::make_IntLit((val_), x_vLocation))

#define MkStrLit(val_) (x_bPrint ? [] (const Location &l, const std::string &org, const std::string &s) { \
    std::cout << l << ": StrLit[" << org << "]" << std::endl; \
    return ::udc::Parser::make_StrLit(s, l); \
}(x_vLocation, sOrg, (val_)) : ::udc::Parser::make_StrLit((val_), x_vLocation))

#define MkOperator(type_) (x_bPrint ? [] (const Location &l, const std::string &s) { \
    std::cout << l << ": Operator[" << s << "]" << std::endl; \
    return ::udc::Parser::CONCAT(make_Op, type_)(l); \
}(x_vLocation, YYText()) : ::udc::Parser::CONCAT(make_Op, type_)(x_vLocation))

#define MkPunctuation(type_) (x_bPrint ? [] (const Location &l, const std::string &s) { \
    std::cout << l << ": Punctuation[" << s << "]" << std::endl; \
    return ::udc::Parser::CONCAT(make_Pu, type_)(l); \
}(x_vLocation, YYText()) : ::udc::Parser::CONCAT(make_Pu, type_)(x_vLocation))

#define Complain(...) (x_vDriver.PrintError(x_vLocation, __VA_ARGS__))

#ifdef YY_DECL
#undef YY_DECL
#endif
#define YY_DECL ::udc::Symbol udc::Scanner::NextSymbol() noexcept

#ifdef yyterminate
#undef yyterminate
#endif
#define yyterminate() return MkEnd()

#ifdef YY_USER_ACTION
#undef YY_USER_ACTION
#endif
#define YY_USER_ACTION x_vLocation.columns(YYLeng());
%}

%option batch
%option c++
%option debug
%option nodefault
%option noinput
%option nounistd
%option nounput
%option noyywrap
%option prefix="Udc"
%option yyclass="Scanner"

%x StrLit

LineBreak (\r|\n|\r\n)

%%
%{
    std::string sOrg {};
    std::string sBuf {};
%}

{LineBreak}+ {
    x_vLocation.lines(YYLeng());
    x_vLocation.step();
}

[ \t]+ {
    x_vLocation.step();
}

"//"[^\r\n]*{LineBreak} {
    MkComment();
    x_vLocation.lines();
    x_vLocation.step();
}

"bool"          return MkKeyword(Bool);
"break"         return MkKeyword(Break);
"class"         return MkKeyword(Class);
"else"          return MkKeyword(Else);
"extends"       return MkKeyword(Extends);
"for"           return MkKeyword(For);
"if"            return MkKeyword(If);
"int"           return MkKeyword(Int);
"new"           return MkKeyword(New);
"null"          return MkKeyword(Null);
"return"        return MkKeyword(Return);
"string"        return MkKeyword(String);
"this"          return MkKeyword(This);
"void"          return MkKeyword(Void);
"while"         return MkKeyword(While);
"static"        return MkKeyword(Static);
"Print"         return MkKeyword(Print);
"ReadInteger"   return MkKeyword(ReadInteger);
"ReadLine"      return MkKeyword(ReadLine);
"instanceof"    return MkKeyword(InstanceOf);

"false"         return MkBoolLit(false);
"true"          return MkBoolLit(true);

"+"     return MkOperator(Add);
"-"     return MkOperator(Sub);
"*"     return MkOperator(Mul);
"/"     return MkOperator(Div);
"%"     return MkOperator(Mod);
"<"     return MkOperator(Les);
"<="    return MkOperator(Leq);
">"     return MkOperator(Gre);
">="    return MkOperator(Geq);
"="     return MkOperator(Asg);
"=="    return MkOperator(Equ);
"!="    return MkOperator(Neq);
"&&"    return MkOperator(And);
"||"    return MkOperator(Ior);
"!"     return MkOperator(Not);

";"     return MkPunctuation(Sem);
","     return MkPunctuation(Com);
"."     return MkPunctuation(Dot);
"["     return MkPunctuation(Sqo);
"]"     return MkPunctuation(Sqc);
"("     return MkPunctuation(Rno);
")"     return MkPunctuation(Rnc);
"{"     return MkPunctuation(Cuo);
"}"     return MkPunctuation(Cuc);

[0-9]+ {
    try {
        return MkIntLit(std::stoi(YYText(), nullptr, 10));
    }
    catch (...) {
        Complain("The integer constant [", YYText(), "] is too large.");
    }
}

0[xX][0-9A-Fa-f]+ {
    try {
        return MkIntLit(std::stoi(YYText(), nullptr, 16));
    }
    catch (...) {
        Complain("The integer constant [", YYText(), "] is too large.");
    }
}

[A-Za-z][_0-9A-Za-z]* {
    return MkIdentifier(YYText());
}

<INITIAL>"\"" {
    BEGIN(StrLit);
    if (x_bPrint)
        sOrg = YYText();
    sBuf.clear();
}
<StrLit>"\"" {
    BEGIN(INITIAL);
    if (x_bPrint)
        sOrg += YYText();
    return MkStrLit(sBuf);
}
<StrLit><<EOF>>     { Complain("Unexpected EOF."); BEGIN(INITIAL); }
<StrLit>{LineBreak} { Complain("Unexpected line break."); BEGIN(INITIAL); }
<StrLit>"\\\""      { if (x_bPrint) sOrg += YYText(); sBuf += '\"'; }
<StrLit>"\\\\"      { if (x_bPrint) sOrg += YYText(); sBuf += '\\'; }
<StrLit>"\\t"       { if (x_bPrint) sOrg += YYText(); sBuf += '\t'; }
<StrLit>"\\n"       { if (x_bPrint) sOrg += YYText(); sBuf += '\n'; }
<StrLit>.           { if (x_bPrint) sOrg += YYText(); sBuf += YYText(); }

.   Complain("Unexpected character [", YYText(), "].");

%%

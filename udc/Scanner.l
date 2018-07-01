%{
#ifdef __GNUC__
// warning: comparison of integer expressions of different signedness: ‘int’ and ‘yy_size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
// if ((int) ((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "Common.hpp"
#include "Print.hpp"
#include "Scanner.hpp"

#include <string>

#define MkComment() (x_bPrint ? [] (const location &l, std::string s) { \
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) \
        s.pop_back(); \
    std::cout << l << ": Comment[" << s << "]" << std::endl; \
}(x_loc, YYText()) : (void) 0)

#define MkEnd() (x_bPrint ? [] (const location &l) { \
    std::cout << l << ": EndOfFile" << std::endl; \
    return ::udc::Parser::make_End(l); \
}(x_loc) : ::udc::Parser::make_End(x_loc))

#define MkKeyword(type_) (x_bPrint ? [] (const location &l, const std::string &s) { \
    std::cout << l << ": Keyword[" << s << "]" << std::endl; \
    return ::udc::Parser::CAT(make_Kw, type_)(l); \
}(x_loc, YYText()) : ::udc::Parser::CAT(make_Kw, type_)(x_loc))

#define MkIdentifier(val_) (x_bPrint ? [] (const location &l, const std::string &s) { \
    std::cout << l << ": Identifier[" << s << "]" << std::endl; \
    return ::udc::Parser::make_Identifier(s, l); \
}(x_loc, (val_)) : ::udc::Parser::make_Identifier((val_), x_loc))

#define MkBoolLit(val_) (x_bPrint ? [] (const location &l, const std::string &s, bool b) { \
    std::cout << l << ": BoolLit[" << s << "]" << std::endl; \
    return ::udc::Parser::make_BoolLit(b, l); \
}(x_loc, YYText(), (val_)) : ::udc::Parser::make_BoolLit((val_), x_loc))

#define MkIntLit(val_) (x_bPrint ? [] (const location &l, const std::string &s, int n) { \
    std::cout << l << ": IntLit[" << s << "]" << std::endl; \
    return ::udc::Parser::make_IntLit(n, l); \
}(x_loc, YYText(), (val_)) : ::udc::Parser::make_IntLit((val_), x_loc))

#define MkStrLit(val_) (x_bPrint ? [] (const location &l, const std::string &org, const std::string &s) { \
    std::cout << l << ": StrLit[" << org << "]" << std::endl; \
    return ::udc::Parser::make_StrLit(s, l); \
}(x_loc, sOrg, (val_)) : ::udc::Parser::make_StrLit((val_), x_loc))

#define MkOperator(type_) (x_bPrint ? [] (const location &l, const std::string &s) { \
    std::cout << l << ": Operator[" << s << "]" << std::endl; \
    return ::udc::Parser::CAT(make_Op, type_)(l); \
}(x_loc, YYText()) : ::udc::Parser::CAT(make_Op, type_)(x_loc))

#define MkPunctuation(type_) (x_bPrint ? [] (const location &l, const std::string &s) { \
    std::cout << l << ": Punctuation[" << s << "]" << std::endl; \
    return ::udc::Parser::CAT(make_Pu, type_)(l); \
}(x_loc, YYText()) : ::udc::Parser::CAT(make_Pu, type_)(x_loc))

#define Complain(...) (::udc::PrintError(x_loc, __VA_ARGS__))

#ifdef YY_DECL
#undef YY_DECL
#endif
#define YY_DECL ::udc::Parser::symbol_type udc::Scanner::NextToken() noexcept

#ifdef yyterminate
#undef yyterminate
#endif
#define yyterminate() return MkEnd()

#ifdef YY_USER_ACTION
#undef YY_USER_ACTION
#endif
#define YY_USER_ACTION x_loc.columns(YYLeng());
%}

%option batch
%option c++
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
    x_loc.lines(YYLeng());
    x_loc.step();
}

[ \t]+ {
    x_loc.step();
}

"//"[^\r\n]*{LineBreak} {
    MkComment();
    x_loc.lines();
    x_loc.step();
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

namespace udc {

Scanner::Scanner(std::istream &is) : UdcFlexLexer(is, std::cout) {}

}

#ifndef UDC_SCANNER_HPP_
#define UDC_SCANNER_HPP_

#include <iosfwd>

#include "Parser.hpp"

#if !defined(yyFlexLexerOnce)
#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer UdcFlexLexer
#include <FlexLexer.h>
#endif

namespace udc {

class Scanner : public UdcFlexLexer {
public:
    Scanner(std::istream &is);
    virtual ~Scanner() = default;

public:
    constexpr void SetPrint(bool bPrint) noexcept {
        x_bPrint = bPrint;
    }
    Parser::symbol_type NextToken() noexcept;

private:
    location x_loc;
    bool x_bPrint = false;

};

}

#endif

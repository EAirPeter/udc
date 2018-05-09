#ifndef UDC_SCANNER_HPP_
#define UDC_SCANNER_HPP_

#include "Location.hpp"
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
    inline Scanner(Driver &vDriver, bool bPrint) noexcept : x_vDriver {vDriver}, x_bPrint {bPrint} {};

    virtual ~Scanner() = default;

public:
    constexpr void SetPrint(bool bPrint) noexcept {
        x_bPrint = bPrint;
    }
    Symbol NextSymbol() noexcept;

private:
    Driver &x_vDriver;
    Location x_vLocation;
    bool x_bPrint;

};

}

#endif

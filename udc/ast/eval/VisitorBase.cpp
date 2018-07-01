#include "../../Location.hpp"
#include "../../Print.hpp"
#include "Type.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {

void VisitorBase::Y_RjLocalShadow(
    const location &loc, const std::string &sName, const location &vPrevious
) noexcept {
    Y_Reject();
    PrintError(loc, "local variable shadows class name");
    PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjDimTooLarge(const location &loc) noexcept {
    Y_Reject();
    PrintError(loc, "too many dimensions");
}

void VisitorBase::Y_RjNonStaticVar(const location &loc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(loc, "invalid use of variable '", sName, "' in static function");
}

void VisitorBase::Y_RjNonStaticCall(const location &loc, const std::string &sName) noexcept
{
    Y_Reject();
    PrintError(loc, "call to non-static function '", sName, "' without an object argument");
}

void VisitorBase::Y_RjThisInStatic(const location &loc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(loc, "invalid use of 'this' in static function");
}

void VisitorBase::Y_RjOverride(const location &loc, const std::string &sName, const location &vPrevious) noexcept {
    Y_Reject();
    PrintError(loc, "overriding function '", sName, "' with different signature");
    PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjInheritCycle(const location &loc) noexcept {
    Y_Reject();
    PrintError(loc, "cyclic inheritance hierarchy is forbiddened");
    PrintNote(loc, "the cycle is detected here");
}

void VisitorBase::Y_RjVoidArray(const location &loc) noexcept {
    Y_Reject();
    PrintError(loc, "array of 'void' is not allowed");
}

void VisitorBase::Y_RjNotFound(const location &loc, const char *pszWhat, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(loc, "no ", pszWhat, " named '", sName, "' found");
}

void VisitorBase::Y_RjTypeMissMatch(const location &loc, const Type &tyExpected, const Type &tyUnexpected) noexcept {
    Y_Reject();
    PrintError(loc, "expected '", tyExpected, "', but '", tyUnexpected, "' encountered");
}

void VisitorBase::Y_RjNoConversion(const location &loc, const Type &tyTo, const Type &tyFrom) noexcept {
    Y_Reject();
    PrintError(loc, "no viable conversion from '", tyFrom, "' to '", tyTo, "'");
}

void VisitorBase::Y_RjNoComparison(const location &loc, const Type &tyLhs, const Type &tyRhs) noexcept {
    Y_Reject();
    PrintError(loc, "cannot compare between '", tyLhs, "' and '", tyRhs, "'");
}

void VisitorBase::Y_RjReturnForVoid(const location &loc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(loc, "cannot return a value for 'void' function '", sName, "'");
}

void VisitorBase::Y_RjNoReturnVal(const location &loc, const Type &tyExpected, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(loc, "must return a value for '", tyExpected, "' function '", sName, "'");
}

void VisitorBase::Y_RjRedefinition(
    const location &loc, const char *pszWhat,
    const std::string &sName, const location &vPrevious
) noexcept {
    Y_Reject();
    PrintError(loc, "redefinition of ", pszWhat, " '", sName, "'");
    PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjIllegalBreak(const location &loc) noexcept {
    Y_Reject();
    PrintError(loc, "break statement not in loop statement");
}

void VisitorBase::Y_RjNotPrintable(const location &loc, const Type &ty) noexcept {
    Y_Reject();
    PrintError(loc, "cannot print '", ty, "'");
}

void VisitorBase::Y_RjAssignNonLval(const location &loc) noexcept {
    Y_Reject();
    PrintError(loc, "expression not assignable");
}

void VisitorBase::Y_RjNotWhat(const location &loc, const char *pszWhat, const Type &ty) noexcept {
    Y_Reject();
    PrintError(loc, "'", ty, "' is not ", pszWhat);
}

void VisitorBase::Y_RjArgNumber(
    const location &loc, const std::string &sName,
    std::size_t cExpected, std::size_t cUnexpected
) noexcept {
    Y_Reject();
    PrintError(
        loc, "function '", sName, "' requires ", cExpected,
        " arguments, but ", cUnexpected, " were provided"
    );
}

namespace {
constexpr const char *OrderSuffixNtmbs(std::size_t idx) {
    switch (idx % 10) {
    case 1:
        return "st";
    case 2:
        return "nd";
    case 3:
        return "rd";
    default:
        return "th";
    }
}
}

void VisitorBase::Y_RjArgType(
    const location &loc, const std::string &sName,
    std::size_t idx, const std::string &sArg,
    const Type &tyExpected, const Type &tyUnexpected
) noexcept {
    Y_Reject();
    PrintError(
        loc, "no viable conversion from '", tyUnexpected, "' to '", tyExpected,
        "' for ", idx, OrderSuffixNtmbs(idx), " argument of function '", sName, "'"
    );
}

}

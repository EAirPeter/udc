#include "../../Driver.hpp"
#include "../../Print.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {

void VisitorBase::Y_RjDimTooLarge(const Location &vLoc) noexcept {
    Y_Reject();
    PrintError(vLoc, "too many dimensions");
}

void VisitorBase::Y_RjNonStaticVar(const Location &vLoc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(vLoc, "invalid use of variable '", sName, "' in static function");
}

void VisitorBase::Y_RjNonStaticCall(const Location &vLoc, const std::string &sName) noexcept
{
    Y_Reject();
    PrintError(vLoc, "call to non-static function '", sName, "' without an object argument");
}

void VisitorBase::Y_RjThisInStatic(const Location &vLoc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(vLoc, "invalid use of 'this' in static function");
}

void VisitorBase::Y_RjOverride(const Location &vLoc, const std::string &sName, const Location &vPrevious) noexcept {
    Y_Reject();
    PrintError(vLoc, "overriding function '", sName, "' with different signature");
    PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjInheritCycle(const Location &vLoc) noexcept {
    Y_Reject();
    PrintError(vLoc, "cyclic inheritance hierarchy is forbiddened");
    PrintNote(vLoc, "the cycle is detected here");
}

void VisitorBase::Y_RjVoidArray(const Location &vLoc) noexcept {
    Y_Reject();
    PrintError(vLoc, "array of 'void' is not allowed");
}

void VisitorBase::Y_RjNotFound(const Location &vLoc, const char *pszWhat, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(vLoc, "no ", pszWhat, " named '", sName, "' found");
}

void VisitorBase::Y_RjTypeMissMatch(const Location &vLoc, const Type &tyExpected, const Type &tyUnexpected) noexcept {
    Y_Reject();
    PrintError(vLoc, "expected '", tyExpected, "', but '", tyUnexpected, "' encountered");
}

void VisitorBase::Y_RjNoConversion(const Location &vLoc, const Type &tyTo, const Type &tyFrom) noexcept {
    Y_Reject();
    PrintError(vLoc, "no viable conversion from '", tyFrom, "' to '", tyTo, "'");
}

void VisitorBase::Y_RjNoComparison(const Location &vLoc, const Type &tyLhs, const Type &tyRhs) noexcept {
    Y_Reject();
    PrintError(vLoc, "cannot compare between '", tyLhs, "' and '", tyRhs, "'");
}

void VisitorBase::Y_RjReturnForVoid(const Location &vLoc, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(vLoc, "cannot return a value for 'void' function '", sName, "'");
}

void VisitorBase::Y_RjNoReturnVal(const Location &vLoc, const Type &tyExpected, const std::string &sName) noexcept {
    Y_Reject();
    PrintError(vLoc, "must return a value for '", tyExpected, "' function '", sName, "'");
}

void VisitorBase::Y_RjRedefinition(
    const Location &vLoc, const char *pszWhat,
    const std::string &sName, const Location &vPrevious
) noexcept {
    Y_Reject();
    PrintError(vLoc, "redefinition of ", pszWhat, " '", sName, "'");
    PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjIllegalBreak(const Location &vLoc) noexcept {
    Y_Reject();
    PrintError(vLoc, "break statement not in loop statement");
}

void VisitorBase::Y_RjNotPrintable(const Location &vLoc, const Type &ty) noexcept {
    Y_Reject();
    PrintError(vLoc, "cannot print '", ty, "'");
}

void VisitorBase::Y_RjAssignNonLval(const Location &vLoc) noexcept {
    Y_Reject();
    PrintError(vLoc, "expression not assignable");
}

void VisitorBase::Y_RjNotWhat(const Location &vLoc, const char *pszWhat, const Type &ty) noexcept {
    Y_Reject();
    PrintError(vLoc, "'", ty, "' is not ", pszWhat);
}

void VisitorBase::Y_RjArgNumber(
    const Location &vLoc, const std::string &sName,
    std::size_t cExpected, std::size_t cUnexpected
) noexcept {
    Y_Reject();
    PrintError(
        vLoc, "function '", sName, "' requires ", cExpected,
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
    const Location &vLoc, const std::string &sName,
    std::size_t idx, const std::string &sArg,
    const Type &tyExpected, const Type &tyUnexpected
) noexcept {
    Y_Reject();
    PrintError(
        vLoc, "no viable conversion from '", tyUnexpected, "' to '", tyExpected,
        "' for ", idx, OrderSuffixNtmbs(idx), " argument of function '", sName, "'"
    );
}

}

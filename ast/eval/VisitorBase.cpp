#include "../../Driver.hpp"
#include "VisitorBase.hpp"

namespace udc::ast::eval {


void VisitorBase::Y_RJThisInStatic(const Location &vLoc, const std::string &sName) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "invalid use of 'this' in a static function");
}

void VisitorBase::Y_RjOverride(const Location &vLoc, const std::string &sName, const Location &vPrevious) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "overriding function '", sName, "' with different signature");
    y_vDriver.PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjInheritCycle(const Location &vLoc) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "cyclic inheritance hierarchy is forbiddened");
    y_vDriver.PrintNote(vLoc, "the cycle is detected here");
}

void VisitorBase::Y_RjIllegalType(const Location &vLoc, const Type &ty) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "invalid type '", ty, "'");
}

void VisitorBase::Y_RjNotFound(const Location &vLoc, const char *pszWhat, const std::string &sName) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "no ", pszWhat, " named '", sName, "' found");
}

void VisitorBase::Y_RjTypeMissMatch(const Location &vLoc, const Type &tyExpected, const Type &tyUnexpected) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "expected '", tyExpected, "', but '", tyUnexpected, "' encountered");
}

void VisitorBase::Y_RjNoConversion(const Location &vLoc, const Type &tyTo, const Type &tyFrom) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "no viable conversion from '", tyFrom, "' to '", tyTo, "'");
}

void VisitorBase::Y_RjNoComparison(const Location &vLoc, const Type &tyLhs, const Type &tyRhs) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "cannot compare between '", tyLhs, "' and '", tyRhs, "'");
}

void VisitorBase::Y_RjReturnForVoid(const Location &vLoc, const std::string &sName) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "cannot return a value for '", Type::tyVoid, "' function '", sName, "'");
}

void VisitorBase::Y_RjNoReturnVal(const Location &vLoc, const Type &tyExpected, const std::string &sName) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "must return a value for '", tyExpected, "' function '", sName, "'");
}

void VisitorBase::Y_RjRedefinition(
    const Location &vLoc, const char *pszWhat,
    const std::string &sName, const Location &vPrevious
) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "redefinition of ", pszWhat, " '", sName, "'");
    y_vDriver.PrintNote(vPrevious, "previous definition is here");
}

void VisitorBase::Y_RjIllegalBreak(const Location &vLoc) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "break statement not in loop statement");
}

void VisitorBase::Y_RjNotPrintable(const Location &vLoc, const Type &ty) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "cannot print '", ty, "'");
}

void VisitorBase::Y_RjAssignNonLval(const Location &vLoc) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "expression not assignable");
}

void VisitorBase::Y_RjNotWhat(const Location &vLoc, const char *pszWhat, const Type &ty) noexcept {
    Y_Reject();
    y_vDriver.PrintError(vLoc, "'", ty, "' is not ", pszWhat);
}

void VisitorBase::Y_RjArgNumber(
    const Location &vLoc, const std::string &sName,
    std::size_t cExpected, std::size_t cUnexpected
) noexcept {
    Y_Reject();
    y_vDriver.PrintError(
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
    y_vDriver.PrintError(
        vLoc, "no viable conversion from '", tyUnexpected, "' to '", tyExpected,
        "' for ", idx, OrderSuffixNtmbs(idx), " argument of function '", sName, "'"
    );
}

}

#ifndef UDC_AST_PROGRAM_HPP_
#define UDC_AST_PROGRAM_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class Program : public Base {
public:
    Program(const Location &vLocation, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept;
    virtual ~Program();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::vector<std::unique_ptr<ClassDef>> x_vecClasses;
};

}

#endif

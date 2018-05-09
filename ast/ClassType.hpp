#ifndef UDC_AST_CLASS_TYPE_HPP_
#define UDC_AST_CLASS_TYPE_HPP_

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ClassType : public Base, public IType {
public:
    ClassType(const Location &vLocation, std::string &&sName) noexcept;
    virtual ~ClassType();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    virtual std::string GetName() const noexcept override;

private:
    std::string x_sName;

};

}

#endif

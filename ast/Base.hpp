#ifndef UDC_AST_BASE_HPP_
#define UDC_AST_BASE_HPP_

#include "Interface.hpp"

namespace udc::ast {

class Base : public virtual INode {
public:
    Base(const Location &vLocation) noexcept;
    Base(const Base &) = delete;
    Base(Base &&) = delete;
    virtual ~Base();

    Base &operator =(const Base &) = delete;
    Base &operator =(Base &&) = delete;

public:
    virtual Location GetLocation() const noexcept override final;

private:
    const Location x_vLocation;
};

}

#endif

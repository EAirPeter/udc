#ifndef UDC_AST_PROGRAM_HPP_
#define UDC_AST_PROGRAM_HPP_

#include "Node.hpp"

namespace udc::ast {

class ClassDef;

class Program {
public:
    Program(const Location &vLocation, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept;
protected:
    virtual ~
private:
    
};

}

#endif

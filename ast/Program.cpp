#include "ClassDef.hpp"
#include "Program.hpp"

namespace udc::ast {

Program::Program(const Location &vLocation, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept :
    Base(vLocation), x_vecClasses(std::move(vecClasses))
{}

Program::~Program() {}

void Program::Print(std::ostream &os, std::uint32_t cIndent) const {
    bool bFirst = true;
    for (auto &&upClass : x_vecClasses) {
        if (bFirst)
            bFirst = false;
        else
            os << std::endl;
        upClass->Print(os, cIndent);
    }
}

}

#include <llvm/IR/Module.h>

#include "ClassDef.hpp"
#include "Program.hpp"

namespace udc::ast {

Program::Program(Driver &drv, const Location &loc, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept :
    NodeBase(drv, loc), x_vTyReg(drv), x_vecClasses(std::move(vecClasses)), x_pMain(nullptr), x_stClass(false)
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

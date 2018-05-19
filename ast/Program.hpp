#ifndef UDC_AST_PROGRAM_HPP_
#define UDC_AST_PROGRAM_HPP_

#include "NodeBase.hpp"
#include "eval/SymbolTable.hpp"
#include "eval/TypeRegistry.hpp"

namespace llvm {
class Module;
}

namespace udc::ast {

class Program : public NodeBase {
public:
    Program(Driver &drv, const Location &loc, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept;
    virtual ~Program();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr eval::TypeRegistry &GetTyReg() noexcept {
        return x_vTyReg;
    }

    constexpr const eval::TypeRegistry &GetTyReg() const noexcept {
        return x_vTyReg;
    }

    constexpr const std::vector<std::unique_ptr<ClassDef>> &GetClasses() const noexcept {
        return x_vecClasses;
    }

    constexpr eval::ClassTable &GetClassTable() noexcept {
        return x_stClass;
    }

    constexpr const eval::ClassTable &GetClassTable() const noexcept {
        return x_stClass;
    }

    constexpr std::unique_ptr<llvm::Module> &GetUplvMod() noexcept {
        return x_uplvMod;
    }

    inline llvm::Module *GetLvMod() const noexcept {
        return x_uplvMod.get();
    }

private:
    eval::TypeRegistry x_vTyReg;
    std::vector<std::unique_ptr<ClassDef>> x_vecClasses;
    eval::ClassTable x_stClass;
    std::unique_ptr<llvm::Module> x_uplvMod;
};

}

#endif

#ifndef UDC_AST_PROGRAM_HPP_
#define UDC_AST_PROGRAM_HPP_

#include "NodeBase.hpp"
#include "eval/SymbolTable.hpp"
#include "eval/Type.hpp"

namespace udc::ast {

class Program : public NodeBase {
public:
    Program(const Location &vLocation, std::vector<std::unique_ptr<ClassDef>> &&vecClasses) noexcept;
    virtual ~Program();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;
    
    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
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

private:
    std::vector<std::unique_ptr<ClassDef>> x_vecClasses;
    eval::ClassTable x_stClass;
};

}

#endif

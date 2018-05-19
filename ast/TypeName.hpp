#ifndef UDC_AST_TYPE_NAME_HPP_
#define UDC_AST_TYPE_NAME_HPP_

#include "NodeBase.hpp"

namespace udc::ast {

enum class TypeId {
    kVoid,
    kInt,
    kBool,
    kString,
    kClass
};

class TypeName : public NodeBase {
public:
    TypeName(Driver &drv, const Location &loc, TypeId vId) noexcept;
    TypeName(Driver &drv, const Location &loc, std::string &&sName) noexcept;
    virtual ~TypeName();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

    virtual inline void AcceptVisitor(eval::VisitorBase &vis) noexcept override {
        vis.Visit(*this);
    }

    constexpr void AddDimension() noexcept {
        ++x_cDimension;
    }

    constexpr TypeId GetId() const noexcept {
        return x_vId;
    }

    constexpr const std::string &GetName() const noexcept {
        return x_sName;
    }

    constexpr std::uint32_t GetDimension() const noexcept {
        return x_cDimension;
    }

private:
    TypeId x_vId;
    std::string x_sName;
    std::uint32_t x_cDimension;

};

}

#endif

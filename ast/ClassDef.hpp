#ifndef UDC_AST_CLASS_DEF_HPP_
#define UDC_AST_CLASS_DEF_HPP_

#include <optional>
#include <vector>

#include "Base.hpp"
#include "Interface.hpp"

namespace udc::ast {

class ClassDef : public Base {
public:
    ClassDef(
        const Location &vLocation,
        std::string &&sName,
        std::optional<std::string> &&soBase,
        std::vector<std::unique_ptr<IDefinition>> &&vecFields
    ) noexcept;
    virtual ~ClassDef();

    virtual void Print(std::ostream &os, std::uint32_t cIndent) const override;

private:
    std::string x_sName;
    std::optional<std::string> x_soBase;
    std::vector<std::unique_ptr<IDefinition>> x_vecFields;
};

}

#endif

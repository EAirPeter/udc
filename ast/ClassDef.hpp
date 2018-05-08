#ifndef UDC_AST_CLASS_DEF_HPP_
#define UDC_AST_CLASS_DEF_HPP_

#include "Node.hpp"

#include <optional>
#include <string>

namespace udc::ast {

class FieldDef;

class ClassDef : public virtual Node {
public:
    ClassDef(
        const Location &vLocation,
        std::string &&sName,
        std::optional<std::string> &&soBase,
        std::vector<std::unique_ptr<FieldDef>> &&vecFields
    ) noexcept;
    virtual ~ClassDef();
protected:
    virtual void Y_Print(std::ostream &os, uint32_t cIndent) const override;
private:
    std::string x_sName;
    std::optional<std::string> x_soBase;
    std::vector<std::unique_ptr<FieldDef>> x_vecFields;
};

}

#endif

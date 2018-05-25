#ifndef UDC_DRIVER_HPP_
#define UDC_DRIVER_HPP_

#include <memory>

namespace std::filesystem {
class path;
}

namespace llvm {
class ConstantPointerNull;
class Target;
class TargetMachine;
}

namespace udc::ast {
class Program;
}

namespace udc::cg {
class CGContext;
}

namespace udc {

struct Options {
    unsigned uOptimization;
    unsigned uFlags;
    constexpr static unsigned kCompile = 1 << 0;
    constexpr static unsigned kAssembly = 1 << 1;
    constexpr static unsigned kLLVM = 1 << 2;
    constexpr static unsigned kAST = 1 << 3;
    constexpr static unsigned kSymbol = 1 << 4;
    constexpr static unsigned kHelp = 1 << 5;
};

class Driver : private Options {
public:
    Driver(const Options &vOpts) noexcept;
    ~Driver();

public:
    int Run(const std::filesystem::path &paInput);
    void SetProgram(std::unique_ptr<ast::Program> &&upProg) noexcept;
    
private:
    std::unique_ptr<cg::CGContext> x_upCgCtx;
    std::unique_ptr<ast::Program> x_upProg;

};

}

#endif

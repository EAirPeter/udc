#include <iostream>

#include <llvm/Support/TargetSelect.h>

#include "Driver.hpp"

int main() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    try {
        udc::Driver vDriver;
        //vDriver.PrintSymbols();
        auto res = vDriver.Parse();
        std::cerr << "Result: " << res << std::endl;
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

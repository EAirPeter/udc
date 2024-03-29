#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <filesystem>
#include <iostream>

#include "Driver.hpp"
#include "Print.hpp"

int main(int ncArg, char *ppszArgs[]) {
    const char *pszInput = nullptr;
    udc::Options vOpts {};
    for (int i = 1; i < ncArg; ++i) {
        using namespace udc;
        auto pszArg = ppszArgs[i];
        auto pchOpt = std::strchr("OcSLath", pszArg[1]);
        if (pszArg[0] != '-' || !pchOpt) {
            if (pszInput) {
                std::cerr << "error: Unrecognized option " << (pszInput[0] == '-' ? pszInput : pszArg) << std::endl;
                std::cerr << " note: Type \"" << ppszArgs[0] << " -h\" for help" << std::endl;
                return -1;
            }
            pszInput = pszArg;
            continue;
        }
        auto chOpt = pszArg[1];
        if (pchOpt[1] != ':') {
            if (pszArg[2]) {
                std::cerr << "error: Unrecognized option " << pszArg << std::endl;
                std::cerr << " note: Type \"" << ppszArgs[0] << " -h\" for help" << std::endl;
                return -1;
            }
        }
        else {
            if (pszArg[2])
                pszArg += 2;
            else if (i + 1 < ncArg)
                pszArg = ppszArgs[++i];
            else {
                std::cerr << "error: Option -" << chOpt << " needs an argument" << std::endl;
                std::cerr << " note: Type \"" << ppszArgs[0] << " -h\" for help" << std::endl;
                return -1;
            }
        }
        switch (chOpt) {
        case 'O':
            vOpts.uFlags |= udc::Options::kOptimiz;
            break;
        case 'c':
            vOpts.uFlags |= udc::Options::kCompile;
            break;
        case 'S':
            vOpts.uFlags |= udc::Options::kAssembly;
            break;
        case 'L':
            vOpts.uFlags |= udc::Options::kLLVM;
            break;
        case 'a':
            vOpts.uFlags |= udc::Options::kAST;
            break;
        case 't':
            vOpts.uFlags |= udc::Options::kSymbol;
            break;
        case 'h':
            vOpts.uFlags |= udc::Options::kHelp;
            break;
        }
    }
    if (vOpts.uFlags & udc::Options::kHelp) {
        std::cout <<
            "UDC: A simple compiler for Decaf/Mind programming language\n"
            "\n"
            "Usage: " << ppszArgs[0] << " [Options] <DecafFile>\n"
            "\n"
            "Options:\n"
            "  -O    Enable optmization\n"
            "  -c    Emit object file (default)\n"
            "  -S    Emit assembly file\n"
            "  -L    Emit LLVM IR file\n"
            "  -a    Print AST for decaf file\n"
            "  -t    Print tokens for decaf file\n"
            "  -h    Print this help";
        std::cout << std::endl;
        return 0;
    }
    if (!pszInput) {
        std::cerr << "error: No decaf file given" << std::endl;
        std::cerr << " note: Type \"" << ppszArgs[0] << " -h\" for help" << std::endl;
        return -1;
    }
    if (!(vOpts.uFlags & ~udc::Options::kOptimiz))
        vOpts.uFlags |= udc::Options::kCompile;
    try {
        udc::Driver vDriver(vOpts);
        auto res = vDriver.Run(pszInput);
        std::clog << "Result: " << res << std::endl;
        return res;
    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
    return -1;
}

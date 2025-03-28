#ifndef PARSER_H
#define PARSER_H

#include <clang/AST/AST.h>
#include <vector>
#include <string>
#include <memory>

// Forward declaration
namespace clang {
    class CompilerInstance;
}

class Parser {
public:
    Parser(); // Add a constructor
    ~Parser(); // Add a destructor
    
    struct FunctionInfo {
        std::string name;
        std::string filename;
        unsigned line;
        bool hasBody;
    };
    
    static std::vector<FunctionInfo> parseFunctions(clang::ASTContext& Context);
    static bool isValidFunction(const clang::FunctionDecl* FD);
    
    // Remove the static version and keep only the instance method
    static clang::ASTContext* parseFile(const std::string& filePath);
    
private:
    static FunctionInfo extractFunctionInfo(const clang::FunctionDecl* FD);
    static std::unique_ptr<clang::CompilerInstance> compilerInstance;
};

#endif // PARSER_H
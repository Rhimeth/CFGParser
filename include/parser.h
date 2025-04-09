#ifndef PARSER_H
#define PARSER_H

<<<<<<< HEAD
#include <clang/Frontend/ASTUnit.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/AST/ASTConsumer.h>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace clang {
    class CompilerInstance;
    class ASTConsumer;
    class FunctionDecl;
    class CFG;
=======
#include <clang/AST/AST.h>
#include <vector>
#include <string>
#include <memory>

// Forward declaration
namespace clang {
    class CompilerInstance;
>>>>>>> e4e3a1ee3e7575d1f091a453a24f18f29459330b
}

class Parser {
public:
<<<<<<< HEAD
    // Forward declare ASTStoringConsumer first
    class ASTStoringConsumer;
    
    struct CFGNode {
        unsigned id;
        std::string label;
        unsigned line;
        std::string code;
    };

    struct CFGEdge {
        unsigned sourceId;
        unsigned targetId;
        std::string label;
    };

    struct FunctionCFG {
        std::string functionName;
        std::vector<CFGNode> nodes;
        std::vector<CFGEdge> edges;
    };

=======
    Parser(); // Add a constructor
    ~Parser(); // Add a destructor
    
>>>>>>> e4e3a1ee3e7575d1f091a453a24f18f29459330b
    struct FunctionInfo {
        std::string name;
        std::string filename;
        unsigned line;
        bool hasBody;
    };
<<<<<<< HEAD

    // Single definition of ThreadLocalState
    struct ThreadLocalState {
        ~ThreadLocalState();
        clang::ASTContext* parse(const std::string& filePath);
        void setupCompiler();
        
        std::unique_ptr<clang::CompilerInstance> compiler;
        std::unique_ptr<ASTStoringConsumer> consumer;
    };

    Parser();
    ~Parser();

    static std::unique_ptr<clang::ASTUnit> parseFileWithAST(const std::string& filename);
    static bool isDotFile(const std::string& filePath);
    std::vector<FunctionInfo> extractFunctions(const std::string& filePath);
    std::vector<FunctionCFG> extractAllCFGs(const std::string& filePath);
    std::string generateDOT(const FunctionCFG& cfg);

private:
    struct FunctionVisitor;
    static clang::ASTContext* parseFile(const std::string& filename);
};

// Define ASTStoringConsumer after Parser class definition
class Parser::ASTStoringConsumer : public clang::ASTConsumer {
public:
    clang::ASTContext* Context = nullptr;
    
    void HandleTranslationUnit(clang::ASTContext& Context) override {
        this->Context = &Context;
    }
=======
    
    static std::vector<FunctionInfo> parseFunctions(clang::ASTContext& Context);
    static bool isValidFunction(const clang::FunctionDecl* FD);
    
    // Remove the static version and keep only the instance method
    static clang::ASTContext* parseFile(const std::string& filePath);
    
private:
    static FunctionInfo extractFunctionInfo(const clang::FunctionDecl* FD);
    static std::unique_ptr<clang::CompilerInstance> compilerInstance;
>>>>>>> e4e3a1ee3e7575d1f091a453a24f18f29459330b
};

#endif // PARSER_H
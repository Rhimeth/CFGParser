#include "parser.h"
#include <clang/Parse/ParseAST.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendOptions.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/CommandLine.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class ASTExtractor : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    json outputJson;

    ASTExtractor() {
        outputJson["functions"] = json::array();
        outputJson["methods"] = json::array();
        outputJson["variables"] = json::array();
        outputJson["conditionals"] = json::array();
        outputJson["loops"] = json::array();
    }

    void run(const clang::ast_matchers::MatchFinder::MatchResult& Result) override {
        clang::SourceManager &SM = Result.Context->getSourceManager();

        // Function Extraction
        if (const auto *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDecl")) {
            if (!SM.isInSystemHeader(FD->getLocation())) {
                json funcJson;
                funcJson["name"] = FD->getNameAsString();
                funcJson["file"] = SM.getFilename(FD->getLocation()).str();
                funcJson["line"] = SM.getSpellingLineNumber(FD->getLocation());
                funcJson["returnType"] = FD->getReturnType().getAsString();
                outputJson["functions"].push_back(funcJson);
            }
        }

        // Class Method Extraction
        if (const auto *MD = Result.Nodes.getNodeAs<clang::CXXMethodDecl>("methodDecl")) {
            if (!SM.isInSystemHeader(MD->getLocation())) {
                json methodJson;
                methodJson["name"] = MD->getNameAsString();
                methodJson["file"] = SM.getFilename(MD->getLocation()).str();
                methodJson["line"] = SM.getSpellingLineNumber(MD->getLocation());
                methodJson["returnType"] = MD->getReturnType().getAsString();
                outputJson["methods"].push_back(methodJson);
            }
        }

        // Variable Extraction
        if (const auto *VD = Result.Nodes.getNodeAs<clang::VarDecl>("varDecl")) {
            if (!SM.isInSystemHeader(VD->getLocation())) {
                json varJson;
                varJson["name"] = VD->getNameAsString();
                varJson["file"] = SM.getFilename(VD->getLocation()).str();
                varJson["line"] = SM.getSpellingLineNumber(VD->getLocation());
                varJson["type"] = VD->getType().getAsString();
                outputJson["variables"].push_back(varJson);
            }
        }

        // If Statements
        if (const auto *IF = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
            json ifJson;
            ifJson["file"] = SM.getFilename(IF->getBeginLoc()).str();
            ifJson["line"] = SM.getSpellingLineNumber(IF->getBeginLoc());
            outputJson["conditionals"].push_back(ifJson);
        }

        // Loops
        if (const auto *FOR = Result.Nodes.getNodeAs<clang::ForStmt>("forStmt")) {
            json loopJson;
            loopJson["type"] = "for";
            loopJson["file"] = SM.getFilename(FOR->getBeginLoc()).str();
            loopJson["line"] = SM.getSpellingLineNumber(FOR->getBeginLoc());
            outputJson["loops"].push_back(loopJson);
        }

        if (const auto *WHILE = Result.Nodes.getNodeAs<clang::WhileStmt>("whileStmt")) {
            json loopJson;
            loopJson["type"] = "while";
            loopJson["file"] = SM.getFilename(WHILE->getBeginLoc()).str();
            loopJson["line"] = SM.getSpellingLineNumber(WHILE->getBeginLoc());
            outputJson["loops"].push_back(loopJson);
        }
    }

    void extractAST(clang::ASTContext& Context, const std::string& outputFilename) {
        clang::ast_matchers::MatchFinder Finder;
    
        // Register AST matchers
        Finder.addMatcher(clang::ast_matchers::functionDecl().bind("functionDecl"), this);
        Finder.addMatcher(clang::ast_matchers::cxxMethodDecl().bind("methodDecl"), this);
        Finder.addMatcher(clang::ast_matchers::varDecl().bind("varDecl"), this);
        Finder.addMatcher(clang::ast_matchers::ifStmt().bind("ifStmt"), this);
        Finder.addMatcher(clang::ast_matchers::forStmt().bind("forStmt"), this);
        Finder.addMatcher(clang::ast_matchers::whileStmt().bind("whileStmt"), this);
    
        // Run the matchers
        Finder.matchAST(Context);
    
        // Save extracted data
        this->saveToFile(outputFilename);
    }

    void saveToFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        outFile << outputJson.dump(4);
        outFile.close();
    }
};

std::unique_ptr<clang::CompilerInstance> Parser::compilerInstance;

clang::ASTContext* Parser::parseFile(const std::string& filePath) {
    std::cout << "DEBUG: Beginning to parse file: " << filePath << std::endl;
    
    // Create a new CompilerInstance
    compilerInstance = std::make_unique<clang::CompilerInstance>();
    auto& compiler = *compilerInstance;
    
    // Create diagnostics
    compiler.createDiagnostics();
    if (!compiler.hasDiagnostics()) {
        std::cerr << "DEBUG: Failed to create diagnostics" << std::endl;
        return nullptr;
    }
    
    // Check if the file exists
    if (!llvm::sys::fs::exists(filePath)) {
        std::cerr << "DEBUG: File does not exist: " << filePath << std::endl;
        return nullptr;
    }
    
    std::cout << "DEBUG: File exists, continuing..." << std::endl;
    
    // Create CompilerInvocation
    auto invocation = std::make_shared<clang::CompilerInvocation>();
    
    // Setup arguments - add more verbose flags for debugging
    std::vector<const char*> args;
    args.push_back("clang-tool");
    args.push_back("-x");
    args.push_back("c++");
    args.push_back("-std=c++17");
    args.push_back("-fsyntax-only");
    args.push_back("-I/usr/include");
    args.push_back("-I/usr/local/include");
    args.push_back("-v"); // Verbose output
    args.push_back(filePath.c_str());
    
    std::cout << "DEBUG: Creating compiler invocation with args:" << std::endl;
    for (const auto& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    
    // Create from args
    if (!clang::CompilerInvocation::CreateFromArgs(*invocation,
        llvm::ArrayRef<const char*>(args.data(), args.size()),
        compiler.getDiagnostics())) {
        std::cerr << "DEBUG: Failed to create compiler invocation" << std::endl;
        return nullptr;
    }
    
    std::cout << "DEBUG: Compiler invocation created successfully" << std::endl;
    compiler.setInvocation(invocation);
    
    // Create file and source managers
    compiler.createFileManager();
    compiler.createSourceManager(compiler.getFileManager());
    
    // Set up target options 
    compiler.setTarget(clang::TargetInfo::CreateTargetInfo(
        compiler.getDiagnostics(), compiler.getInvocation().TargetOpts));
    
    if (!compiler.hasTarget()) {
        std::cerr << "DEBUG: Failed to set target" << std::endl;
        return nullptr;
    }
    
    // Create preprocessor
    compiler.createPreprocessor(clang::TranslationUnitKind::TU_Complete);
    if (!compiler.hasPreprocessor()) {
        std::cerr << "DEBUG: Failed to create preprocessor" << std::endl;
        return nullptr;
    }
    
    // Create ASTContext
    compiler.createASTContext();
    if (!compiler.hasASTContext()) {
        std::cerr << "DEBUG: Failed to create ASTContext" << std::endl;
        return nullptr;
    }
    
    std::cout << "DEBUG: Successfully created ASTContext" << std::endl;
    
    clang::DiagnosticConsumer& client = compiler.getDiagnosticClient();
    
    clang::ParseAST(compiler.getPreprocessor(), &compiler.getASTConsumer(), compiler.getASTContext());
    
    client.BeginSourceFile(compiler.getLangOpts(), &compiler.getPreprocessor());
    client.EndSourceFile();
    
    std::cout << "DEBUG: Finished parsing AST" << std::endl;
    
    return &compiler.getASTContext();
}
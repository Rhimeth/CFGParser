#ifndef CFG_ANALYZER_H
#define CFG_ANALYZER_H

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <set>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>

namespace CFGAnalyzer {

class CFGVisitor : public clang::RecursiveASTVisitor<CFGVisitor> {
public:
    explicit CFGVisitor(clang::ASTContext* Context, const std::string& outputDir = "cfg_output");
    bool VisitFunctionDecl(clang::FunctionDecl* FD);
    bool VisitCallExpr(clang::CallExpr* CE);
    void FinalizeCombinedFile();
    
    // Function dependency analysis
    void PrintFunctionDependencies() const;
    std::unordered_map<std::string, std::set<std::string>> GetFunctionDependencies() const;

private:
    clang::ASTContext* Context;
    std::string CurrentFunction;  // Track which function we're currently in
    std::unordered_map<std::string, std::set<std::string>> FunctionDependencies;  // caller -> set of callees
    std::string OutputDir;  // Store output directory path
};

class CFGConsumer : public clang::ASTConsumer {
public:
    explicit CFGConsumer(clang::ASTContext* Context, const std::string& outputDir = "cfg_output");
    void HandleTranslationUnit(clang::ASTContext& Context) override;

private:
    std::unique_ptr<CFGVisitor> Visitor;
};

class CFGAction : public clang::ASTFrontendAction {
public:
    explicit CFGAction(const std::string& outputDir = "cfg_output");
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& CI, llvm::StringRef File) override;
        
private:
    std::string OutputDir;
};

bool analyze(const std::string& filename);
}

#endif
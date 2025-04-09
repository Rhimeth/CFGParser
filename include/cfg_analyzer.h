<<<<<<< HEAD
// cfg_analyzer.h
#ifndef CFG_ANALYZER_H
#define CFG_ANALYZER_H

#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <QString>
#include <QMutex>
#include <string>
#include <unordered_map>
#include <set>
#include <memory>

namespace CFGAnalyzer {

    struct AnalysisResult {
        std::string dotOutput;
        std::string jsonOutput;
        std::string report;
        bool success;
        std::unordered_map<std::string, std::set<std::string>> functionDependencies;
    };

    class CFGConsumer;  // Forward declaration
    class CFGAction;    // Forward declaration

    class CFGVisitor : public clang::RecursiveASTVisitor<CFGVisitor> {
    public:
        explicit CFGVisitor(clang::ASTContext* Context,
                         const std::string& outputDir,
                         AnalysisResult& results);
        
        bool VisitFunctionDecl(clang::FunctionDecl* FD);
        bool VisitCallExpr(clang::CallExpr* CE);
        void PrintFunctionDependencies() const;
        std::unordered_map<std::string, std::set<std::string>> GetFunctionDependencies() const;
        void FinalizeCombinedFile();
        
        AnalysisResult& getResults() { return m_results; }
        
    private:
        clang::ASTContext* Context;
        std::string OutputDir;
        std::string CurrentFunction;
        AnalysisResult& m_results;
        std::unordered_map<std::string, std::set<std::string>> FunctionDependencies;
    };

    class CFGConsumer : public clang::ASTConsumer {
    public:
        CFGConsumer(clang::ASTContext* Context,
                  const std::string& outputDir,
                  AnalysisResult& results);
        
        void HandleTranslationUnit(clang::ASTContext& Context) override;
        
    private:
        std::unique_ptr<CFGVisitor> Visitor;
    };

    class CFGAction : public clang::ASTFrontendAction {
    public:
        CFGAction(const std::string& outputDir,
                AnalysisResult& results);
        
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance& CI, llvm::StringRef File) override;
        
    private:
        std::string OutputDir;
        AnalysisResult& m_results;
    };

    class CFGAnalyzer {
    public:
        CFGAnalyzer() = default;
        ~CFGAnalyzer() = default;
    
        AnalysisResult analyzeFile(const QString& filePath);
        AnalysisResult analyze(const std::string& filename);
    
        void lock() { m_analysisMutex.lock(); }
        void unlock() { m_analysisMutex.unlock(); }
    
    private:
        std::string generateDotOutput(const AnalysisResult& result) const;
        std::string generateReport(const AnalysisResult& result) const;
        static std::string getCurrentDateTime();
        
        mutable QMutex m_analysisMutex;
        AnalysisResult m_results;
    };    
} // namespace CFGAnalyzer
#endif // CFG_ANALYZER_H
=======
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
>>>>>>> e4e3a1ee3e7575d1f091a453a24f18f29459330b

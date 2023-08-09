#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class FindIfBoundaryVisitor : public RecursiveASTVisitor<FindIfBoundaryVisitor> {
private:
    SourceManager &SourceMgr;
    const LangOptions &LangOpts;

public:
    FindIfBoundaryVisitor(SourceManager &SM, const LangOptions &LO)
        : SourceMgr(SM), LangOpts(LO) {}

    bool VisitIfStmt(IfStmt *ifStmt) {
        // TODO: Complete code.
        llvm::errs() << "Found an if statement:\n";

        Stmt *ifBody = ifStmt->getThen();
        printBoundary(ifBody);

        Stmt *elseBody = ifStmt->getElse();
        while (elseBody) {
            if (auto *elseIfStmt = dyn_cast<IfStmt>(elseBody)) {
                llvm::errs() << "Found an else if statement:\n";
                ifBody = elseIfStmt->getThen();
                printBoundary(ifBody);
                elseBody = elseIfStmt->getElse();
            } else {
                llvm::errs() << "Found an else statement:\n";
                printBoundary(elseBody);
                break;
            }
        }

        return true;
    }

    void printBoundary(Stmt *stmt) {
        SourceLocation startLoc = stmt->getBeginLoc();
        SourceLocation endLoc = stmt->getEndLoc();

        if (!startLoc.isValid() || !endLoc.isValid())
            return;

        SourceLocation expandedStartLoc = SourceMgr.getExpansionLoc(startLoc);
        SourceLocation expandedEndLoc = SourceMgr.getExpansionLoc(endLoc);

        if (!expandedStartLoc.isFileID() || !expandedEndLoc.isFileID())
            return;

        FileID startFileID = SourceMgr.getFileID(expandedStartLoc);
        FileID endFileID = SourceMgr.getFileID(expandedEndLoc);

        if (startFileID != endFileID)
            return;

        unsigned startOffset = SourceMgr.getFileOffset(expandedStartLoc);
        unsigned endOffset = SourceMgr.getFileOffset(expandedEndLoc);

        FileID fileID = startFileID;
        llvm::StringRef fileContent = SourceMgr.getBufferData(fileID);
        llvm::StringRef stmtSourceCode = fileContent.substr(startOffset, endOffset - startOffset);

        llvm::errs() << "Source Code:\n" << stmtSourceCode << "\n\n";
    }
};

class FindIfBoundaryConsumer : public ASTConsumer {
private:
    FindIfBoundaryVisitor visitor;

public:
    FindIfBoundaryConsumer(SourceManager &SM, const LangOptions &LO)
        : visitor(SM, LO) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class FindIfBoundaryAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override {
        CI.getPreprocessor().SetSuppressIncludeNotFoundError(true);
        return std::make_unique<FindIfBoundaryConsumer>(CI.getSourceManager(), CI.getLangOpts());
    }

    bool ParseArgs(const CompilerInstance &CI,
                const std::vector<std::string>& args) override {
    // for (unsigned i = 0, e = args.size(); i != e; ++i) {
    //     if (args[i] == "-some-arg") {
    //     // Handle the command line argument.
    //     }
    // }
    return true;
    }
};

static FrontendPluginRegistry::Add<FindIfBoundaryAction>
X("find-if-boundary", "Find `if` block when transforming AST to IR.");

// extern "C" void clang_registerFrontendPlugin(clang::FrontendPluginRegistry::AddPluginFn Add) {
//     Add(std::make_unique<FindIfBoundaryAction>());
// }

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
private:
    SourceManager &SourceMgr;
    const LangOptions &LangOpts;

public:
    MyASTVisitor(SourceManager &SM, const LangOptions &LO)
        : SourceMgr(SM), LangOpts(LO) {}

    bool VisitIfStmt(IfStmt *ifStmt) {
        llvm::errs() << "Found an if statement:\n";

        SourceLocation startLoc = ifStmt->getBeginLoc();
        SourceLocation endLoc = ifStmt->getEndLoc();

        if (!startLoc.isValid() || !endLoc.isValid())
            return true;

        SourceLocation expandedStartLoc = SourceMgr.getExpansionLoc(startLoc);
        SourceLocation expandedEndLoc = SourceMgr.getExpansionLoc(endLoc);

        const FileEntry *startFileEntry = SourceMgr.getFileEntryForID(SourceMgr.getFileID(startLoc));
        if (!startFileEntry)
            return true;
        llvm::StringRef startFilename = startFileEntry->getName();
        llvm::errs() << "File: " << startFilename << "\n";

        if (!expandedStartLoc.isFileID() || !expandedEndLoc.isFileID())
            return true;

        FileID startFileID = SourceMgr.getFileID(expandedStartLoc);
        FileID endFileID = SourceMgr.getFileID(expandedEndLoc);

        if (startFileID != endFileID)
            return true;

        unsigned startOffset = SourceMgr.getFileOffset(expandedStartLoc);
        unsigned endOffset = SourceMgr.getFileOffset(expandedEndLoc);

        FileID fileID = startFileID;
        llvm::StringRef fileContent = SourceMgr.getBufferData(fileID);
        llvm::StringRef ifSourceCode = fileContent.substr(startOffset, endOffset - startOffset);

        llvm::errs() << "Source Code:\n" << ifSourceCode << "\n\n";

        return true; // Continue traversal
    }
};

class MyASTConsumer : public ASTConsumer {
private:
    MyASTVisitor visitor;

public:
    MyASTConsumer(SourceManager &SM, const LangOptions &LO)
        : visitor(SM, LO) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class ViewIfASTPlugin : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override {
        CI.getPreprocessor().SetSuppressIncludeNotFoundError(true); // ignore header missing error.
        return std::make_unique<MyASTConsumer>(CI.getSourceManager(), CI.getLangOpts());
    }

    bool ParseArgs(const CompilerInstance &CI,
                const std::vector<std::string>& args) override {
        return true;
    }
};

static FrontendPluginRegistry::Add<ViewIfASTPlugin>
X("view-if-ast", "View the AST of if statements");

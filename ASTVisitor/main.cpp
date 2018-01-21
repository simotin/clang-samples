#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class FindNamedClassVisitor : public RecursiveASTVisitor<FindNamedClassVisitor> 
{
public:
  	explicit FindNamedClassVisitor(ASTContext *pContext) : _pContext(pContext) {}
  	bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
  	{
  		if (Declaration->getQualifiedNameAsString() == "n::m::C")
  		{
  			FullSourceLoc FullLocation = _pContext->getFullLoc(Declaration->getLocStart());
      		if (FullLocation.isValid())
  			{
  				llvm::outs() << "Found declaration at " << FullLocation.getSpellingLineNumber() << ":" << FullLocation.getSpellingColumnNumber() << "\n";
  			}
    }
    return true;
  }

private:
  ASTContext *_pContext;
};

class FindNamedClassConsumer : public clang::ASTConsumer
{
public:
  explicit FindNamedClassConsumer(ASTContext *Context) : _visitor(Context) {}
  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    _visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindNamedClassVisitor _visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction
{
public:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
	{
    	return std::unique_ptr<clang::ASTConsumer>(new FindNamedClassConsumer(&Compiler.getASTContext()));
	}
};

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		clang::tooling::runToolOnCode(new FindNamedClassAction, argv[1]);
	}

	return 0;
}

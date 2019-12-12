#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include "ast.h"

extern int yyparse();

// Parsed AST.
AST* ast = nullptr;

auto ctx = new Context();
void OnParsed() {
  if (!ast) return;
  // Interpreter
  auto expression = dynamic_cast<ExpressionAST*>(ast);
  if (expression) std::cout << "=>" << expression->Evaluate(ctx) << std::endl;
  auto statement = dynamic_cast<StatementAST*>(ast);
  if (statement) statement->Execute(ctx);
  // JsonTree
  std::cout << "Parsed Syntax Tree:" << std::endl
            << ast->JsonTree().dump(4) << std::endl;
  // IR
  auto ir = ast->GenIR(ctx);
  std::string ir_string;
  llvm::raw_string_ostream ofs(ir_string);
  ctx->llvm_module_.print(ofs, nullptr);
  std::cout << "Generated LLVM IR:" << std::endl << ir_string << std::endl;

  delete ast;
}

int main() {
  // Create a main function for interactive mode.
  ctx->blocks_.push(new BlockAST());
  std::vector<llvm::Type*> args;
  auto FT =
      llvm::FunctionType::get(llvm::Type::getVoidTy(ctx->llvm_context_), false);
  auto fun = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main",
                                    ctx->llvm_module_);
  auto bb = llvm::BasicBlock::Create(ctx->llvm_context_, "entry", fun);
  ctx->builder_.SetInsertPoint(bb);

  yyparse();
  return 0;
}

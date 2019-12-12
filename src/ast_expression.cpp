#include <string>
#include "ast.h"
#include "blc.tab.hpp"

double DoubleAST::Evaluate(Context* context) { return value_; }

nlohmann::json DoubleAST::JsonTree() {
  nlohmann::json json;
  json["type"] = "double";
  json["value"] = value_;
  return json;
}

double BinaryOperationAST::Evaluate(Context* context) {
  switch (type_) {
    case '+':
      return lhs_->Evaluate(context) + rhs_->Evaluate(context);
    case '-':
      return lhs_->Evaluate(context) - rhs_->Evaluate(context);
    case '*':
      return lhs_->Evaluate(context) * rhs_->Evaluate(context);
    case '/':
      return lhs_->Evaluate(context) / rhs_->Evaluate(context);
    case '>':
      return lhs_->Evaluate(context) > rhs_->Evaluate(context);
    case '<':
      return lhs_->Evaluate(context) < rhs_->Evaluate(context);
    case GEQ:
      return lhs_->Evaluate(context) >= rhs_->Evaluate(context);
    case LEQ:
      return lhs_->Evaluate(context) <= rhs_->Evaluate(context);
    case EQ:
      return lhs_->Evaluate(context) == rhs_->Evaluate(context);
    case NE:
      return lhs_->Evaluate(context) != rhs_->Evaluate(context);
    default:
      return 0;
  }
}

nlohmann::json BinaryOperationAST::JsonTree() {
  nlohmann::json json;
  json["type"] = "BinaryOperation";
  json["lhs"] = lhs_->JsonTree();
  json["rhs"] = rhs_->JsonTree();
  switch (type_) {
    case GEQ:
      json["operationType"] = ">=";
      break;
    case LEQ:
      json["operationType"] = "<=";
      break;
    case EQ:
      json["operationType"] = "==";
      break;
    case NE:
      json["operationType"] = "!=";
      break;
    default:
      json["operationType"] = std::string(1, type_);
  }
  return json;
}

double IdentifierAST::Evaluate(Context* context) {
  auto symbol = context->blocks_.top()->get_symbol(name_);
  try {
    return std::get<double>(symbol);
  } catch (...) {
    return std::get<ExpressionAST*>(symbol)->Evaluate(context);
  }
}

nlohmann::json IdentifierAST::JsonTree() {
  nlohmann::json json;
  json["type"] = "Identifier";
  json["name"] = name_;
  return json;
}

double VariableAssignmentAST::Evaluate(Context* context) {
  auto value = value_->Evaluate(context);
  context->blocks_.top()->set_symbol(name_->get_name(),
                                     BlockAST::SymbolType(value));
  return value;
}

nlohmann::json VariableAssignmentAST::JsonTree() {
  nlohmann::json json;
  json["type"] = "VariableAssignment";
  json["identifier"] = name_->JsonTree();
  json["value"] = value_->JsonTree();
  return json;
}

double ExpressionAssignmentAST::Evaluate(Context* context) {
  context->blocks_.top()->set_symbol(name_->get_name(),
                                     BlockAST::SymbolType(value_));
  return value_->Evaluate(context);
}

nlohmann::json ExpressionAssignmentAST::JsonTree() {
  nlohmann::json json;
  json["type"] = "ExpressionAssignment";
  json["identifier"] = name_->JsonTree();
  json["value"] = value_->JsonTree();
  return json;
}
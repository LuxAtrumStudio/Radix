#include "expression.hpp"

#include <iostream>

#include "function/function_base.hpp"
#include "value/value.hpp"

radix::Expression::Expression() {}
radix::Expression::Expression(const ExpressionType& type) : type_(type) {}
radix::Expression::Expression(const Expression& copy)
    : type_(copy.type_),
      next_(copy.next_),
      prev_(copy.prev_),
      children_(copy.children_) {}
radix::Expression::~Expression() {}

std::shared_ptr<radix::Expression> radix::Expression::Eval() const {
  std::cout << "EVALUATING...\n";
  Expression exp(*this);
  std::cout << exp.Tree() << "\n";
  return NULL;
}

std::string radix::Expression::Latex(bool recurse) const {
  return std::string();
}
std::string radix::Expression::Tree(std::size_t indent) const {
  std::string ret = "Exp[NULL]()";
  std::string rep = "\u2502" + std::string(indent, ' ');
  std::string bar;
  for (std::size_t i = 0; i < indent; i++) {
    bar += "\u2500";
  }
  if (children_.size() != 0) {
    ret += "\n";
  }
  for (auto it = children_.begin(); it != children_.end(); ++it) {
    if (it != children_.end() - 1) {
      ret += "\u251c" + bar;
    } else {
      ret += "\u2514" + bar;
      rep = std::string(indent + 1, ' ');
    }
    std::string sub = (*it)->Tree(indent);
    size_t pos = 0;
    while ((pos = sub.find('\n', pos)) != std::string::npos) {
      sub.insert(++pos, rep);
      pos += rep.length();
    }
    ret += sub;
    if (it != children_.end() - 1) {
      ret += "\n";
    }
  }
  return ret;
}

void radix::Expression::insert(std::size_t pos,
                               std::shared_ptr<Expression> child) {
  children_.insert(children_.begin() + pos, child);
  if (*(children_.begin()) != child) {
    children_[pos - 1]->next_ = child;
  }
  if (*(children_.end() - 1) != child) {
    children_[pos + 1]->prev_ = child;
  }
}
std::shared_ptr<radix::Expression>& radix::Expression::at(std::size_t pos){
  return children_.at(pos);
}

void radix::Expression::append(std::shared_ptr<Expression> child) {
  children_.push_back(child);
  if (*(children_.begin()) != child) {
    (*(children_.end() - 1))->next_ = child;
  }
}

void radix::Expression::prepend(std::shared_ptr<Expression> child) {
  children_.insert(children_.begin(), child);
  if (*(children_.end() - 1) != child) {
    (*(children_.begin() + 1))->prev_ = child;
  }
}

void radix::Expression::clear() { children_.clear(); }

std::vector<std::shared_ptr<radix::Expression>>::iterator
radix::Expression::begin() {
  return children_.begin();
}
std::vector<std::shared_ptr<radix::Expression>>::iterator
radix::Expression::end() {
  return children_.end();
}
std::shared_ptr<radix::Expression>& radix::Expression::front() {
  return children_.front();
}
std::shared_ptr<radix::Expression>& radix::Expression::back() {
  return children_.back();
}

std::shared_ptr<radix::Expression> radix::GenerateExpression(
    std::string_view str) {
  return std::make_shared<Expression>(Expression());
}

std::shared_ptr<radix::Expression> radix::CopyExpression(
    std::shared_ptr<Expression> exp) {
  std::shared_ptr<Expression> ret = std::make_shared<Expression>(Expression());
  if (exp->type_ == VALUE) {
    ret = CopyValue(exp);
  } else if (exp->type_ == FUNCTION) {
    ret = CopyFunction(exp);
  }
  ret->type_ = exp->type_;
  if (exp->next_) {
    ret->next_ = std::make_shared<Expression>(*(exp->next_));
  }
  if (exp->prev_) {
    ret->prev_ = std::make_shared<Expression>(*(exp->prev_));
  }
  for (auto& it : exp->children_) {
    ret->append(CopyExpression(it));
  }
  return ret;
}

#pragma once

#include <string>

struct Objc3GenericParamDecl {
  std::string name;
  std::string variance_spelling;
  bool has_constraint = false;
  std::string constraint_type_name;
  bool has_constraint_generic_suffix = false;
  bool constraint_generic_suffix_terminated = true;
  std::string constraint_generic_suffix_text;
  unsigned line = 1;
  unsigned column = 1;
  unsigned constraint_line = 1;
  unsigned constraint_column = 1;
};

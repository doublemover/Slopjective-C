bool IsPointerArithmeticMutationOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "++" || op == "--";
}

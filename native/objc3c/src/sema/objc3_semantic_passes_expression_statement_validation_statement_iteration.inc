static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               int defer_loop_base_depth, int defer_switch_base_depth,
                               bool inside_defer_body,
                               std::size_t max_message_send_args,
                               const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatement(
    const Stmt *stmt, std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type,
    const std::string &function_name, std::vector<std::string> &diagnostics,
    int loop_depth, int switch_depth, int defer_loop_base_depth,
    int defer_switch_base_depth, bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatementsFromIndex(
    const std::vector<std::unique_ptr<Stmt>> &statements, std::size_t start_index,
    std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type, const std::string &function_name,
    std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
    int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context) {
  for (std::size_t index = start_index; index < statements.size(); ++index) {
    ValidateStatement(statements[index].get(), scopes, globals, functions,
                      expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, defer_loop_base_depth,
                      defer_switch_base_depth, inside_defer_body,
                      max_message_send_args,
                      message_send_context);
  }
}

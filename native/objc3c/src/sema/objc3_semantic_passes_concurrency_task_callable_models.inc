struct Objc3MessageSendResolutionContext {
  const Objc3SemanticIntegrationSurface *surface = nullptr;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      *protocol_definitions = nullptr;
  std::string current_implementation_name;
  std::string current_super_name;
  bool inside_method = false;
  bool is_class_method = false;
  bool inside_async_context = false;
};

struct Objc3ConcurrencyTaskCallableLegalityProfile {
  std::size_t task_creation_sites = 0;
  std::size_t detached_task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
};

struct Objc3ResolvedMessageSendMethod {
  enum class Status {
    RuntimeDynamic,
    Resolved,
    MissingConcreteSelector,
    AmbiguousConcreteSelector
  };

  Status status = Status::RuntimeDynamic;
  const Objc3MethodInfo *method = nullptr;
  std::string owner_name;
  bool is_class_method = false;
  bool missing_base = false;
  bool cycle_detected = false;
};

struct Objc3ResolvedMessageSendPropertyAccessor {
  const Objc3PropertyInfo *property = nullptr;
  bool is_setter = false;
};

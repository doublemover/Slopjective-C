enum class Objc3ProtocolRequirementClosureStatus {
  Ready,
  MissingProtocol,
  ForwardDeclaration,
  InheritanceCycle,
  IncompatibleInheritedRequirement,
};

struct Objc3ProtocolRequirementClosure {
  Objc3ProtocolRequirementClosureStatus status =
      Objc3ProtocolRequirementClosureStatus::Ready;
  std::string failing_protocol_name;
  std::string conflicting_member_name;
  bool conflicting_member_is_method = false;
  bool conflicting_selector_is_class_method = false;
  unsigned conflicting_member_line = 1;
  unsigned conflicting_member_column = 1;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      required_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      optional_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      required_methods_by_key;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      optional_methods_by_key;
};

struct Objc3ProtocolQualifiedMessageRequirementResolution {
  enum class Status {
    NotProtocolQualified,
    MissingRequirementClosure,
    MissingSelector,
    AmbiguousSelector,
    Resolved,
  };

  Status status = Status::NotProtocolQualified;
  const Objc3ProtocolRequirementInfo *requirement = nullptr;
  const Objc3ProtocolPropertyRequirementInfo *property_requirement = nullptr;
  bool property_requirement_is_setter = false;
  std::string protocol_name;
  std::string failing_protocol_name;
};

static bool CollectProtocolRequirementClosure(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    const std::string &protocol_name,
    std::unordered_set<std::string> &active_protocols,
    Objc3ProtocolRequirementClosure &closure);

static Objc3ProtocolQualifiedMessageRequirementResolution
ResolveProtocolQualifiedMessageRequirement(
    const Objc3SemanticCanonicalType &receiver_type,
    const std::string &selector,
    bool is_class_method,
    const Objc3MessageSendResolutionContext &context);

static std::string BuildProtocolRequirementKey(const std::string &selector,
                                               bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string FormatMethodSelectorForDiagnostic(const std::string &selector,
                                                     bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string BuildCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

struct Objc3CategoryMergeOrderEntry {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string container_name;
  unsigned line = 1;
  unsigned column = 1;
};

static std::string FormatObjcContainerName(const std::string &name,
                                           bool has_category,
                                           const std::string &category_name) {
  return has_category ? name + "(" + category_name + ")" : name;
}

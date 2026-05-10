struct Objc3ProtocolRequirementInfo {
  std::string selector;
  Objc3MethodInfo method;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolPropertyRequirementInfo {
  std::string property_name;
  Objc3PropertyInfo property;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolSemanticDefinition {
  std::string name;
  bool is_forward_declaration = false;
  std::vector<std::string> inherited_protocols_lexicographic;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      required_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      optional_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      required_methods_by_key;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      optional_methods_by_key;
};

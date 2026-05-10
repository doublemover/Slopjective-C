struct Objc3FrontendTypeSystemCanonicalTypeRecord {
  std::vector<std::string> generic_arguments_source_order;
};

struct Objc3FrontendTypeSystemPropertyRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord canonical_type;
};

struct Objc3FrontendTypeSystemMethodRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord return_canonical_type;
  std::vector<Objc3FrontendTypeSystemCanonicalTypeRecord>
      param_canonical_types;
};

struct Objc3FrontendTypeSystemFunctionRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord return_canonical_type;
  std::vector<Objc3FrontendTypeSystemCanonicalTypeRecord>
      param_canonical_types;
};

struct Objc3FrontendTypeSystemInterfaceRecord {
  std::string name;
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::vector<Objc3FrontendTypeSystemPropertyRecord> properties_lexicographic;
  std::vector<Objc3FrontendTypeSystemMethodRecord> methods_lexicographic;
};

struct Objc3FrontendTypeSystemImplementationRecord {
  std::vector<Objc3FrontendTypeSystemPropertyRecord> properties_lexicographic;
  std::vector<Objc3FrontendTypeSystemMethodRecord> methods_lexicographic;
};

struct Objc3FrontendTypeSystemSemanticMetadataRecord {
  std::vector<Objc3FrontendTypeSystemFunctionRecord> functions_lexicographic;
  std::vector<Objc3FrontendTypeSystemInterfaceRecord> interfaces_lexicographic;
  std::vector<Objc3FrontendTypeSystemImplementationRecord>
      implementations_lexicographic;
  bool deterministic = true;
};

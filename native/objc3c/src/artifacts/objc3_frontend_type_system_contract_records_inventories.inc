struct Objc3FrontendTypeSystemGenericContractInventory {
  std::size_t interface_count = 0;
  std::size_t generic_interface_count = 0;
  std::size_t generic_parameter_count = 0;
  std::size_t generic_variance_annotation_count = 0;
  std::size_t generic_argument_reference_count = 0;
  std::size_t protocol_qualified_generic_argument_count = 0;
};

struct Objc3FrontendTypeSystemProtocolContractInventory {
  std::size_t protocol_decl_count = 0;
  std::size_t protocol_forward_declaration_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t class_protocol_adoption_count = 0;
  std::size_t category_protocol_adoption_count = 0;
};

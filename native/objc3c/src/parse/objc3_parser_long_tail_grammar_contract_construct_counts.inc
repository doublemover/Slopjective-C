inline std::size_t BuildObjc3LongTailGrammarConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_property_decl_count + snapshot.protocol_method_decl_count +
         snapshot.interface_property_decl_count + snapshot.interface_method_decl_count +
         snapshot.implementation_property_decl_count + snapshot.implementation_method_decl_count +
         snapshot.interface_category_decl_count + snapshot.implementation_category_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

inline std::size_t BuildObjc3LongTailGrammarCoveredConstructCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.protocol_class_method_decl_count + snapshot.protocol_instance_method_decl_count +
         snapshot.interface_class_method_decl_count + snapshot.interface_instance_method_decl_count +
         snapshot.implementation_class_method_decl_count + snapshot.implementation_instance_method_decl_count +
         snapshot.function_prototype_count + snapshot.function_pure_count;
}

  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::vector<std::string>>
      generic_parameter_constraints_lexicographic;
  std::string generic_callable_signature_replay_key;
  std::string generic_callable_reification_policy = "erased_default";
  std::string generic_callable_mangling_policy_id =
      "objc3c.generic-callable.semantic-mangling.v1";
  bool generic_callable_contract_deterministic = true;

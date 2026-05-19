  void ParseParameterTypeSuffix(FuncParam &param) {
    bool parsed_generic_suffix = false;
    while (true) {
      bool suffix_failed = false;
      if (ParseParameterGenericTypeSuffix(param, parsed_generic_suffix, suffix_failed)) {
        if (suffix_failed) {
          return;
        }
        continue;
      }

      if (ParseParameterPointerTypeSuffix(param)) {
        continue;
      }

      if (ParseParameterNullabilityTypeSuffix(param)) {
        continue;
      }

      if (ParseParameterOwnershipQualifierTypeSuffix(param)) {
        continue;
      }

      break;
    }
  }

    if (!parameter_type_spelling_matched && AtReservedValueOptionalType()) {
      return RejectReservedValueOptionalType(Peek());
    }


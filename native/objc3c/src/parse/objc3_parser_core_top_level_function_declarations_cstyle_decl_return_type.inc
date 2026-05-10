  bool ParseTopLevelCStyleFunctionReturnType(FunctionDecl &fn) {
    const Token &return_type_token = Peek();
    const Objc3CStyleFunctionTypeParseResult parsed_return_type =
        ParseObjc3CStyleFunctionType(tokens_, index_, true,
                                     return_type_token.line,
                                     return_type_token.column,
                                     "function return", diagnostics_);
    if (!parsed_return_type.ok) {
      return false;
    }

    fn.return_type = parsed_return_type.type;
    fn.return_id_spelling = parsed_return_type.id_spelling;
    fn.return_class_spelling = parsed_return_type.class_spelling;
    fn.return_sel_spelling = parsed_return_type.sel_spelling;
    fn.return_instancetype_spelling = parsed_return_type.instancetype_spelling;
    fn.return_object_pointer_type_spelling =
        parsed_return_type.object_pointer_spelling;
    fn.return_object_pointer_type_name =
        parsed_return_type.object_pointer_name;
    fn.has_return_pointer_declarator = parsed_return_type.pointer_depth > 0u;
    fn.return_pointer_declarator_depth = parsed_return_type.pointer_depth;
    fn.return_typecheck_family_symbol = BuildObjcTypecheckReturnFamilySymbol(fn);
    return true;
  }

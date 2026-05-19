namespace {

std::vector<std::string> CollectInteropLocalImportModuleNames(
    const Objc3Program &program) {
  std::vector<std::string> names;
  const auto accumulate_callable = [&names](const auto &decl) {
    if (decl.objc_import_module_declared &&
        !decl.objc_import_module_name.empty()) {
      names.push_back(decl.objc_import_module_name);
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  std::sort(names.begin(), names.end());
  names.erase(std::unique(names.begin(), names.end()), names.end());
  return names;
}

}  // namespace

void EmitObjc3IRRuntimeBootstrapLoweringFunctions(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::ostringstream &out) {
  if (!Objc3IRRuntimeBootstrapLoweringReady(frontend_metadata)) {
    return;
  }

  const std::string init_stub_symbol =
      "@" + runtime_metadata_symbols.init_stub_symbol;
  const std::string ctor_root_symbol =
      "@" + frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol;
  const std::string registration_table_symbol =
      "@" + runtime_metadata_symbols.registration_table_symbol;
  const std::string register_image_symbol =
      "@" + frontend_metadata
                .runtime_bootstrap_lowering_registration_entrypoint_symbol;

  out << "define internal void " << init_stub_symbol << "() {\n";
  out << "entry:\n";
  out << "  %bootstrap_state_slot = getelementptr inbounds "
      << Objc3IRRuntimeBootstrapRegistrationTableType() << ", ptr "
      << registration_table_symbol << ", i32 0, i32 13\n";
  out << "  %bootstrap_state_cell = load ptr, ptr %bootstrap_state_slot, align 8\n";
  out << "  %bootstrap_state = load i8, ptr %bootstrap_state_cell, align 1\n";
  out << "  %bootstrap_already_initialized = icmp ne i8 %bootstrap_state, 0\n";
  out << "  br i1 %bootstrap_already_initialized, label %bootstrap_success, label %bootstrap_register\n";
  out << "bootstrap_register:\n";
  out << "  call void @" << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
      << "(ptr " << registration_table_symbol << ")\n";
  out << "  %bootstrap_image_slot = getelementptr inbounds "
      << Objc3IRRuntimeBootstrapRegistrationTableType() << ", ptr "
      << registration_table_symbol << ", i32 0, i32 2\n";
  out << "  %bootstrap_image = load ptr, ptr %bootstrap_image_slot, align 8\n";
  out << "  %bootstrap_status = call i32 " << register_image_symbol
      << "(ptr %bootstrap_image)\n";
  out << "  %bootstrap_ok = icmp eq i32 %bootstrap_status, 0\n";
  out << "  br i1 %bootstrap_ok, label %bootstrap_success, label %bootstrap_fail\n";
  out << "bootstrap_fail:\n";
  out << "  call void @abort()\n";
  out << "  unreachable\n";
  out << "bootstrap_success:\n";
  out << "  store i8 1, ptr %bootstrap_state_cell, align 1\n";
  out << "  ret void\n";
  out << "}\n\n";

  out << "define internal void " << ctor_root_symbol << "() {\n";
  out << "entry:\n";
  out << "  call void " << init_stub_symbol << "()\n";
  out << "  ret void\n";
  out << "}\n\n";
}

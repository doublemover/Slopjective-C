void AppendObjc3FrontendArtifactManifestRecordArrays(
    std::ostream &manifest,
    const Objc3Program &program,
    const std::vector<int> &resolved_global_values,
    const std::vector<const FunctionDecl *> &manifest_functions,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3RuntimeMetadataSourceRecordSet
        &runtime_metadata_source_records) {
  manifest << "  \"semantic_canonical_type_metadata\":";
  objc3::artifacts::json::WriteSemanticTypeMetadataHandoffManifestObject(
      manifest, type_metadata_handoff);
  manifest << ",\n";
  manifest << "  \"globals\": ";
  objc3::artifacts::json::WriteProgramGlobalsManifestArray(
      manifest, program.globals, resolved_global_values);
  manifest << ",\n";
  manifest << "  \"functions\": ";
  objc3::artifacts::json::WriteFunctionDeclarationsManifestArray(
      manifest, manifest_functions);
  manifest << ",\n";
  manifest << "  \"interfaces\": ";
  objc3::artifacts::json::WriteRuntimeMetadataInterfaceManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"implementations\": ";
  objc3::artifacts::json::WriteRuntimeMetadataImplementationManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"protocols\": ";
  objc3::artifacts::json::WriteRuntimeMetadataProtocolManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"categories\": ";
  objc3::artifacts::json::WriteRuntimeMetadataCategoryManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"runtime_metadata_source_records\": ";
  objc3::artifacts::json::WriteRuntimeMetadataSourceRecordSetManifestObject(
      manifest, runtime_metadata_source_records);
  manifest << "\n";
}

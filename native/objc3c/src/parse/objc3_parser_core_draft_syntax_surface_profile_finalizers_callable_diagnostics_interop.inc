  template <typename TCallableDecl>
  void CountDraftSyntaxCallableInteropDiagnostics(
      const TCallableDecl &decl,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    if (decl.objc_foreign_declared || decl.objc_import_module_declared ||
        decl.objc_swift_name_declared || decl.objc_swift_private_declared ||
        decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
        decl.objc_export_header_declared || decl.objc_abi_align_declared ||
        decl.objc_foreign_type_declared || decl.objc_mixed_image_declared ||
        decl.objc_package_entry_declared ||
        decl.objc_nserror_declared || decl.objc_status_code_declared) {
      ++summary.interop_attribute_sites;
    }
    if (decl.objc_export_header_declared) {
      ++summary.interop_header_export_sites;
    }
    if (decl.objc_abi_align_declared) {
      ++summary.interop_abi_alignment_sites;
    }
    if (decl.objc_foreign_type_declared) {
      ++summary.interop_foreign_type_sites;
    }
    if (decl.objc_mixed_image_declared) {
      ++summary.interop_mixed_image_sites;
    }
    if (decl.objc_package_entry_declared) {
      ++summary.interop_package_entry_sites;
    }
    if (decl.objc_import_module_declared) {
      ++summary.interop_import_module_sites;
    }
    if (decl.objc_swift_name_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_swift_private_declared) {
      ++summary.interop_swift_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.interop_cxx_annotation_sites;
    }
    if (decl.objc_header_name_declared) {
      ++summary.interop_header_import_sites;
    }
    summary.interop_error_bridge_sites +=
        decl.objc_nserror_attribute_sites + decl.objc_status_code_attribute_sites;
  }

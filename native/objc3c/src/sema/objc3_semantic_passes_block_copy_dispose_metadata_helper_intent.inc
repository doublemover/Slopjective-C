struct Objc3BlockCopyDisposeHelperIntent {
  bool copy_helper_required = false;
  bool dispose_helper_required = false;
  bool copy_dispose_profile_is_normalized = false;
  bool ownership_helper_eligibility_is_normalized = false;
};

static Objc3BlockCopyDisposeHelperIntent ClassifyBlockCopyDisposeHelperIntent(
    const Expr &expr,
    const Objc3BlockCopyDisposeSiteMetadata &metadata) {
  Objc3BlockCopyDisposeHelperIntent intent;
  intent.copy_helper_required =
      expr.block_runtime_capture_ownership_is_normalized
          ? expr.block_runtime_copy_helper_required
          : metadata.byref_slot_count > 0u;
  intent.dispose_helper_required =
      expr.block_runtime_capture_ownership_is_normalized
          ? expr.block_runtime_dispose_helper_required
          : metadata.byref_slot_count > 0u ||
                metadata.explicit_move_capture_count > 0u;
  intent.copy_dispose_profile_is_normalized =
      expr.block_source_storage_annotations_are_normalized &&
      (!expr.block_runtime_capture_ownership_is_normalized ||
       !expr.block_runtime_capture_ownership_profile.empty());
  intent.ownership_helper_eligibility_is_normalized =
      expr.block_runtime_capture_ownership_is_normalized;
  return intent;
}

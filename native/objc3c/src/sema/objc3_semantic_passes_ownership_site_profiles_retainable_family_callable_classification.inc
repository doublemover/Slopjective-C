template <typename TCallableDecl>
static void CollectOwnershipRetainableFamilyCallableSites(
    const TCallableDecl &decl,
    OwnershipCaptureListRetainableFamilySiteProfile &profile) {
  bool has_callable_attributes = false;
  bool has_canonical_operation = false;
  bool has_compatibility_alias = false;
  for (const auto &attribute_name :
       decl.retainable_c_family_callable_attributes) {
    has_callable_attributes = true;
    has_canonical_operation =
        has_canonical_operation ||
        IsRetainableFamilyCanonicalOperationAttribute(attribute_name);
    has_compatibility_alias =
        has_compatibility_alias ||
        IsRetainableFamilyReturnAliasAttribute(attribute_name) ||
        IsRetainableFamilyConsumedAliasAttribute(attribute_name);
  }
  if (has_callable_attributes) {
    ++profile.retainable_family_callable_sites;
  }
  if (has_canonical_operation) {
    ++profile.retainable_family_operation_callable_sites;
  }
  if (has_compatibility_alias) {
    ++profile.retainable_family_alias_callable_sites;
  }
}

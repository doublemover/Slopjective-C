struct OwnershipBorrowedEscapeSiteProfile {
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
};

struct OwnershipCaptureListRetainableFamilySiteProfile {
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_ownership_mode_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
};

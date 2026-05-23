$ErrorActionPreference = "Stop"

function Get-CoreRecoveryDispatchRemainderCaseDefinitions {
  $cacheAwareDispatchLlTokens = @(
    "declare { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(",
    "call { i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr } @objc3_runtime_cache_aware_dispatch_i32_checked(",
    "declare i32 @objc3_runtime_prepare_cache_aware_dispatch_descriptor(",
    "call i32 @objc3_runtime_prepare_cache_aware_dispatch_descriptor(",
    "define i32 @objc3c_entry"
  )

  return @(
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_call_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_call_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = $cacheAwareDispatchLlTokens
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_extern_call_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_extern_call_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = $cacheAwareDispatchLlTokens
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_mutable_global_identifier_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_mutable_global_identifier_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = $cacheAwareDispatchLlTokens
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_invalidation_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_bound_identifier_invalidation_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = $cacheAwareDispatchLlTokens
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_zero_receiver_non_elided.objc3"
      CaseName = "objc3_dispatch_numeric_zero_receiver_non_elided"
      RequireLl = $true
      RequiredLlTokens = $cacheAwareDispatchLlTokens
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    }
  )
}

Export-ModuleMember -Function "Get-CoreRecoveryDispatchRemainderCaseDefinitions"

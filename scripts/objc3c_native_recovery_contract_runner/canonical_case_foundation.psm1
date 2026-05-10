$ErrorActionPreference = "Stop"

function Get-CoreRecoveryFoundationCaseDefinitions {
  return @(
    @{
      Source = "tests/tooling/fixtures/native/hello.m"
      CaseName = "objc_baseline"
    },
    @{
      Source = "tests/tooling/fixtures/native/hello.objc3"
      CaseName = "objc3_frontend"
      RequireLl = $true
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/hello.objc3"
      CaseName = "objc3_frontend_wrapper_launch_contract"
      RequireLl = $true
      RequireCompileProvenance = $true
      RequireObjc3ManifestSurface = $true
      UseCompileWrapper = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/function_return_annotation_bool.objc3"
      CaseName = "objc3_typed_signature_bool_return"
      RequireLl = $true
      RequiredLlTokens = @("define i1 @is_zero(i32 %arg0)", "call i1 @is_zero(i32 0)", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"name":"is_zero"', '"return":"bool"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/typed_i32_bool.objc3"
      CaseName = "objc3_typed_signature_mixed_abi"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @select_i32(i32 %arg0, i32 %arg1, i1 %arg2)", "zext i1 %arg2 to i32", "call i32 @select_i32(i32", "define i32 @objc3c_entry")
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/dispatch/message_send_six_args.objc3"
      CaseName = "objc3_dispatch_surface_custom_symbol_argslots"
      RequireLl = $true
      ExtraArgs = @("--objc3-runtime-dispatch-symbol", "objc3_runtime_dispatch_lane_c_surface", "--objc3-max-message-args", "6")
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_lane_c_surface(i32, ptr, i32, i32, i32, i32, i32, i32)", "call i32 @objc3_runtime_dispatch_lane_c_surface(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_symbol":"objc3_runtime_dispatch_lane_c_surface"', '"runtime_dispatch_arg_slots":6', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    }
  )
}

Export-ModuleMember -Function "Get-CoreRecoveryFoundationCaseDefinitions"

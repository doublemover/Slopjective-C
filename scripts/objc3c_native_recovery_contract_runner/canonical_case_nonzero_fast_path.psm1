$ErrorActionPreference = "Stop"

function Get-CoreRecoveryNonzeroFastPathCaseDefinitions {
  return @(
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_nonzero_receiver_fast_path.objc3"
      CaseName = "objc3_dispatch_numeric_nonzero_receiver_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_bound_identifier_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_constant_expression_receiver_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_constant_expression_receiver_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_const_expr_identifier_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_const_expr_identifier_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_call_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_pure_call_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_pure_prototype_call_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_pure_prototype_call_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_immutable_global_identifier_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_immutable_global_identifier_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_unary_receiver_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_unary_receiver_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_short_circuit_receiver_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_short_circuit_receiver_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_const_expr_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_bound_identifier_const_expr_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_post_assignment_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_bound_identifier_post_assignment_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("icmp eq i32", "msg_nil_", "msg_dispatch_", " = phi i32 [0, %msg_nil_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    }
  )
}

Export-ModuleMember -Function "Get-CoreRecoveryNonzeroFastPathCaseDefinitions"

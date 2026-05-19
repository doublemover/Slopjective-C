$ErrorActionPreference = "Stop"

function Get-CoreRecoveryNilReceiverCaseDefinitions {
  return @(
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_short_circuit.objc3"
      CaseName = "objc3_dispatch_nil_receiver_short_circuit"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_unary_short_circuit.objc3"
      CaseName = "objc3_dispatch_nil_receiver_unary_short_circuit"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_zero_result_positive.objc3"
      CaseName = "objc3_dispatch_nil_receiver_zero_result"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "icmp eq i32", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_mixed_expression_flow.objc3"
      CaseName = "objc3_dispatch_nil_receiver_mixed_expression_flow"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "icmp eq i32", "cond_true_", "define i32 @objc3c_entry")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_elision.objc3"
      CaseName = "objc3_dispatch_direct_nil_receiver_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_direct_nil_receiver_keyword_elision.objc3"
      CaseName = "objc3_dispatch_direct_nil_receiver_keyword_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_unary_elision.objc3"
      CaseName = "objc3_dispatch_nil_bound_identifier_unary_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_keyword_elision.objc3"
      CaseName = "objc3_dispatch_nil_bound_identifier_keyword_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_alias_identifier_elision.objc3"
      CaseName = "objc3_dispatch_nil_alias_identifier_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_conditional_receiver_elision.objc3"
      CaseName = "objc3_dispatch_nil_conditional_receiver_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "cond_true_", "cond_false_", "cond_merge_", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_elision.objc3"
      CaseName = "objc3_dispatch_nil_immutable_global_identifier_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_mutable_global_identifier_non_elided.objc3"
      CaseName = "objc3_dispatch_nil_mutable_global_identifier_non_elided"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_global_identifier_post_call_non_elided.objc3"
      CaseName = "objc3_dispatch_nil_global_identifier_post_call_non_elided"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_call_elision.objc3"
      CaseName = "objc3_dispatch_nil_immutable_global_identifier_post_pure_call_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_immutable_global_identifier_post_pure_prototype_call_elision.objc3"
      CaseName = "objc3_dispatch_nil_immutable_global_identifier_post_pure_prototype_call_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_mixed_flow.objc3"
      CaseName = "objc3_dispatch_nil_bound_identifier_mixed_flow"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_pre_reassignment_elision.objc3"
      CaseName = "objc3_dispatch_nil_bound_identifier_pre_reassignment_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_bound_identifier_post_assignment_nil_elision.objc3"
      CaseName = "objc3_dispatch_nil_bound_identifier_post_assignment_nil_elision"
      RequireLl = $true
      RequiredLlTokens = @("define i32 @main()", "define i32 @objc3c_entry", "ret i32")
      ForbiddenLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "icmp eq i32", "msg_dispatch_")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    }
  )
}

Export-ModuleMember -Function "Get-CoreRecoveryNilReceiverCaseDefinitions"

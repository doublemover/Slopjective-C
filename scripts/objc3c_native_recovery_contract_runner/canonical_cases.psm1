$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "case_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking

function Get-CoreRecoveryContractCases {
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
    },
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
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nil_receiver_semantic_compatibility.objc3"
      CaseName = "objc3_dispatch_nil_receiver_semantic_compatibility"
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
    },
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
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_call_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_call_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
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
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_global_identifier_post_extern_call_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_global_identifier_post_extern_call_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_mutable_global_identifier_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_mutable_global_identifier_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
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
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_nonzero_bound_identifier_invalidation_non_fast_path.objc3"
      CaseName = "objc3_dispatch_nonzero_bound_identifier_invalidation_non_fast_path"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    },
    @{
      Source = "tests/tooling/fixtures/native/recovery/positive/message_send_numeric_zero_receiver_non_elided.objc3"
      CaseName = "objc3_dispatch_numeric_zero_receiver_non_elided"
      RequireLl = $true
      RequiredLlTokens = @("declare i32 @objc3_runtime_dispatch_i32(", "call i32 @objc3_runtime_dispatch_i32(", "define i32 @objc3c_entry")
      RequiredManifestTokens = @('"runtime_dispatch_arg_slots":4', '"selector_global_ordering":"lexicographic"')
      RequireObjc3ManifestSurface = $true
    }
  )
}

function Invoke-CoreRecoveryContractCases {
  foreach ($case in Get-CoreRecoveryContractCases) {
    Invoke-ContractCase @case
  }
}

function Invoke-InvalidDispatchSymbolContract {
  param(
    [string]$OutDir,
    [string]$HelloObjc3Source
  )

  $invalidDispatchOutDir = Join-Path $OutDir "objc3_invalid_dispatch_symbol"
  New-Item -ItemType Directory -Force -Path $invalidDispatchOutDir | Out-Null
  $resolvedInvalidDispatchSource = [System.IO.Path]::GetFullPath($HelloObjc3Source)
  if (-not (Test-Path -LiteralPath $resolvedInvalidDispatchSource -PathType Leaf)) {
    throw "contract FAIL: missing invalid-dispatch source fixture at $resolvedInvalidDispatchSource"
  }
  $invalidDispatchExit = Invoke-Objc3cNativeWithRecovery -Arguments @(
    $resolvedInvalidDispatchSource,
    "--out-dir",
    $invalidDispatchOutDir,
    "--emit-prefix",
    "module",
    "--objc3-runtime-dispatch-symbol",
    "9invalid_symbol"
  )
  if ($invalidDispatchExit -ne 2) {
    throw "contract FAIL: invalid runtime dispatch symbol should fail with exit 2 (got $invalidDispatchExit)"
  }
  Write-Output "objc3_invalid_dispatch_symbol_rejected=true"
  Write-Output "objc3_invalid_dispatch_symbol_exit_code=$invalidDispatchExit"
}

Export-ModuleMember -Function @(
  "Get-CoreRecoveryContractCases",
  "Invoke-CoreRecoveryContractCases",
  "Invoke-InvalidDispatchSymbolContract"
)

# Native Execution Negative Fixtures

Each negative execution fixture is a pair of files sharing a basename:

- `<name>.objc3`: source fixture compiled by the execution smoke harness.
- `<name>.meta.json`: deterministic failure expectations for that fixture.

## Sidecar schema (`<name>.meta.json`)

```json
{
  "schema_version": 1,
  "fixture": "<name>.objc3",
  "expect_failure": {
    "stage": "compile|link|run",
    "required_diagnostic_tokens": [
      "token-a",
      "token-b"
    ]
  },
  "execution": {
    "requires_runtime_shim": true
  }
}
```

Field notes:

- `schema_version`: integer schema discriminator.
- `fixture`: fixture filename this sidecar describes.
- `expect_failure.stage`: first failing pipeline stage (`compile`, `link`, or `run`).
- `expect_failure.required_diagnostic_tokens`: case-sensitive substrings that must all appear in diagnostics for the failing stage.
- `execution.requires_runtime_shim`: whether successful execution would require a runtime dispatch shim.
- `execution.runtime_dispatch_symbol` (optional): expected dispatch symbol when `requires_runtime_shim` is true.

## Assignment fixture note

- `assignment_unknown_target.objc3` is a compile-stage negative expecting assignment-target diagnostics (`O3S214`).

## Compound-assignment fixture notes

- `compound_assignment_bool_target.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `compound_assignment_unknown_target.objc3` is a compile-stage negative expecting assignment-target diagnostics (`O3S214`).
- `compound_assignment_missing_rhs.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`).

## Increment/decrement fixture notes

- `increment_decrement_bool_target.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `increment_decrement_unknown_target.objc3` is a compile-stage negative expecting assignment-target diagnostics (`O3S214`).

## Bitwise/shift fixture notes

- `bitwise_bool_operand.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `bitwise_parser_missing_rhs.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`).

## Modulo/remainder fixture notes

- `modulo_bool_operand.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `modulo_assign_bool_rhs.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).

## Block-comment fixture notes

- `block_comment_unterminated.objc3` is a compile-stage negative expecting lexical diagnostics (`O3L002`).
- `block_comment_nested.objc3` is a compile-stage negative expecting lexical diagnostics (`O3L003`).
- `block_comment_stray_terminator.objc3` is a compile-stage negative expecting lexical diagnostics (`O3L004`).

## Do-while fixture note

- `do_while_missing_while_keyword.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`).

## For-loop fixture notes

- `for_missing_lparen.objc3` is a compile-stage negative expecting parser diagnostics (`O3P106`).
- `for_missing_body_statement.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`).
- `for_condition_function_value.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).

## Nested-block fixture note

- `nested_block_missing_rbrace.objc3` is a compile-stage negative expecting parser diagnostics (`O3P111`).

## Parameter suffix fixture notes

- `parameter_bool_nullability_suffix_unsupported.objc3` is a compile-stage negative expecting semantic parameter-suffix diagnostics (`O3S206`).
- `parameter_bool_generic_suffix_unsupported.objc3` is a compile-stage negative expecting semantic parameter generic-suffix diagnostics (`O3S206`).
- `parameter_bool_pointer_declarator_unsupported.objc3` is a compile-stage negative expecting semantic parameter pointer-declarator diagnostics (`O3S206`).

## Return fixture notes

- `return_void_with_value.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S211`).
- `return_nonvoid_missing_value.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S211`).
- `return_bool_generic_suffix_unsupported.objc3` is a compile-stage negative expecting semantic return-suffix diagnostics (`O3S206`).
- `return_bool_nullability_suffix_unsupported.objc3` is a compile-stage negative expecting semantic return-suffix diagnostics (`O3S206`).
- `return_bool_pointer_declarator_unsupported.objc3` is a compile-stage negative expecting semantic return pointer-declarator diagnostics (`O3S206`).
- `sel_return_nullability_suffix_unsupported.objc3` is a compile-stage negative expecting semantic return-suffix diagnostics (`O3S206`).
- `protocol_return_nullability_suffix_unsupported.objc3` is a compile-stage negative expecting semantic return-suffix diagnostics (`O3S206`).
- `instancetype_return_nullability_suffix_unsupported.objc3` is a legacy-name compile-stage negative that still asserts semantic return-suffix diagnostics (`O3S206`) for unsupported non-`id`/`Class`/`instancetype` return suffixes.

## id-alias parser fixture note

- `id_parser_missing_param_colon.objc3` is a compile-stage negative expecting parser diagnostics (`O3P107`).

## Foundation-alias parser fixture note

- `foundation_alias_parser_missing_param_colon.objc3` is a compile-stage negative expecting parser diagnostics (`O3P107`).

## Runtime-dispatch fixture notes

- `runtime_dispatch_unresolved_symbol.objc3` is a link-stage negative expecting unresolved symbol diagnostics for `objc3_msgsend_i32` on non-nil message-send lowering.
- `nil_receiver_runtime_dispatch_unresolved_symbol.objc3` is a link-stage negative expecting unresolved symbol diagnostics for `objc3_msgsend_i32` when a mutable receiver is reassigned from runtime-unknown value and lowering retains dispatch linkage.
- `numeric_zero_receiver_runtime_dispatch_unresolved_symbol.objc3` is a link-stage negative expecting unresolved symbol diagnostics for `objc3_msgsend_i32`; numeric zero receivers are intentionally non-elided and retain dispatch linkage.
- `nil_bound_identifier_reassigned_function.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`) for invalid mutable nil-bound identifier reassignment to a function value.

## Prototype fixture notes

- `prototype_signature_mismatch.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `prototype_unresolved_external.objc3` is a link-stage negative expecting unresolved symbol diagnostics for `external_add`.
- `extern_missing_fn_keyword.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for invalid `extern` declaration forms missing `fn`.
- `extern_function_with_body.objc3` is a compile-stage negative expecting parser diagnostics (`O3P104`) for `extern fn` declarations that include a function body.
- `extern_pure_missing_fn_keyword.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for malformed `extern pure` qualifier chains missing `fn`.
- `pure_extern_missing_fn_keyword.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for malformed `pure extern` qualifier chains missing `fn`.
- `extern_pure_function_with_body.objc3` is a compile-stage negative expecting parser diagnostics (`O3P104`) for declaration-only `extern pure fn` contracts violated by body-bearing forms.
- `pure_extern_function_with_body.objc3` is a compile-stage negative expecting parser diagnostics (`O3P104`) for declaration-only `pure extern fn` contracts violated by body-bearing forms.
- `duplicate_pure_qualifier.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for duplicate `pure` declaration qualifiers.
- `duplicate_extern_qualifier.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for duplicate `extern` declaration qualifiers.
- `overchained_extern_pure_extern_qualifiers.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for over-chained qualifier sequences with duplicate `extern`.
- `overchained_pure_extern_pure_qualifiers.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for over-chained qualifier sequences with duplicate `pure`.
- `misplaced_qualifier_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after `fn`.
- `misplaced_qualifier_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after `fn`.
- `misplaced_qualifier_extern_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after `extern fn`.
- `misplaced_qualifier_pure_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after `pure fn`.
- `misplaced_qualifier_after_name_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after a function identifier.
- `misplaced_qualifier_after_name_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a function identifier.
- `misplaced_qualifier_after_name_extern_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` between function identifier and parameter list in `extern fn` declarations.
- `misplaced_qualifier_after_name_pure_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` between function identifier and parameter list in `pure fn` declarations.
- `misplaced_qualifier_after_params_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after a function parameter list.
- `misplaced_qualifier_after_params_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a function parameter list.
- `misplaced_qualifier_after_params_extern_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after an `extern fn` parameter list.
- `misplaced_qualifier_after_params_pure_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a `pure fn` parameter list.
- `misplaced_qualifier_after_return_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after a function return annotation.
- `misplaced_qualifier_after_return_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a function return annotation.
- `misplaced_qualifier_after_return_extern_fn_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after an `extern fn` return annotation.
- `misplaced_qualifier_after_return_pure_fn_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a `pure fn` return annotation.
- `misplaced_qualifier_in_param_type_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` used as a parameter type annotation.
- `misplaced_qualifier_in_param_type_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` used as a parameter type annotation.
- `misplaced_qualifier_in_return_type_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` used as a function return type annotation.
- `misplaced_qualifier_in_return_type_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` used as a function return type annotation.
- `misplaced_qualifier_after_param_type_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after a parameter type annotation.
- `misplaced_qualifier_after_param_type_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after a parameter type annotation.
- `misplaced_qualifier_after_param_type_trailing_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` appearing after the final parameter type annotation.
- `misplaced_qualifier_after_param_type_trailing_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` appearing after the final parameter type annotation.
- `misplaced_qualifier_in_param_name_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in parameter identifier position.
- `misplaced_qualifier_in_param_name_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in parameter identifier position.
- `misplaced_qualifier_in_param_name_trailing_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in trailing parameter identifier position.
- `misplaced_qualifier_in_param_name_trailing_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in trailing parameter identifier position.
- `misplaced_qualifier_after_param_name_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` after a parameter name and before `:`.
- `misplaced_qualifier_after_param_name_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` after a parameter name and before `:`.
- `misplaced_qualifier_after_param_name_trailing_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` after a trailing parameter name and before `:`.
- `misplaced_qualifier_after_param_name_trailing_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` after a trailing parameter name and before `:`.
- `misplaced_qualifier_statement_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in statement position.
- `misplaced_qualifier_statement_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in statement position.
- `misplaced_qualifier_statement_trailing_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in trailing statement position.
- `misplaced_qualifier_statement_trailing_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in trailing statement position.
- `misplaced_qualifier_expression_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in expression position.
- `misplaced_qualifier_expression_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in expression position.
- `misplaced_qualifier_expression_let_init_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in `let` initializer expression position.
- `misplaced_qualifier_expression_let_init_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in `let` initializer expression position.
- `misplaced_qualifier_message_selector_head_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in message selector head position.
- `misplaced_qualifier_message_selector_head_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in message selector head position.
- `misplaced_qualifier_message_selector_segment_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in keyword selector segment position.
- `misplaced_qualifier_message_selector_segment_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in keyword selector segment position.
- `misplaced_qualifier_message_receiver_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in message receiver expression position.
- `misplaced_qualifier_message_receiver_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in message receiver expression position.
- `misplaced_qualifier_case_label_pure.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `pure` in case-label expression position.
- `misplaced_qualifier_case_label_extern.objc3` is a compile-stage negative expecting parser diagnostics (`O3P100`) for misplaced `extern` in case-label expression position.

## Switch fixture notes

- `switch_missing_lparen.objc3` is a compile-stage negative expecting parser diagnostics (`O3P106`).
- `switch_duplicate_default.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).
- `switch_signed_case_label_missing_number_minus.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`) for malformed signed case labels missing a numeric literal after `-`.
- `switch_signed_case_label_missing_number_plus.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`) for malformed signed case labels missing a numeric literal after `+`.
- `switch_return_missing_default.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because non-void switch return paths are not guaranteed without a `default` path.
- `switch_nonempty_case_fallthrough_return_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because an explicit `break` in a non-empty case body exits the switch before reaching downstream returning arms.
- `switch_conditional_case_break_path_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because conditional case-body `break` escape paths are conservative fail-closed for return-path analysis.
- `switch_case_loop_escape_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because loop/control-flow case-body escape paths remain conservative fail-closed for return-path analysis.
- `switch_case_while_true_break_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because non-static while conditions remain conservative fail-closed for case-body return-path chaining.
- `switch_case_for_true_break_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because non-static for conditions remain conservative fail-closed for case-body return-path chaining.
- `switch_case_do_while_true_break_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because non-static do-while conditions remain conservative fail-closed for case-body return-path chaining.
- `switch_case_unary_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved unary-negation condition forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_binary_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal binary condition forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_relational_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal relational condition forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_logical_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal logical condition forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_conditional_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal conditional-expression forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_arithmetic_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal arithmetic-expression forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_bitwise_shift_unknown_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved/non-literal bitwise/shift-expression forms remain conservative fail-closed for case-body return-path chaining.
- `switch_case_identifier_reassigned_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because reassigned identifier conditions remain conservative fail-closed for case-body return-path chaining.
- `switch_case_global_assigned_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because assigned global identifier conditions remain conservative fail-closed for case-body return-path chaining.
- `switch_case_if_unknown_break_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved `if` branch-selection remains conservative fail-closed for case-body return-path chaining.
- `if_call_unknown_else_missing_return_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved function-level `if` branch-selection remains conservative fail-closed for guaranteed-return analysis.
- `switch_call_condition_selected_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved function-level switch-condition selection remains conservative fail-closed for guaranteed-return analysis.
- `while_call_condition_body_return_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because unresolved function-level while-loop entry conditions remain conservative fail-closed for guaranteed-return analysis.
- `while_identifier_reassigned_body_return_conservative_reject.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S205`) because reassigned while-loop entry identifiers remain conservative fail-closed for guaranteed-return analysis.

## Conditional fixture notes

- `conditional_missing_colon.objc3` is a compile-stage negative expecting parser diagnostics (`O3P107`).
- `conditional_missing_then_statement.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`).
- `conditional_condition_function_value.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).

## Unary-plus fixture notes

- `unary_plus_missing_operand.objc3` is a compile-stage negative expecting parser diagnostics (`O3P103`).
- `unary_plus_bool_operand.objc3` is a compile-stage negative expecting semantic diagnostics (`O3S206`).

# Native Execution Positive Fixtures

Each positive execution fixture is a pair of files sharing a basename:

- `<name>.objc3`: source fixture compiled, linked, and executed by the native smoke harness.
- `<name>.exitcode.txt`: expected process exit code as a base-10 integer.
- Optional `<name>.meta.json`: fixture-specific native compile args for the smoke harness.

## Sidecar pattern

- Sidecar path rule: `[fixture].exitcode.txt`
- Example: `basic_i32_return_main.objc3` -> `basic_i32_return_main.exitcode.txt`

The expected exit code must be deterministic.

Optional meta sidecar schema:

- `fixture`: must match `<name>.objc3`.
- `execution.native_compile_args`: optional string array appended to native compiler arguments.
- `execution.requires_runtime_shim` (optional): defaults to `true`; set to `false` for fixtures that should link/execute without the runtime shim.

## Runtime-shim note

Fixtures that use message-send syntax (`[receiver selector: ...]`) require the runtime shim implementation of `objc3_msgsend_i32`.

For `message_send_runtime_shim.objc3`:

- Selector: `sum:with:`
- `selector_score = 4299`
- Shim value:
  - `41 + 97*9 + 7*3 + 11*4 + 13*0 + 17*0 + 19*4299 = 82660`
- Fixture return expression: `82660 - 82583 = 77`

So `message_send_runtime_shim.exitcode.txt` is `77`.

For `message_send_nil_receiver_short_circuit.objc3`:

- Mutable receiver value can evaluate to nil at runtime and short-circuits through the emitted nil-dispatch branch.
- Fixture returns `0 + 5`, so `message_send_nil_receiver_short_circuit.exitcode.txt` is `5`.
- Runtime dispatch symbol linkage is still required for this fixture because lowering retains the non-nil dispatch branch.

For `message_send_direct_nil_receiver_elision.objc3`:

- Direct nil receiver message-send lowering is compile-time elided to constant `0`.
- Fixture sets `execution.requires_runtime_shim=false` in sidecar metadata and links without `objc3_msgsend_i32` shim.
- Fixture returns `0 + 9`, so `message_send_direct_nil_receiver_elision.exitcode.txt` is `9`.

For `message_send_direct_nil_receiver_keyword_elision.objc3`:

- Direct nil receiver keyword message-send lowering is compile-time elided to constant `0`.
- Fixture sets `execution.requires_runtime_shim=false` in sidecar metadata and links without `objc3_msgsend_i32` shim.
- Fixture returns `0 + 6`, so `message_send_direct_nil_receiver_keyword_elision.exitcode.txt` is `6`.

For `message_send_nil_bound_identifier_unary_elision.objc3`:

- Immutable nil-bound identifier receiver unary message-send lowering is compile-time elided to constant `0`.
- Fixture sets `execution.requires_runtime_shim=false` in sidecar metadata and links without `objc3_msgsend_i32` shim.
- Fixture returns `0 + 12`, so `message_send_nil_bound_identifier_unary_elision.exitcode.txt` is `12`.

For `message_send_nil_bound_identifier_keyword_elision.objc3`:

- Immutable nil-bound identifier receiver keyword message-send lowering is compile-time elided to constant `0`.
- Fixture sets `execution.requires_runtime_shim=false` in sidecar metadata and links without `objc3_msgsend_i32` shim.
- Fixture returns `0 + 13`, so `message_send_nil_bound_identifier_keyword_elision.exitcode.txt` is `13`.

For `message_send_nil_bound_identifier_mixed_flow.objc3`:

- Mixed immutable/mutable nil-bound receiver flows are deterministic: immutable binding elides while mutable binding retains runtime dispatch branch/call behavior.
- Fixture links with runtime shim (default behavior) because one send remains non-elided.
- Fixture returns `0 + 0 + 14`, so `message_send_nil_bound_identifier_mixed_flow.exitcode.txt` is `14`.

For `message_send_nil_bound_identifier_pre_reassignment_elision.objc3`:

- A nil-bound identifier send-site that occurs before reassignment is flow-sensitively elided.
- Fixture sets `execution.requires_runtime_shim=false` in sidecar metadata and links without `objc3_msgsend_i32` shim.
- Fixture returns `0 + 16`, so `message_send_nil_bound_identifier_pre_reassignment_elision.exitcode.txt` is `16`.

## Assignment fixtures

- `assignment_basic_counter.objc3` returns `15` (`1+2+3+4+5`).
- `assignment_bool_branch.objc3` returns `1` after deterministic bool assignment.
- `assignment_nested_loop_control.objc3` returns `32` (`4` outer iterations * inner contribution `8`).

## Compound-assignment fixtures

- `compound_assignment_basic.objc3` returns `9` after deterministic `+=`, `-=`, `*=`, `/=` sequencing.
- `compound_assignment_for_step.objc3` returns `10` and validates `i += 1` in `for` step plus `sum += i` in loop body.

## Increment/decrement fixtures

- `increment_decrement_basic.objc3` returns `1` after deterministic `value++`, `++value`, `value--`, `--value` sequencing.
- `increment_decrement_for_step.objc3` returns `9` and validates postfix/prefix update operators in `for` step clauses.

## Bitwise/shift fixtures

- `bitwise_basic.objc3` returns `25` and validates `&`, `|`, `^`, `<<`, `>>`, and unary `~`.
- `bitwise_precedence.objc3` returns `17` and validates precedence across bitwise, shift, and additive tiers.

## Modulo/remainder fixtures

- `modulo_basic.objc3` returns `4` and validates `%` plus `%=` update semantics.
- `modulo_loop_mix.objc3` returns `10` and validates modulo expressions inside deterministic loop flow.

## Do-while fixtures

- `do_while_sum.objc3` returns `15`.
- `do_while_break_continue.objc3` returns `19`.

## For-loop fixtures

- `for_sum.objc3` returns `10` (`0+1+2+3+4`).
- `for_break_continue.objc3` returns `13` (skips `2`, breaks at `6`).
- `for_no_condition_break.objc3` returns `4` (infinite-form loop with deterministic break).

## Switch fixtures

- `switch_basic_match.objc3` returns `22` from a matched case arm.
- `switch_default_path.objc3` returns `77` from the default arm.
- `switch_nested_loop_break.objc3` returns `23` and validates switch-in-loop `break`/`continue` behavior.

## Conditional fixtures

- `conditional_basic_select.objc3` returns `11`.
- `conditional_nested_select.objc3` returns `7`.

## Unary-plus fixtures

- `unary_plus_basic.objc3` returns `9` and validates unary-plus identity lowering in expression position.
- `unary_plus_global_initializer_chain.objc3` returns `11` and validates unary-plus support in global constant initializer chains.

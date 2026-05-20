# Native Execution Positive Fixtures

Each positive execution fixture is a pair of files sharing a basename:

- `<name>.objc3`: source fixture compiled, linked, and executed by the native smoke harness.
- `<name>.exitcode.txt`: expected process exit code as a base-10 integer.
- Optional `<name>.meta.json`: fixture-specific native compile args for the smoke harness.

## Sidecar pattern

- Sidecar path rule: `[fixture].exitcode.txt`
- Example: `basic_i32_return_main.objc3` -> `basic_i32_return_main.exitcode.txt`

The expected exit code must be deterministic.

## Block ARC fixture notes

- `escaping_owned_object_block_copy_dispose.objc3` returns `17` and prepares issue #8033 evidence for an escaping block that combines implicit owned-object capture, explicit weak capture, and byref state under ARC mode. This is fixture preparation only; runtime support claims remain owned by the block ARC implementation and executable proof lanes.

Execution-positive fixtures are e2e-owned success contracts. A filename that
mentions a parser, semantic, lowering, or runtime concept is phase provenance
for the corresponding canonical owner, but the positive claim remains a
deterministic compile-link-run claim. It must not be cited as gate, retired route,
compatibility, migration, or alternate runtime acceptance support.

Optional meta sidecar schema:

- `fixture`: must match `<name>.objc3`.
- `execution.native_compile_args`: optional string array appended to native compiler arguments.
- `execution.requires_live_runtime_dispatch` (optional): defaults to `false`; set to `true` for fixtures that must keep a live runtime-dispatch declaration/call in emitted LLVM IR.
- `execution.runtime_dispatch_symbol` (optional): expected emitted dispatch symbol when `execution.requires_live_runtime_dispatch` is `true` and one symbol is sufficient.
- `execution.runtime_dispatch_symbols` (optional): expected emitted dispatch symbols when a fixture must prove multiple live dispatch entrypoints, such as i32 plus typed/from-class dispatch. This field is mutually exclusive with `execution.runtime_dispatch_symbol` and must be absent when live dispatch is not required.

## Live-runtime dispatch note

Fixtures that use supported live message-send syntax (`[receiver selector: ...]`) now prove execution through the native runtime dispatch entrypoint family: `objc3_runtime_dispatch_i32`, `objc3_runtime_dispatch_i32_from_class`, `objc3_runtime_dispatch_typed_value`, or `objc3_runtime_dispatch_typed_value_from_class`. Unknown selectors publish a typed strict dispatch error through `objc3_runtime_dispatch_i32_checked`, and the public `i32` entrypoint aborts instead of fabricating a value when strict dispatch fails.

For `message_send_nil_receiver_short_circuit.objc3`:

- Mutable receiver value can evaluate to nil at runtime and short-circuits through the emitted nil-dispatch branch.
- Fixture returns `0 + 5`, so `message_send_nil_receiver_short_circuit.exitcode.txt` is `5`.
- No live runtime dispatch linkage is required because explicit nil reassignment enables compile-time elision.

For `message_send_direct_nil_receiver_elision.objc3`:

- Direct optional nil receiver message-send lowering returns `0` without a live dispatch requirement.
- Fixture returns `0 + 9`, so `message_send_direct_nil_receiver_elision.exitcode.txt` is `9`.

For `message_send_direct_nil_receiver_keyword_elision.objc3`:

- Direct optional nil receiver keyword message-send lowering returns `0` without a live dispatch requirement.
- Fixture returns `0 + 6`, so `message_send_direct_nil_receiver_keyword_elision.exitcode.txt` is `6`.

For `message_send_nil_bound_identifier_unary_elision.objc3`:

- Immutable nil-bound identifier receiver unary optional send returns `0` without a live dispatch requirement.
- Fixture returns `0 + 12`, so `message_send_nil_bound_identifier_unary_elision.exitcode.txt` is `12`.

For `message_send_nil_bound_identifier_keyword_elision.objc3`:

- Immutable nil-bound identifier receiver keyword optional send returns `0` without a live dispatch requirement.
- Fixture returns `0 + 13`, so `message_send_nil_bound_identifier_keyword_elision.exitcode.txt` is `13`.

For `message_send_nil_bound_identifier_mixed_flow.objc3`:

- Mixed immutable/mutable nil-bound receiver flows are deterministic through optional sends.
- Fixture returns `0 + 0 + 14`, so `message_send_nil_bound_identifier_mixed_flow.exitcode.txt` is `14`.

For `message_send_nil_bound_identifier_pre_reassignment_elision.objc3`:

- A nil-bound identifier optional send-site that occurs before reassignment returns `0`.
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

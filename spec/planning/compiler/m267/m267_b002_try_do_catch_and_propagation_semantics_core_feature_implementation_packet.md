# M267-B002 Packet

Milestone: `M267`
Lane: `B`
Issue: `M267-B002`

## Summary

Implement truthful Part 6 source-only semantic legality for `try`, `throw`, and `do/catch` so the compiler stops treating them as parser-rejected placeholders and instead emits one deterministic semantic packet describing the live legality boundary.

## Dependencies

- `M267-B001`
- `M266-E002`
- `M265-E002`

## Deliverables

- parser admission and AST source records for `try`, `try?`, `try!`, `throw`, and `do/catch`
- one semantic packet at `frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`
- deterministic legality checking for propagation context, throwing/bridged operand resolution, catch-clause ordering, and throw-context legality
- issue-local expectations, packet, checker, pytest, readiness runner, and fixtures
- explicit docs/spec/package anchors describing the source-only semantic boundary and the deferred native-runtime boundary

## Positive proof surface

Use:

- `tests/tooling/fixtures/native/m267_try_do_catch_semantics_positive.objc3`

Required positive truths:

- `try_expression_sites = 3`
- `try_propagating_sites = 1`
- `try_optional_sites = 1`
- `try_forced_sites = 1`
- `throw_statement_sites = 1`
- `do_catch_sites = 1`
- `catch_clause_sites = 2`
- `catch_binding_sites = 1`
- `catch_all_sites = 1`
- `throwing_callable_try_sites = 2`
- `bridged_callable_try_sites = 3`
- `caller_propagation_sites = 1`
- `local_handler_sites = 0`
- `contract_violation_sites = 0`
- `try_surface_landed = true`
- `throw_surface_landed = true`
- `do_catch_surface_landed = true`
- `deterministic = true`
- `ready_for_lowering_and_runtime = false`

## Negative proof surface

- `tests/tooling/fixtures/native/m267_try_requires_throwing_context_negative.objc3`
  - expected diagnostic: `propagating try requires a throws function or an enclosing do/catch [O3S272]`
- `tests/tooling/fixtures/native/m267_try_requires_throwing_or_bridged_operand_negative.objc3`
  - expected diagnostic: `try operand must be a throwing or NSError-bridged call surface [O3S271]`
- `tests/tooling/fixtures/native/m267_throw_requires_throws_or_catch_negative.objc3`
  - expected diagnostic: `throw statements require a throws function or a catch body [O3S274]`
- `tests/tooling/fixtures/native/m267_catch_after_catch_all_negative.objc3`
  - expected diagnostic: `catch clauses after a catch-all are unreachable [O3S269]`
- `tests/tooling/fixtures/native/m267_try_do_catch_native_fail_closed.objc3`
  - expected diagnostic: `unsupported feature claim: do/catch statements are not yet runnable in Objective-C 3 native mode [O3S267]`

## Exit condition

`try`, `throw`, and `do/catch` are parser-admitted and semantically validated in source-only native validation runs; one deterministic semantic packet is emitted at `frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`; native emission remains truthfully fail-closed for this feature family until later lane-C/D work lands.

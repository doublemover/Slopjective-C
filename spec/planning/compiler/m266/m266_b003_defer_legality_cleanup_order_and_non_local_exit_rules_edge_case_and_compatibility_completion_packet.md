# M266-B003 Packet

Milestone: `M266`
Lane: `B`
Issue: `M266-B003`

## Summary

Implement live `defer` legality semantics for the admitted source-only Part 5 surface so cleanup-order accounting and non-local-exit restrictions are no longer placeholders.

## Dependencies

- `M266-B002`

## Implementation slice

- admit statement-form `defer { ... }` in the source-only frontend/sema pipeline
- publish truthful Part 5 source-closure and semantic-model summaries for live `defer` sema
- account for deterministic `defer` cleanup-order sites in the semantic packet
- diagnose `return`, escaping `break`, and escaping `continue` inside `defer` bodies with deterministic diagnostics
- keep runnable lowering/runtime execution of `defer` fail closed until later lane-C/lane-D work

## Happy-path proof

- a source-only positive fixture with a legal nested loop inside a `defer` body compiles successfully
- the manifest shows live `defer` semantic-site and cleanup-order accounting with no defer non-local-exit diagnostics

## Negative proof

- `return` inside `defer` fails with `O3S222`
- escaping `break` inside `defer` fails with `O3S222`
- escaping `continue` inside `defer` fails with `O3S222`

## Exit condition

The compiler truthfully admits source-only `defer` sema, emits the updated Part 5 semantic packet, rejects illegal defer-mediated non-local exits deterministically, and the issue-local validation chain passes.

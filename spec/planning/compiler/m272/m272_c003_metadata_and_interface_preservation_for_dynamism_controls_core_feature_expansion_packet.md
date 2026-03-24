# M272-C003 Packet: Metadata And Interface Preservation For Dynamism Controls - Core Feature Expansion

Issue: `#7341`
Dependencies: `M272-C002`, `M272-B003`
Next issue: `M272-D001`

## Objective

Implement the remaining Part 9 metadata/interface preservation layer above the live `M272-C002` direct-call lowering slice.

## Implementation requirements

1. Preserve effective direct/final/sealed dispatch intent in runtime metadata source records.
2. Preserve the same Part 9 intent through emitted `module.runtime-import-surface.json` artifacts.
3. Reload the same Part 9 preservation packet and widened source-record fields from imported runtime surfaces.
4. Emit one replay-stable LLVM IR/frontend metadata summary for the Part 9 preservation surface.
5. Add issue-local provider/consumer fixture coverage, checker, pytest, and lane-C readiness replay.

## Truth constraints

- Keep this lane focused on metadata/interface preservation and replay proof.
- Do not claim broader runtime dispatch realization or optimizer-driven direct dispatch.
- Do not add a new public runtime ABI surface.
- Stable evidence must land under `tmp/reports/m272/M272-C003/`.

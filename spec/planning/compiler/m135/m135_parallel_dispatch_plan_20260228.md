# M135 Parallel Dispatch Plan (2026-02-28)

## Scope

- Milestone: `M135 Native Frontend Extensions and Direct LLVM Emission` (`#220`)
- Packet map: `spec/planning/compiler/m135/m135_issue_packets_20260228.md`
- Closeout evidence: `spec/planning/compiler/m135/m135_closeout_evidence_20260228.md`

## Lane Allocation

| Lane | Issue | Focus |
| --- | --- | --- |
| A | [#4264](https://github.com/doublemover/Slopjective-C/issues/4264) | Replace clang IR object compile invocation with direct native LLVM object emission for `.objc3`. |
| B | [#4265](https://github.com/doublemover/Slopjective-C/issues/4265) | Expand parser/sema support for richer Objective-C object declarator forms. |
| C | [#4266](https://github.com/doublemover/Slopjective-C/issues/4266) | Extend lowering coverage for additional Objective-C dispatch patterns. |
| D | [#4267](https://github.com/doublemover/Slopjective-C/issues/4267) | Harden runtime/link integration for native-emitted object artifacts. |
| E | [#4268](https://github.com/doublemover/Slopjective-C/issues/4268) | Closeout gate packet, direct-LLVM contract docs, and CI/tooling fail-closed enforcement. |
| INT | Lane E integration responsibility | No separate M135 INT issue; lane E owns closeout sequencing and regroup evidence. |

## Dispatch Order And Ownership

1. Execute lanes `A`, `B`, `C`, and `D` in parallel with non-overlapping write ownership.
2. Keep lane `E` active as the integration gate for packet quality, contract documentation, and CI/tooling guardrails.
3. Final closeout sequence for M135: `A -> B -> C -> D -> E`.

## M135 Native Command Set

Lane evidence and closeout gating use the following deterministic command set:

- `npm run build:objc3c-native`
- `npm run test:objc3c:execution-smoke`
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lane-e`

## Tracking Notes

- Milestone state source: `gh api repos/doublemover/Slopjective-C/milestones/220`
- Issue state source: `gh api "repos/doublemover/Slopjective-C/issues?milestone=220&state=all&per_page=100"`

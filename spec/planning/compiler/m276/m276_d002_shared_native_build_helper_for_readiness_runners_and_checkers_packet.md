# M276-D002 Shared Native Build Helper For Readiness Runners And Checkers Packet

Issue: `#7394`
Milestone: `M276`
Lane: `D`

## Objective

Introduce one shared helper for native build acquisition so readiness runners and checkers stop hard-coding raw npm build semantics.

## Dependency handoff

- Depends on `M276-A001`, `M276-A002`, and `M276-C001`.
- Unblocks `M276-D001`, which will migrate the broader active readiness range onto this helper surface.

## Implementation truths

- The shared helper is `scripts/ensure_objc3c_native_build.py`.
- The helper maps policy modes onto the PowerShell wrapper execution modes.
- The PowerShell wrapper now supports forced reconfigure without deleting the build tree.
- Active readiness runners may adopt the helper incrementally; this issue only proves the surface with a small active-runner slice.

## Proof model

- Run helper `fast`.
- Run helper `contracts`.
- Run helper `full --force-reconfigure`.
- Prove at least two active readiness runners invoke the helper rather than raw npm build calls.

## Exit condition

A single helper owns build-mode selection and forced reconfigure behavior, emits summaries under `tmp/`, and is already adopted by at least two active readiness runners before the broader migration issue begins.

# M272-D001 Packet: Runtime Fast-Path Integration Contract - Contract and Architecture Freeze

Packet: `M272-D001`
Issue: `#7342`
Dependencies: `M272-C002`
Next issue: `M272-D002`

## Objective

Freeze the truthful Part 9 runtime contract above the already-landed direct-call lowering and the existing runtime method-cache / slow-path / fallback engine.

## Implementation requirements

1. Keep the issue focused on the current runtime boundary rather than inventing a second dispatch runtime.
2. Make the runtime contract explicit in the private runtime header/source and the cross-module link-plan path.
3. Add one runnable fixture plus one runtime probe that prove direct-bypass, dynamic opt-out, positive cache reuse, and deterministic fallback behavior.
4. Add deterministic checker, pytest, package scripts, and lane-D readiness coverage.
5. Land stable evidence under `tmp/reports/m272/M272-D001/`.

## Truth constraints

- Do not claim broader live runtime fast-path widening yet.
- Do not claim optimizer-driven devirtualization.
- Do not reopen the public runtime ABI.

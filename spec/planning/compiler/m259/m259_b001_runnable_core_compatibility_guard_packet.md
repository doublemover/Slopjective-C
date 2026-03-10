# M259-B001 Runnable Core Compatibility Guard Packet

Packet: `M259-B001`

Issue: `#7210`

Milestone: `M259`

Lane: `B`

## Objective

Freeze the compatibility/migration guard that keeps the current runnable core
truthful while later advanced surfaces remain outside the release-facing native
subset.

## Dependencies

- `M259-A002`

## Acceptance

- Freeze the docs/spec/code boundary for the runnable-core guard.
- Keep compatibility mode and migration assist explicit as live selections.
- Keep `O3S216` plus the current unsupported-feature diagnostics explicit as the
  landed fail-closed semantic boundary.
- Keep smoke and replay positioned as runnable-core coverage only.
- Publish deterministic evidence at
  `tmp/reports/m259/M259-B001/runnable_core_compatibility_guard_summary.json`.
- Next issue: `M259-B002`.

## Truthful boundary

- `M259-A002` remains the current integrated live runtime proof asset.
- This packet does not claim blocks, ARC, throws, async/await, actors,
  strictness modes, strict concurrency, or feature-macro publication are part
  of the runnable release-facing core.
- This packet freezes the current semantic and documentation boundary so the
  next issue can implement broader fail-closed diagnostics against it.

# ADR-0002: CLI and Library Separation

- Status: Accepted
- Date: 2026-02-27
- Deciders: objc3c native maintainers
- Related issues: M132-A001 (#4228), M132-B002 (#4231)

## Context

The native driver must support both CLI execution and embedding use cases.
Mixing process/file operations with frontend core logic makes parity and testing
difficult.

## Decision

- Keep `libobjc3c_frontend` free of process spawning and direct filesystem
  policy concerns.
- Restrict `driver` to argument parsing, orchestration invocation, and exit code
  mapping.
- Route filesystem/process effects through `io` adapters.

## Consequences

- Library/CLI parity tests can compare output contracts directly.
- Embedders can call library APIs without inheriting CLI side effects.
- Driver code size should reduce as extraction progresses.

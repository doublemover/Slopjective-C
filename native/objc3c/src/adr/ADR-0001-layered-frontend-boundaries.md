# ADR-0001: Layered Frontend Boundaries

- Status: Accepted
- Date: 2026-02-27
- Deciders: objc3c native maintainers
- Related issue: M132-A001 (#4228)

## Context

`native/objc3c/src/main.cpp` is monolithic. Extraction requires a fixed module
shape and dependency direction before code moves begin.

## Decision

Adopt layered frontend boundaries in this order:

`lex -> parse -> sema -> lower -> ir`

with integration modules:

- `pipeline` (stage orchestration)
- `libobjc3c_frontend` (public API)
- `driver` (CLI adapter)
- `io` (artifact/process adapter)

Dependency directions are defined in `../ARCHITECTURE.md` and are mandatory for
includes and target links.

## Consequences

- Extraction tasks can proceed in parallel without ambiguous ownership.
- Boundary checks can be automated by script and CI gates.
- Some current utility code in `main.cpp` will need relocation into shared
  utility headers to avoid cross-layer coupling.

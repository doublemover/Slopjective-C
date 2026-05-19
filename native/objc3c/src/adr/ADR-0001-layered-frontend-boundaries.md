# ADR-0001: Layered Frontend Boundaries

- Status: Accepted
- Date: 2026-02-27
- Deciders: objc3c native maintainers
- Related surface: `objc3c.frontend.layeredboundaries.v1`

## Context

`native/objc3c/src/main.cpp` is now the native CLI shell entrypoint that
delegates into the driver boundary. The hard-cutover tree has split stage,
support, pipeline, artifact, runtime, library, driver, CLI, and tool owners
under `native/objc3c/src/`; this ADR records the dependency direction those
owners must preserve.

## Decision

Adopt layered frontend boundaries in this order:

`lex -> parse -> sema -> lower -> ir`

with integration modules:

- `artifacts` (manifest, metadata, and publication projections over the
  completed frontend phases)
- `pipeline` (stage orchestration)
- `libobjc3c_frontend` (public API)
- `driver` (CLI adapter)
- `io` (artifact/process adapter)

Dependency directions are defined in `../ARCHITECTURE.md` and are mandatory for
includes and target links.

## Consequences

- Extraction tasks can proceed in parallel without ambiguous ownership.
- Boundary checks can be automated by script and CI gates.
- `main.cpp` must remain a shell over the driver boundary; shared utilities
  belong in stage/support owners rather than re-entering the CLI entrypoint.

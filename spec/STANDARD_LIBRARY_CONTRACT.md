# Objective-C 3.0 - Standard Library Contract (Minimum) {#s}

_Working draft v0.11 - last updated 2026-02-23_

## S.0 Purpose and scope (normative) {#s-0}

This document defines the minimum standard-library surface required for Objective-C 3.0 conformance claims.

It specifies:

- required module names and capability identifiers,
- minimum exported types/functions for each required module,
- semantic contracts needed by language rules in [Part 3](#part-3), [Part 6](#part-6), [Part 7](#part-7), and [Part 12](#part-12),
- versioning and ABI-stability expectations,
- conformance test obligations for required library APIs.

This contract is normative for toolchains claiming ObjC 3.0 Core/Strict/Strict Concurrency/Strict System profiles.

## S.1 Required modules and capability identifiers (normative) {#s-1}

### S.1.1 Canonical module names {#s-1-1}

A conforming implementation shall provide the following modules (or aliases mapped to these canonical capability IDs):

| Canonical module | Capability ID | Required for profile |
| --- | --- | --- |
| `objc3.core` | `objc3.cap.core` | Core and above |
| `objc3.errors` | `objc3.cap.errors` | Core and above |
| `objc3.concurrency` | `objc3.cap.concurrency` | Core and above |
| `objc3.keypath` | `objc3.cap.keypath` | Core and above |
| `objc3.system` | `objc3.cap.system` | Strict System |

If implementation-specific names are used, module metadata and conformance reports shall publish the canonical capability-ID mapping.

### S.1.2 Discovery and import requirements {#s-1-2}

- Importing required modules shall preserve effects/isolation metadata required by [D Table A](#d-3-1).
- Missing required modules for a claimed profile is a conformance failure and shall be diagnosed.
- Capability IDs shall be queryable from emitted interface/module metadata.

## S.2 Minimum API surface by module (normative) {#s-2}

### S.2.1 `objc3.core` {#s-2-1}

`objc3.core` shall provide:

- baseline protocol/type aliases needed by this draft to name core contracts,
- feature/capability query hooks sufficient for profile-aware diagnostics,
- public constants or metadata exposing language/profile revision identifiers.

### S.2.2 `objc3.errors` {#s-2-2}

`objc3.errors` shall provide at least:

- protocol `Error` (or equivalent) matching [Part 6](#part-6),
- carrier `Result<T, E>` with `Ok`/`Err` construction and inspection,
- helpers required to bridge status-code/NSError-style APIs to `throws`/`Result` flows.

Canonical nil-to-error helpers (required):

- `orThrow<T>(value: T?, makeError: () -> id<Error>) throws -> T`
- `okOr<T, E: Error>(value: T?, makeError: () -> E) -> Result<T, E>`

Minimum semantic guarantees:

- `Error` values are stably representable across module boundaries.
- `Result` case identity and payload layout are stable enough for separate compilation.
- Conversions used by language-defined lowering preserve error payloads without silent loss.
- `orThrow` and `okOr` shall evaluate `value` exactly once.
- `orThrow`/`okOr` shall evaluate `makeError` lazily and at most once, only when `value` is `nil`.
- `orThrow` shall throw exactly the produced `Error` value on `nil`; `okOr` shall return `Err(producedError)` on `nil`.

### S.2.3 `objc3.concurrency` {#s-2-3}

`objc3.concurrency` shall provide at least:

- child-task spawn primitive (recognized as structured spawn),
- detached-task spawn primitive,
- task-handle join/wait APIs (throwing and/or status-returning forms),
- task-group scope APIs,
- cancellation query and cancellation checkpoint APIs,
- executor hop primitive or equivalent runtime hook.

Minimum semantic guarantees:

- child/detached cancellation behavior matches [Part 7](#part-7),
- join/wait APIs reflect cancellation/error outcomes without reporting false success,
- executor/actor boundaries preserve required suspension semantics.

### S.2.4 `objc3.keypath` {#s-2-4}

`objc3.keypath` shall provide at least:

- key-path value type(s) for typed key-path literals,
- lookup/application APIs sufficient to evaluate key paths against supported roots,
- metadata representation that preserves type components across module boundaries.

### S.2.5 `objc3.system` (profile-gated) {#s-2-5}

For Strict System claims, `objc3.system` shall provide:

- resource cleanup helpers needed by [Part 8](#part-8),
- borrowed/lifetime helper intrinsics where language rules require runtime cooperation,
- diagnostics/tooling hooks required by [Part 12](#part-12) strict-system diagnostics.

## S.3 Semantic contracts for required primitives (normative) {#s-3}

### S.3.1 Errors and propagation {#s-3-1}

- `throws` lowering and `Result` adapters shall be interoperable under separate compilation.
- `try`/`try?`/propagation behavior shall remain consistent whether declarations are imported textually or via module metadata.
- `orThrow` shall participate in `try` as an ordinary throwing call; `okOr` shall participate in postfix `?` as an ordinary `Result` value per [Part 6](#part-6).

### S.3.2 Concurrency primitives {#s-3-2}

- Spawn APIs must preserve structured-parent relationships when marked as child spawn.
- Detached APIs must not inherit cancellation unless explicitly documented.
- Cancellation checkpoints must be observable and testable.
- Executor hop APIs must preserve current-task context and required isolation metadata.

### S.3.3 Key-path behavior {#s-3-3}

- Key-path application must preserve nullability/type constraints defined in [Part 3](#part-3).
- Invalid key-path application shall diagnose per profile requirements.

## S.4 Versioning and ABI expectations (normative) {#s-4}

### S.4.1 Module versioning model {#s-4-1}

Each required module shall expose semantic version metadata:

- `module_semver_major`,
- `module_semver_minor`,
- `module_semver_patch`,
- and supported capability IDs.

Major-version changes indicate potentially incompatible API/ABI changes.
Minor-version changes are additive and backward compatible.

### S.4.2 ABI stability requirements {#s-4-2}

For claimed profile conformance:

- exported signatures for required APIs shall be stable within a major version,
- metadata describing effects/isolation/capabilities shall satisfy [Part 2](#part-2) and [D](#d) compatibility rules,
- mixing module binaries/interfaces with incompatible major versions shall be diagnosed.

### S.4.3 Profile-specific stability requirements {#s-4-3}

- Core/Strict claims require stable `objc3.core`, `objc3.errors`, `objc3.concurrency`, and `objc3.keypath` contracts.
- Strict System claims additionally require stable `objc3.system` contracts.
- Optional feature-set modules are out of scope unless that feature-set conformance is claimed.

## S.5 Conformance test obligations (normative minimum) {#s-5}

A conforming implementation shall include tests equivalent in coverage to:

1. Module presence/discovery:
   - required module import succeeds for claimed profile;
   - missing required module is diagnosed.
2. Error model:
   - `Error` and `Result` APIs interoperate with `throws` lowering across module boundaries.
   - `orThrow` with non-`nil` input returns payload and does not evaluate `makeError`.
   - `orThrow` with `nil` input throws the value produced by `makeError` (single evaluation).
   - `okOr` with non-`nil` input returns `Ok(payload)` and does not evaluate `makeError`.
   - `okOr` with `nil` input returns `Err(producedError)` from a single `makeError` evaluation.
   - `try orThrow(...)` and `okOr(...)?` compile and behave as specified in [Part 6](#part-6).
3. Concurrency:
   - child vs detached semantics, cancellation propagation, join/wait outcomes, executor hops.
4. Key paths:
   - typed key-path metadata round-trip and runtime application behavior.
5. Versioning:
   - incompatibility diagnostics for major-version mismatch;
   - compatibility for additive minor updates.

## S.6 Relationship to conformance checklist (normative) {#s-6}

- [E.3](#e-3) profile checklist items that reference standard-library requirements shall be interpreted using this document.
- Toolchains shall cite this contract in conformance manifests when claiming Core/Strict/Strict Concurrency/Strict System profiles.


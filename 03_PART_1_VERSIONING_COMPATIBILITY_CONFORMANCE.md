# Part 1 — Versioning, Compatibility, and Conformance

## 1.1 Purpose
This part defines:
- how Objective‑C 3.0 is enabled,
- how source and behavior changes are contained,
- conformance levels and the meaning of “strictness,”
- required feature-test mechanisms,
- migration tooling requirements.

## 1.2 Language mode selection
### 1.2.1 Compiler flag
A conforming implementation shall provide a command-line mechanism equivalent to:

- `-fobjc-version=3`

Selecting ObjC 3.0 mode shall:
- enable the ObjC 3.0 grammar additions,
- enable ObjC 3.0 default rules (nonnull-by-default regions, etc.),
- enable ObjC 3.0 diagnostics (at least warnings).

### 1.2.2 Source directive
A conforming implementation may provide a source directive, such as:

- `#pragma objc_language_version(3)`

If supported, the directive shall apply to the translation unit and shall be rejected in headers intended for multiple language modes unless a conditional mechanism is used.

> Open issue: decide whether per-file-only is permitted or per-region switching is allowed. This draft assumes per-translation-unit switching to avoid surprising interactions.

## 1.3 Reserved keywords
In ObjC 3.0 mode, the following tokens are reserved as keywords (at minimum):

- `defer`, `guard`, `match`, `case`, `await`, `async`, `actor`

The spec shall define a compatibility escape hatch:
- a raw identifier syntax (e.g., `@identifier(defer)`) or
- a compiler option that warns and offers fix-its for renaming conflicts.

## 1.4 Feature test macros
A conforming implementation shall provide predefined macros to allow conditional compilation:

- `__OBJC_VERSION__` (integer, at least 3 in ObjC 3.0 mode)
- `__OBJC3_FEATURE_<NAME>__` per feature, e.g.:
  - `__OBJC3_FEATURE_DEFER__`
  - `__OBJC3_FEATURE_OPTIONALS__`
  - `__OBJC3_FEATURE_ASYNC_AWAIT__`

Feature macros shall be defined in ObjC 3.0 mode even if a feature is disabled by profile or configuration, to allow feature probing.

## 1.5 Conformance levels (strictness)
A translation unit in ObjC 3.0 mode may additionally declare a conformance level. Conformance levels alter diagnostics and may alter defaults where explicitly stated.

### 1.5.1 Levels
- **Permissive**: language features enabled; safety checks default to warnings; legacy patterns allowed.
- **Strict**: key safety checks are errors; opt-outs require explicit `unsafe` spellings.
- **Strict-system**: strict + additional system-API safety checks (resource cleanup correctness, borrowed pointer escapes, etc.).

### 1.5.2 Selecting a level
A conforming implementation shall provide an option equivalent to:

- `-fobjc3-strictness=permissive|strict|strict-system`

### 1.5.3 Diagnostic escalation rule
If a construct is ill-formed in strict mode, it may still be accepted in permissive mode with a warning *only* if:
- the compiler can preserve baseline behavior, and
- the behavior is not undefined.

## 1.6 Source compatibility principles
### 1.6.1 No silent semantic changes in non-ObjC3 code
Compiling code not in ObjC 3.0 mode shall not change meaning due to this specification.

### 1.6.2 Contained default changes
Default changes (e.g., nonnull-by-default) apply only inside explicitly marked regions or ObjC 3.0 modules.

## 1.7 Migration tooling requirements
A conforming implementation shall provide:
- diagnostics with fix-its for common migrations,
- a batch migrator capable of applying safe transformations.

Minimum migrator capabilities:
1. Insert nullability annotations based on static inference and usage.
2. Convert “weak-strong dance” patterns to capture lists (Part 8).
3. Convert common manual cleanup patterns to `defer` or `@resource` (Part 8).
4. Suggest `withLifetime/keepAlive` when borrowed-pointer diagnostics trigger (Part 8).
5. Provide stubs for `throws` and `Result` bridging from NSError patterns (Part 6).

## 1.8 Profiles (hook point)
The spec defines optional profiles (e.g., `@profile(system)`, `@profile(kernel)`) as a mechanism to apply additional restrictions and defaults. Profiles are defined in Part 8 (system) and may be extended in future drafts.

> Open issue: the exact syntax for profiles is not finalized in this draft; the semantic requirement is.

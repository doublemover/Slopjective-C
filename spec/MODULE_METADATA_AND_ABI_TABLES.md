# Objective‑C 3.0 — Module Metadata and ABI Surface Tables {#d}

_Working draft v0.11 — last updated 2026-02-27_

## D.0 Purpose {#d-0}

Objective‑C 3.0 adds source-level semantics that **must survive module boundaries**:
effects (`async`, `throws`), nullability defaults, executor/actor isolation, and dispatch controls (`objc_direct`, `objc_sealed`, etc.).

This document is a **normative checklist** for what a conforming implementation must preserve in:

- module metadata (binary or AST-based), and
- any emitted textual interface ([B.7](#b-7)).

It also summarizes which features are **ABI-affecting** and what ABI stability constraints apply.

> This document does not mandate a specific on-disk format. It defines _required information_.

## D.1 Terminology {#d-1}

- **Module metadata**: any compiled representation that an importer uses instead of re-parsing raw headers (e.g., Clang module files, serialized AST, interface stubs).
- **Textual interface**: a tool-emitted, importable text representation intended to preserve semantics for distribution ([B.7](#b-7)).
- **ABI-affecting**: changes the calling convention, symbol shape, layout, or runtime dispatch surface such that mismatches across translation units could cause miscompilation or runtime faults.

## D.2 Module metadata requirements (normative) {#d-2}

A conforming implementation shall preserve, for all exported declarations:

1. **Full type information** including:
   - parameter/return types,
   - nullability qualifiers and defaults ([Part 3](#part-3)),
   - ownership qualifiers and ARC-related attributes ([Part 4](#part-4)),
   - pragmatic generics information ([Part 3](#part-3)) sufficient for type checking (even if erased at ABI).

2. **Effect information**:
   - whether the callable is `throws`,
   - whether the callable is `async`,
   - and whether the callable is _potentially suspending at call sites_ due to isolation ([Part 7](#part-7)).

3. **Isolation and scheduling metadata**:
   - executor affinity (`objc_executor(...)`) ([Part 7](#part-7)),
   - actor isolation (actor type, isolated vs nonisolated members) ([Part 7](#part-7)),
   - Sendable-like constraints for cross-task/actor boundaries ([Part 7](#part-7)).

4. **Dispatch and dynamism controls**:
   - `objc_direct`, `objc_final`, `objc_sealed` ([Part 9](#part-9)),
   - any attributes that change call legality or override/subclass legality.

5. **Interop bridging markers**:
   - `objc_nserror` and `objc_status_code(...)` ([Part 6](#part-6)),
   - any language-defined overlays/bridges that affect call lowering.

6. **Provenance**:
   - the owning module of each exported declaration ([Part 2](#part-2)),
   - API partition membership (public/SPI/private) if the toolchain supports it ([Part 2](#part-2)).

### D.2.1 Metadata encoding/version header (normative) {#d-2-1}

Each module metadata payload shall carry a version header with:

- `schema_major` (non-negative integer),
- `schema_minor` (non-negative integer),
- a `required_capabilities` set naming non-ignorable semantic extensions used by the payload.

In addition, each encoded metadata field shall be classified as one of:

- **required semantic field**: importer must understand and validate it to preserve [Table A](#d-3-1) semantics,
- **ignorable extension field**: importer may ignore it without changing required language semantics.

Unknown required semantic fields (or unknown `required_capabilities`) shall be treated as incompatibilities.
Unknown ignorable extension fields are forward-compatible and may be skipped.

### D.2.2 Versioning rules (normative) {#d-2-2}

`schema_major` shall be incremented for any incompatible encoding or semantic interpretation change, including:

- changing the meaning of an existing required field,
- deleting a required field without an equivalent replacement,
- reinterpreting a required field such that an older importer would miscompile accepted code.

`schema_minor` shall be incremented for compatible additive changes, including:

- adding new ignorable extension fields,
- adding new required capabilities that can be detected and rejected by older importers.

Within one `schema_major` line, a change is compatible only if an importer that does not implement the new minor revision will either:

- ignore only ignorable extension fields, or
- reject import deterministically before semantic use because a required field/capability is unknown.

Within one `schema_major` line:

- changing a required semantic field to ignorable, or changing semantic defaults for omitted required metadata, is incompatible and requires `schema_major` increment,
- changing an ignorable extension field to required semantic is allowed only with a new required capability and `schema_minor` increment so older importers hard-error instead of miscompiling,
- editorial-only/spec-clarification updates with no encoding or semantic change do not require a schema version change.

### D.2.3 Importer validation outcomes (normative) {#d-2-3}

Importer validation distinguishes:

- **hard error**: import is rejected for that module/interface unit,
- **recoverable diagnostic**: import may continue with conservative assumptions, as allowed by profile rules in [Table E](#d-3-5).

A conforming importer shall not silently drop missing, unknown, or mismatched required semantic metadata.

Importer validation shall, at minimum:

1. validate `schema_major` support (mismatch is a hard error),
2. validate `required_capabilities` and required-field recognition (unknown required data is a hard error),
3. classify absent/invalid known-required semantic metadata as a missing-metadata condition and diagnose per [Table E](#d-3-5),
4. accept unknown ignorable extension fields without changing required [Table A](#d-3-1) semantics.

### D.2.4 Portable concurrency interface metadata (OCI-1) {#d-2-4}

To support cross-toolchain interchange beyond native module files, implementations shall support OCI-1 metadata export/import for concurrency-relevant declarations.

OCI-1 requirements:

- schema header: `oci_schema_major`, `oci_schema_minor`,
- declaration identity key stable across emit/import within a release line,
- required concurrency fields listed in [Table F](#d-3-6).

OCI-1 versioning and compatibility follow the same rules as [D.2.2](#d-2-2) and [D.3.4](#d-3-4).
Unknown required OCI-1 fields/capabilities are hard errors.

## D.3 Required metadata tables {#d-3}

### D.3.1 Table A — “must preserve across module boundaries” {#d-3-1}

| Feature                                                         | Applies to                         | Must be recorded in module metadata | Must be emitted in textual interface | Notes                                                                                                  |
| --------------------------------------------------------------- | ---------------------------------- | ----------------------------------: | -----------------------------------: | ------------------------------------------------------------------------------------------------------ |
| Nullability qualifiers (`_Nullable`, `_Nonnull`)                | types, params, returns, properties |                                  ✅ |                                   ✅ | Includes inferred defaults where part of the public contract.                                          |
| Nonnull-by-default regions                                      | headers / interface units          |                                  ✅ |                                   ✅ | Canonical pragmas from [B.2](#b-2).                                                                    |
| Pragmatic generics parameters                                   | class/protocol types, collections  |                                  ✅ |                                   ✅ | ABI may erase; type checking must not.                                                                 |
| `T?`, `T!` (optional spellings)                                 | type surface                       |                                  ✅ |                                   ✅ | Emitted form may choose canonical spellings but semantics must match.                                  |
| `throws` effect                                                 | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| `async` effect                                                  | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| Executor affinity `objc_executor(...)`                          | funcs/methods/types                |                                  ✅ |                                   ✅ | May imply call-site `await` when crossing executors ([Part 7](#part-7)).                               |
| Actor type / actor isolation                                    | actor classes + members            |                                  ✅ |                                   ✅ | Includes nonisolated markings.                                                                         |
| Sendable-like constraints                                       | types, captures, params/returns    |                                  ✅ |                                   ✅ | Importers must be able to enforce strict checks.                                                       |
| Borrowed pointer qualifier (`borrowed T *`)                     | types (params/returns)             |                                  ✅ |                                   ✅ | [Part 8](#part-8); enables escape diagnostics in strict-system. Importers must preserve the qualifier. |
| Borrowed-return marker (`objc_returns_borrowed(owner_index=N)`) | functions/methods                  |                                  ✅ |                                   ✅ | [Part 8](#part-8); identifies the owner parameter for lifetime checking of interior pointers.          |
| Task-spawn recognition (`objc_task_spawn` etc.)                 | stdlib entry points                |                                  ✅ |                                   ✅ | Needed for compiler enforcement at call sites.                                                         |
| Direct method `objc_direct`                                     | methods                            |                                  ✅ |                                   ✅ | Changes call legality and category interactions.                                                       |
| Final `objc_final`                                              | classes/methods                    |                                  ✅ |                                   ✅ | Optimization + legality constraints.                                                                   |
| Sealed `objc_sealed`                                            | classes                            |                                  ✅ |                                   ✅ | Cross-module subclass legality.                                                                        |
| Derive/macro synthesized declarations                           | types/members                      |                                  ✅ |                                   ✅ | Synthesized members must be visible to importers before layout/ABI decisions.                          |
| Error bridging markers (`objc_nserror`, `objc_status_code`)     | funcs/methods                      |                                  ✅ |                                   ✅ | Affects lowering of `try` and wrappers.                                                                |
| Availability / platform gates                                   | all exported decls                 |                                  ✅ |                                   ✅ | Not a new ObjC 3.0 feature, but required for correctness.                                              |

### D.3.2 Table B — ABI boundary summary (v1) {#d-3-2}

| Feature                  | ABI impact                            | Required stability property                                       | Canonical/recommended ABI shape                                                      |
| ------------------------ | ------------------------------------- | ----------------------------------------------------------------- | ------------------------------------------------------------------------------------ |
| `throws`                 | **Yes**                               | Caller and callee must agree on calling convention across modules | Recommended: trailing `id<Error> * _Nullable outError` ([C.4](#c-4)).                |
| `async`                  | **Yes**                               | Importers must call using the same coroutine/continuation ABI     | Recommended: LLVM coroutine lowering ([C.5](#c-5)), with a task/executor context.    |
| Executor/actor isolation | Usually **no** (call-site scheduling) | Metadata must exist so importer inserts hops and requires `await` | No ABI change required for sync bodies; hop is an `await`ed scheduling operation.    |
| Optional chaining/sends  | No                                    | Semantics preserved in caller IR                                  | Lower to conditional receiver check; args not evaluated on nil ([C.3.1](#c-3-1)).    |
| Direct methods           | Sometimes (dispatch surface)          | Importers must know legality + dispatch mode                      | Recommended: direct symbol call + omit dynamic lookup where permitted ([C.8](#c-8)). |
| Generics (pragmatic)     | No (erased)                           | Importers must type-check consistently                            | Erased in ABI; preserved in metadata/interface.                                      |
| Key paths                | Library ABI                           | Standard library must define stable representation                | ABI governed by stdlib; metadata must preserve `KeyPath<Root,Value>` types.          |

### D.3.3 Table C — Runtime hooks (minimum expectations) {#d-3-3}

This table names **conceptual hooks**. Implementations may use different symbol names.

| Area                 | Minimum required capability                                          | Notes                                                                  |
| -------------------- | -------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| Executors            | Enqueue continuation onto an executor; query “current executor”      | Needed for `objc_executor(...)` and for `await` resumption.            |
| Tasks                | Create child/detached tasks; join; cancellation state                | Standard library provides API; compiler enforces via attributes.       |
| Actors               | Each actor instance maps to a serial executor; enqueue isolated work | Representation is implementation-defined (inline field or side table). |
| Autorelease pools    | Push/pop implicit pools around execution slices                      | Normative on ObjC runtimes ([C.7](#c-7)).                              |
| Diagnostics metadata | Preserve enough source mapping for async/macro debugging             | [Part 12](#part-12) requires debuggability.                            |

### D.3.4 Table D — Metadata version compatibility matrix (normative) {#d-3-4}

| Producer metadata vs importer support / payload condition                     | Compatibility direction                      | Required importer behavior                                                                                    |
| ----------------------------------------------------------------------------- | -------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| `schema_major` equal; producer `schema_minor` <= importer max minor           | backward (new importer reads older payload)  | Accept; treat absent newer fields as unavailable.                                                             |
| `schema_major` equal; producer `schema_minor` > importer max minor            | forward (older importer reads newer payload) | Accept only if all unknown elements are ignorable extension fields and all `required_capabilities` are known. |
| `schema_major` equal; producer minor newer with unknown required data         | forward                                      | Hard error: reject import; report unknown required field/capability.                                          |
| `schema_major` equal; known-required field missing/invalid in payload         | both                                         | Diagnose per [Table E](#d-3-5); ABI-significant/effect-lowering omissions remain hard errors in all profiles. |
| `schema_major` differs                                                        | both                                         | Hard error: reject import; report producer and importer major versions.                                       |

For forward compatibility, ignorable extension fields are explicitly non-semantic for [Table A](#d-3-1) conformance and may be skipped.

### D.3.5 Table E — Importer validation by conformance profile (normative) {#d-3-5}

Profiles are defined in [Part 1](#part-1) and the checklist in [E.2](#e-2).

| Validation condition                                                                                                                          | Core                                                                   | Strict                  | Strict Concurrency      | Strict System           |
| --------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------- | ----------------------- | ----------------------- | ----------------------- |
| Missing or mismatched ABI-significant/effect-lowering metadata (`throws`, `async`, `objc_nserror`, `objc_status_code(...)`, dispatch markers) | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Missing or mismatched isolation metadata (`objc_executor(...)`, actor isolation/nonisolated markers)                                          | Recoverable diagnostic; conservative import                            | Hard error              | Hard error              | Hard error              |
| Missing or mismatched Sendable/task-spawn metadata                                                                                            | Recoverable diagnostic                                                 | Recoverable diagnostic  | Hard error              | Hard error              |
| Missing or mismatched borrowed/lifetime system metadata                                                                                       | Recoverable diagnostic                                                 | Recoverable diagnostic  | Recoverable diagnostic  | Hard error              |
| Unknown ignorable extension field                                                                                                             | Ignored (may emit note)                                                | Ignored (may emit note) | Ignored (may emit note) | Ignored (may emit note) |
| Unknown required field/capability                                                                                                             | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Textual interface vs module metadata mismatch for any [Table A](#d-3-1) item                                                                  | Hard error for ABI-significant items; otherwise recoverable diagnostic | Hard error              | Hard error              | Hard error              |

Missing-metadata severity by profile shall follow [Table E](#d-3-5) exactly:

- **Core**: non-ABI missing metadata may be recoverable, but diagnostics are mandatory and import must remain conservative.
- **Strict**: missing isolation metadata is a hard error; sendability/task-spawn and borrowed/lifetime metadata remain recoverable unless another row upgrades severity.
- **Strict Concurrency**: missing concurrency metadata required for isolation/sendability/task-spawn is a hard error.
- **Strict System**: missing concurrency metadata and borrowed/lifetime metadata is a hard error.

When OCI-1 is the active interchange path, missing OCI-1 fields map into these same rows by category (`effects.*`, `isolation.*`, `sendable.*`).

### D.3.6 Table F — OCI-1 required concurrency metadata fields (normative) {#d-3-6}

| OCI-1 field | Meaning | Required for profile |
| --- | --- | --- |
| `decl_id` | Stable declaration identity key for merge/join across interface payloads | Core and above |
| `effects.async` | Whether declaration is async | Core and above |
| `effects.throws` | Whether declaration is throwing | Core and above |
| `isolation.executor` | Executor affinity (`objc_executor(...)` equivalent) | Core and above |
| `isolation.actor` | Actor isolation binding for declarations/members | Core and above |
| `isolation.nonisolated` | Explicit nonisolated marker | Core and above |
| `sendable.boundary` | Whether Sendable-like checking applies at this boundary | Strict Concurrency and Strict System (diagnostic optional in Core/Strict) |
| `sendable.unsafe_marker` | Unsafe-sendable escape marker metadata | Strict Concurrency and Strict System |

## D.4 Conformance tests (minimum) {#d-4}

A conforming implementation’s test suite shall include the following module/interface tests.

### D.4.1 Normative round-trip interface test flow {#d-4-1}

For each representative API set, tests shall perform all of:

1. Build module `M` from source declarations.
2. Emit an interface representation `I` for `M` (textual or equivalent).
3. Import `I` in a clean translation unit and separately import `M`.
4. Verify that imported declarations from step 3 are semantically equivalent for [Table A](#d-3-1) items.
5. Compile call sites that exercise the imported declarations and verify diagnostics/typing behavior are unchanged.

Minimum required declaration coverage for round-trip tests:

- `throws` declarations (function + method), including bridging markers (`objc_nserror`, `objc_status_code(...)`) and unchanged `try` obligations.
- `async` declarations with `await` call sites.
- combined `async throws` declarations with `try await` call sites.
- Executor-isolated declarations (`objc_executor(...)`) requiring a hop.
- Actor-isolated declarations, including at least one `nonisolated` member.
- Dispatch legality attributes (`objc_direct`, `objc_final`, `objc_sealed`).
- Profile-gated metadata: Sendable/task-spawn metadata and borrowed/lifetime metadata where claimed.

Round-trip verification shall compare, for each covered declaration, the effect/attribute tuple seen by importers:

- effects (`throws`, `async`) and error-bridging markers,
- isolation/executor/nonisolated metadata,
- task-spawn/sendability metadata where profile-claimed,
- dispatch and borrowed/lifetime markers where applicable.

### D.4.2 Compatibility and diagnostics tests {#d-4-2}

A conforming test suite shall include positive and negative import tests for [Table D](#d-3-4) and [Table E](#d-3-5), including:

- importing older minor metadata with a newer importer (accepted),
- importing newer minor metadata containing only ignorable extension fields (accepted),
- importing newer metadata with unknown required fields/capabilities (hard error),
- importing metadata with different `schema_major` (hard error),
- intentionally removing or altering metadata for `throws`/`async`/error-bridging/isolation and asserting profile-appropriate diagnostics,
- profile-matrix checks for missing metadata severity (Core recoverable for non-ABI metadata; Strict isolation hard error; Strict Concurrency hard error for concurrency metadata; Strict System hard error for concurrency plus borrowed/lifetime metadata).

### D.4.3 OCI-1 export/import tests (normative minimum) {#d-4-3}

A conforming test suite shall include OCI-1-specific tests that validate:

- exporting OCI-1 from an interface and importing it in a clean build preserves concurrency behavior (`await`/hop/isolation diagnostics),
- missing required OCI-1 fields are diagnosed per [D.3.5](#d-3-5),
- unknown required OCI-1 fields/capabilities trigger hard errors,
- additive OCI-1 minor-version fields marked ignorable are accepted,
- OCI-1 round-trip preserves `effects.async`, `effects.throws`, and required isolation/sendability fields from [Table F](#d-3-6).


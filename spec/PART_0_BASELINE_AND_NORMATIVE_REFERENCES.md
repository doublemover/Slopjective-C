# Part 0 — Baseline and Normative References {#part-0}

_Working draft v0.11 — last updated 2026-02-27_

## 0.1 Purpose {#part-0-1}

This part establishes:

1. The baseline language and de-facto dialect that Objective-C 3.0 builds upon.
2. The stable external references that are normative for conformance.
3. Priority rules when this draft and an external reference conflict.
4. Shared normative terminology used across all parts.

Objective-C 3.0 is a language mode layered on top of a stable baseline. That baseline is pinned by the normative reference identifiers in [Section 0.2.1](#part-0-2-1).
Cross-part composition rules for ordering/lifetime/cleanup/suspension are defined in [Abstract Machine and Semantic Core](#am).

## 0.2 Normative reference model {#part-0-2}

### 0.2.1 Stable normative reference index {#part-0-2-1}

The table below is the complete normative reference index for this draft.
No external source is normative unless it appears here with an `NR-*` identifier.
For each identifier, a conforming toolchain release shall pin an immutable tuple: `(version or edition, date or release stamp, commit or artifact hash)`.

| ID                  | Canonical reference                                                         | Version / edition                      | Date / release stamp                        | Commit / artifact hash                                        | Exact normative scope in this draft                                                                                                             |
| ------------------- | --------------------------------------------------------------------------- | -------------------------------------- | ------------------------------------------- | ------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| **NR-C18**          | ISO/IEC 9899:2018 (C18)                                                     | ISO/IEC 9899:2018                      | 2018 edition                                | N/A (published standard text)                                 | Core C lexical grammar, preprocessing model, declarators, expression semantics, and inherited C UB categories unless explicitly overridden.     |
| **NR-CPP20**        | ISO/IEC 14882:2020 (C++20)                                                  | ISO/IEC 14882:2020                     | 2020 edition                                | N/A (published standard text)                                 | C++ core-language rules that apply in mixed Objective-C++ translation units where this draft allows C++ interop.                                |
| **NR-LLVM-OBJC**    | LLVM/Clang Objective-C language + ARC behavior                              | `llvmorg-18.1.8`                       | Release stamp for `llvmorg-18.1.8`          | Git commit resolved by tag `llvmorg-18.1.8`                   | Objective-C front-end baseline for ARC qualifier semantics, retain/release insertion, method-family inference, and legacy ObjC diagnostics.     |
| **NR-BLOCKS-ABI**   | Clang Blocks specification and block ABI model                              | `llvmorg-18.1.8`                       | Release stamp for `llvmorg-18.1.8`          | Git commit resolved by tag `llvmorg-18.1.8`                   | Block object representation, capture semantics, calling convention, and copy/dispose helper contracts used by Parts 4/7/8.                      |
| **NR-OBJC-RUNTIME** | Objective-C runtime profile bundle for selected platform family             | `objc3-runtime-2025Q4`                 | Profile stamp `2025Q4`                      | Artifact digest(s) listed in `objc3-runtime-2025Q4` manifest  | Runtime class/metaclass model, selector identity, dispatch and category attachment semantics, and runtime constraints referenced by Parts 9/11. |
| **NR-ABI-PLATFORM** | Platform C/ObjC ABI profile bundle selected by the toolchain conformance ID | `objc3-abi-2025Q4`                     | Profile stamp `2025Q4`                      | Artifact digest(s) listed in `objc3-abi-2025Q4` manifest      | Calling conventions, data layout assumptions, symbol/linkage contracts, and ABI boundaries where this draft does not define a replacement ABI.  |

Conforming implementations shall publish concrete URLs and resolved hashes for `NR-LLVM-OBJC`, `NR-BLOCKS-ABI`, `NR-OBJC-RUNTIME`, and `NR-ABI-PLATFORM` in release notes or conformance reports.
For published language standards (`NR-C18`, `NR-CPP20`), the cited edition text is the immutable artifact.

### 0.2.2 Reference precedence and conflict rules {#part-0-2-2}

If sources disagree, conformance shall be determined by this priority order:

1. Explicit normative rules in this draft (Parts 0-12, plus B/C/D/E).
2. Normative references listed in [Section 0.2.1](#part-0-2-1), interpreted by their pinned tuples.
3. Implementation-defined behavior explicitly permitted by this draft.

Conflict/override rules:

- If an ObjC 3.0 rule and a normative reference conflict, the ObjC 3.0 rule overrides the reference for conformance.
- If this draft forbids a construct that an external reference would otherwise allow, the construct is ill-formed; diagnostic required.
- If this draft is silent and two external references conflict, the implementation shall choose one behavior, document it as implementation-defined, and preserve that selection consistently within a toolchain release line.
- A conforming implementation shall emit a diagnostic when the selected behavior can cause cross-module incompatibility under strict profiles.
- Informative text never overrides normative text.

### 0.2.3 Policy for de-facto behavior without a stable standard {#part-0-2-3}

Some Objective-C behaviors are de-facto rather than formally standardized. For those behaviors:

- The implementation shall bind behavior to explicit snapshot profiles (`NR-OBJC-RUNTIME`, `NR-ABI-PLATFORM`) instead of an unversioned "current compiler/runtime" description.
- Each snapshot profile shall include: toolchain version(s), runtime family/version(s), target ABI set, and any deviations from this draft.
- If a behavior is neither specified by this draft nor pinned by an `NR-*` snapshot, that behavior is implementation-defined and shall be documented before claiming conformance.
- Upgrading snapshot profiles is a versioned compatibility event and shall follow [Part 1](#part-1) compatibility rules.

### 0.2.4 Cross-reference rule for Parts 3-12 {#part-0-2-4}

Parts 3-12 shall cite baseline dependencies by stable identifiers from [Section 0.2.1](#part-0-2-1) (for example, `NR-LLVM-OBJC`) rather than by vendor/tool names alone.

### 0.2.5 Citation granularity for external rules {#part-0-2-5}

When a requirement depends on external behavior, the containing section shall cite the `NR-*` identifier and, where available, the external clause/section name.

## 0.3 Normative language and conformance tags {#part-0-3}

### 0.3.1 Requirement words {#part-0-3-1}

The terms **shall**, **must**, **shall not**, **must not**, **should**, and **may** are interpreted with RFC-style normative force.

### 0.3.2 Section tags and conformance impact {#part-0-3-2}

This draft uses the following labels consistently:

- **Normative**: required for conformance.
- **Minimum**: normative floor; stronger behavior is permitted if it does not violate this draft.
- **Normative intent**: normative summary of required externally observable behavior when low-level lowering details are left implementation-specific.
- **Recommended**: quality-of-implementation guidance, not required for conformance unless the section also says "shall/must".
- **Informative** / **informative summary**: explanatory only.
- If a heading omits a status label, the section shall be treated as normative only when it contains explicit requirement words (`shall`, `must`, `shall not`, `must not`); otherwise it is informative guidance.

When a heading label and sentence-level requirement words appear to conflict, sentence-level requirement words control.

## 0.4 Definitions and shared terminology {#part-0-4}

### 0.4.1 Ill-formed program; diagnostic required {#part-0-4-1}

A program is **ill-formed** if it violates a syntactic or static semantic rule in this draft.
A conforming implementation shall emit at least one diagnostic in the required phase from [Section 0.4.6](#part-0-4-6).

Preferred wording in this draft is: **"ill-formed; diagnostic required"**.
Parts 1-12 shall use this exact phrase when introducing static prohibitions, optionally followed by profile-specific severity requirements.

### 0.4.2 Conditionally-supported construct {#part-0-4-2}

A construct is **conditionally-supported** if support is optional.
If an implementation does not support that construct and a program requires it, the program is ill-formed for that implementation; diagnostic required.
The implementation shall document support status and any profile gating.

### 0.4.3 Undefined behavior {#part-0-4-3}

**Undefined behavior (UB)** means no semantic requirements are imposed on the implementation after the UB point is reached, except where a rule explicitly requires a diagnostic or runtime handling.

### 0.4.4 Unspecified behavior {#part-0-4-4}

**Unspecified behavior** means two or more outcomes are permitted and the implementation is not required to document or consistently choose one outcome.

### 0.4.5 Implementation-defined behavior {#part-0-4-5}

**Implementation-defined behavior** means one of multiple permitted outcomes is selected by the implementation and that selection shall be documented.

### 0.4.6 Phase mapping for core behavior terms {#part-0-4-6}

Unless a specific part overrides this table, the following phase mapping applies:

| Term                                           | Compile/import phase                                                                                        | Link phase                                                                                                           | Runtime phase                                                                                                                     |
| ---------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| **Ill-formed**                                 | At least one diagnostic shall be emitted when the violation is detectable before code generation completes. | If detection requires whole-program information, at least one link-time diagnostic shall be emitted where practical. | Runtime handling is required only when a part explicitly designates a runtime-detected violation (for example trap/throw/assert). |
| **Conditionally-supported** (unsupported case) | Diagnostic required once unsupported use is determinable during compile/import.                             | Diagnostic required if unsupported use is first determinable during linkage/composition.                             | If support checks are intentionally deferred, runtime shall report failure per the relevant part.                                 |
| **Undefined behavior**                         | Diagnostic not required (optional diagnostics are permitted).                                               | Diagnostic not required.                                                                                             | No requirements after UB is reached unless explicitly added by another rule.                                                      |
| **Unspecified behavior**                       | Diagnostic not required.                                                                                    | Diagnostic not required.                                                                                             | Any permitted outcome may occur at each instance; documentation is not required.                                                  |
| **Implementation-defined behavior**            | Diagnostic not required solely due to the existence of alternatives; selected behavior shall be documented. | Same as compile/import.                                                                                              | Executions shall follow the documented selection when the behavior is exercised.                                                  |

### 0.4.7 Translation unit {#part-0-4-7}

A translation unit is a single source file after preprocessing and module import processing, compiled as one unit.

### 0.4.8 Objective-C 3.0 mode {#part-0-4-8}

Objective-C 3.0 mode is the compilation mode in which ObjC 3.0 grammar, defaults, and diagnostics are enabled ([Part 1](#part-1)).

### 0.4.9 Conformance level {#part-0-4-9}

A conformance level is a set of additional requirements and diagnostics applied on top of ObjC 3.0 mode ([Part 1](#part-1)).

### 0.4.10 Retainable pointer type {#part-0-4-10}

A retainable pointer type is a type for which ARC may insert retain/release operations. This includes:

- Objective-C object pointers (`id`, `Class`, `NSObject *`, etc.),
- block pointers,
- additional retainable families declared by annotations ([Part 8](#part-8)).

### 0.4.11 Object pointer type {#part-0-4-11}

An object pointer type is a pointer to an Objective-C object, including `id` and `Class`. In this draft, object pointer excludes CoreFoundation toll-free bridged pointers unless explicitly annotated as retainable families ([Part 8](#part-8)).

### 0.4.12 Block pointer type {#part-0-4-12}

A block pointer type is a pointer to a block literal/closure as defined by NR-BLOCKS-ABI.

### 0.4.13 Nullability {#part-0-4-13}

Nullability is a type property (for object and block pointer types) indicating whether `nil` is a valid value. Nullability kinds are defined in [Part 3](#part-3).

### 0.4.14 Optional type {#part-0-4-14}

An optional type is written `T?` in ObjC 3.0 mode. Optional types are a type-system feature for object and block pointer types (v1), distinct from Objective-C's "message to nil returns zero" runtime behavior ([Part 3](#part-3)).

### 0.4.15 IUO type {#part-0-4-15}

An implicitly unwrapped optional type is written `T!`. In v1, IUO primarily supports migration and is discouraged in strict modes ([Part 3](#part-3)).

### 0.4.16 Carrier type {#part-0-4-16}

A carrier type represents either a success value or an alternative early-exit/absence value and participates in propagation (`?`). In v1, carriers include:

- optional types (`T?`), and
- `Result<T, E>` ([Part 6](#part-6)).

### 0.4.17 Effects {#part-0-4-17}

Effects are part of a function's type and govern call-site obligations. In v1, effects include `throws` ([Part 6](#part-6)) and `async` ([Part 7](#part-7)).

### 0.4.18 Executor {#part-0-4-18}

An executor is an abstract scheduler for task continuations ([Part 7](#part-7)). Executors may represent:

- the main/UI executor,
- a global background executor,
- or custom executors defined by the runtime/library.

### 0.4.19 Task {#part-0-4-19}

A task is a unit of asynchronous work managed by the concurrency runtime/library. Tasks may be child tasks or detached tasks and may be cancellable ([Part 7](#part-7)).

### 0.4.20 Actor {#part-0-4-20}

An actor is a concurrency-isolated reference type whose mutable state is protected by isolation rules ([Part 7](#part-7)). Cross-actor interactions are mediated by `await` and Sendable-like constraints.

## 0.5 Relationship to C and C++ {#part-0-5}

Objective-C 3.0 remains a superset of C and compatible with Objective-C++ as implemented by the toolchain.

- Unless explicitly overridden by this draft, C/C++ undefined, unspecified, and implementation-defined behavior classes are inherited from `NR-C18` and `NR-CPP20`.
- ObjC 3.0 may add stricter static constraints; where it does, behavior that would otherwise be inherited is reclassified as ill-formed; diagnostic required.
- ObjC 3.0 does not weaken inherited C/C++ undefined behavior into defined behavior unless a rule explicitly defines replacement semantics.
- In mixed Objective-C++ translation units, C++ core-language UB remains UB unless a stricter ObjC 3.0 rule rejects the construct earlier.

## 0.6 Open issues {#part-0-6}

No open issues are tracked in this part for v0.11.

Resolved references: runtime manifest publication
(`spec/conformance/objc3-runtime-2025Q4_manifest_validation.md`), ABI
manifest publication
(`spec/conformance/objc3_abi_manifest_validation_v0.11_A02.md`), and
abstract-machine synchronization protocol
(`spec/process/ABSTRACT_MACHINE_SYNC_PROTOCOL.md`).


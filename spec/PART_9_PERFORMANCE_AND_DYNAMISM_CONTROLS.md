# Part 9 — Performance and Dynamism Controls {#part-9}

_Working draft v0.11 — last updated 2026-02-23_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 9.1 Purpose {#part-9-1}

Objective‑C’s dynamic dispatch is a strength, but “open world” dynamism can inhibit optimization and complicate reasoning about safety.

Objective‑C 3.0 provides **opt-in** controls that:

- enable direct calls and devirtualization where safe,
- communicate intent to compilers and reviewers,
- preserve dynamic behavior by default,
- remain safe under separate compilation and across module boundaries ([C.2](#c-2)).

This part is designed to accommodate both Cocoa-style dynamic patterns _and_ system/library subsets where predictability and performance matter.

## 9.2 Direct methods {#part-9-2}

### 9.2.1 Concept {#part-9-2-1}

A _direct method_ is a method that:

- is not dynamically dispatched by selector lookup,
- cannot be overridden,
- and is intended to be invoked via a direct call when statically referenced.

### 9.2.2 Canonical spelling {#part-9-2-2}

A direct method is declared with:

```c
__attribute__((objc_direct))
```

applied to an Objective‑C method declaration.

Objective‑C 3.0 v1 also standardizes a class-level default:

```c
__attribute__((objc_direct_members))
```

applied to an Objective‑C class `@interface` / `@implementation`.

### 9.2.3 Semantics (normative) {#part-9-2-3}

1. A direct method shall not be overridden in any subclass.
2. A direct method shall not be introduced or replaced by a category with the same selector.
3. A direct method shall not participate in message forwarding semantics (`forwardInvocation:`) as part of selector lookup.

Calling model:

- A direct method call shall be formed only by a _statically-resolved_ method reference (i.e., the compiler must know it targets a direct method declaration).
- Dynamic message sends (e.g., `objc_msgSend`, `performSelector:`) shall not be relied upon to find a direct method.

**Programmer model:** “If you want dynamic, do not mark it direct.”

### 9.2.4 Separate compilation notes (normative) {#part-9-2-4}

Because direct methods and class-level direct defaults change call legality and dispatch surfaces, the `objc_direct` and `objc_direct_members` attributes (plus per-member `objc_dynamic` opt-outs) shall be:

- recorded in module metadata (see [D.3.1](#d-3-1) [Table A](#d-3-1)), and
- preserved in emitted interfaces.

Importers shall reconstruct the same **effective directness** for each method declaration.
Effect/attribute mismatches across modules are ill-formed ([C.2](#c-2)).

### 9.2.5 Recommended lowering (informative) {#part-9-2-5}

Recommended lowering is described in [C.8](#c-8).
At a high level:

- the compiler emits a callable symbol for each method implementation,
- calls to an effectively-direct method use a direct call rather than `objc_msgSend`, and
- methods explicitly marked `objc_dynamic` remain selector-dispatched.

### 9.2.6 Class-level direct-members semantics (normative) {#part-9-2-6}

For a class annotated with `objc_direct_members`:

- each method declared in the class interface/implementation is treated as if annotated `objc_direct`, unless explicitly annotated `objc_dynamic`,
- synthesized property accessors for properties declared in that class are treated the same way,
- category declarations are not implicitly covered by the class-level default.

Legality is identical to explicit `objc_direct`:

- overriding an effectively-direct method is ill-formed,
- introducing/replacing an effectively-direct selector from a category is ill-formed.

## 9.3 Final methods and classes {#part-9-3}

### 9.3.1 Concept {#part-9-3-1}

A _final_ declaration prohibits overriding/subclassing but does not necessarily remove the declaration from dynamic dispatch.

Final is primarily a **reasoning and optimization** tool:

- the compiler may devirtualize calls where it can prove the receiver type,
- but dynamic lookup semantics remain observable unless paired with `objc_direct`.

### 9.3.2 Canonical spelling {#part-9-3-2}

Final declarations use:

```c
__attribute__((objc_final))
```

applied to:

- classes, to prohibit subclassing (subject to module sealing rules), and/or
- methods, to prohibit overriding.

### 9.3.3 Semantics (normative) {#part-9-3-3}

- A `objc_final` class shall not be subclassed in any translation unit that sees the declaration.
- A `objc_final` method shall not be overridden by any subclass declaration.

Violations are ill-formed.

## 9.4 Sealed classes {#part-9-4}

### 9.4.1 Concept {#part-9-4-1}

A _sealed_ class prohibits subclassing **outside** the defining module, but may allow subclassing **inside** the module.

This enables:

- whole-module reasoning about the subclass set, and
- optimization across module boundaries while still supporting internal extension points.

### 9.4.2 Canonical spelling {#part-9-4-2}

Sealed classes use:

```c
__attribute__((objc_sealed))
```

Toolchains may treat existing equivalent attributes (e.g., “subclassing restricted”) as aliases.

### 9.4.3 Semantics (normative) {#part-9-4-3}

- Subclassing a sealed class outside the owning module is ill-formed.
- Whether same-module subclassing is permitted is implementation-defined unless the attribute explicitly specifies (future extension).

At minimum, sealed shall be strong enough to support the “no external subclassing” contract.

## 9.5 Interaction with categories, swizzling, and reflection {#part-9-5}

### 9.5.1 Categories {#part-9-5-1}

- Categories may add methods to any class by default (baseline Objective‑C behavior).
- Categories shall not add or replace a `objc_direct` method selector; doing so is ill-formed.

### 9.5.2 Swizzling and IMP replacement (non-normative guidance) {#part-9-5-2}

Swizzling is a common dynamic technique. Objective‑C 3.0 does not outlaw it, but it makes the tradeoff explicit:

- Do not mark APIs `objc_direct` if swizzling/forwarding must be supported.
- `objc_final` and `objc_sealed` do not prevent swizzling by themselves; they primarily constrain subclassing/overriding.

### 9.5.3 Force-dynamic dispatch attribute (normative) {#part-9-5-3}

Objective‑C 3.0 v1 standardizes:

```c
__attribute__((objc_dynamic))
```

applied to Objective‑C method declarations.

Semantics:

- calls that statically reference an `objc_dynamic` method shall use selector-based dynamic dispatch (`objc_msgSend`-family),
- `objc_dynamic` methods participate in forwarding, swizzling, and selector-based reflection,
- `objc_dynamic` is an explicit opt-out from `objc_direct_members` for that method.

Interaction with `direct`/`final`/`sealed`:

- `objc_dynamic` and `objc_direct` on the same method are ill-formed,
- `objc_dynamic` does not relax `objc_final` method/class legality rules,
- `objc_dynamic` does not relax `objc_sealed` cross-module subclassing restrictions.

Example: an `objc_final objc_dynamic` method is dynamically dispatched but still cannot be overridden.

## 9.6 ABI and visibility rules (normative for implementations) {#part-9-6}

Implementations shall ensure that direct/final/sealed/dynamic controls remain safe under:

- separate compilation,
- LTO and non-LTO builds,
- and module boundaries.

Minimum requirements:

1. Attributes that affect dispatch legality (`objc_direct`, `objc_direct_members`, `objc_dynamic`) must be recorded in module metadata.
2. Interface emission must preserve the attributes using canonical spellings ([B.7](#b-7)).
3. Calling or redeclaring a method with incompatible assumptions about directness/dynamicness/finality across module boundaries must be diagnosed when possible ([Part 12](#part-12)).

## 9.7 Required diagnostics (minimum) {#part-9-7}

- Declaring `objc_direct` on a method that is declared in an Objective‑C protocol and implemented dynamically: error (direct methods are not dispatchable by selector).
- Overriding a `objc_direct` or `objc_final` method: error.
- Overriding a method that is effectively direct via `objc_direct_members`: error.
- Declaring a category method with the same selector as a `objc_direct` method: error.
- Declaring both `objc_direct` and `objc_dynamic` on the same method: error.
- Attempting to subclass an `objc_final` class: error.
- Attempting to subclass an `objc_sealed` class from another module: error.
- In strict performance mode: warn when a direct-call-capable reference is forced through dynamic dispatch (e.g., via `performSelector:`), with note explaining semantics.

## 9.8 Open issues {#part-9-8}

- None currently for v1 dispatch controls in this part.

## 9.9 Conformance tests (minimum) {#part-9-9}

A conforming implementation’s suite shall include at least:

### 9.9.1 Direct-members legality and cross-module preservation {#part-9-9-1}

- `PERF-DIRMEM-01` (same-module override legality): subclass override of a method made direct by `objc_direct_members` is diagnosed as ill-formed.
- `PERF-DIRMEM-02` (same-module opt-out): a method explicitly marked `objc_dynamic` inside an `objc_direct_members` class is not treated as direct; override legality follows normal plus `objc_final` rules.
- `PERF-DIRMEM-03` (cross-module override legality): module A exports an `objc_direct_members` class; module B attempts to override an effectively-direct member and receives an error.
- `PERF-DIRMEM-04` (cross-module round-trip): emitted interface/import round-trip preserves effective directness for members declared under `objc_direct_members`, including `objc_dynamic` opt-outs.

### 9.9.2 Force-dynamic dispatch behavior {#part-9-9-2}

- `PERF-DYN-01` (dispatch path): calls to `objc_dynamic` methods lower through dynamic selector dispatch rather than direct-call lowering.
- `PERF-DYN-02` (mixed class behavior): in an `objc_direct_members` class, unannotated members lower as direct while `objc_dynamic` members remain dynamic.
- `PERF-DYN-03` (attribute conflict): `objc_direct` + `objc_dynamic` on the same method is rejected.
- `PERF-DYN-04` (final/sealed interaction): `objc_final objc_dynamic` methods remain non-overridable but dynamically dispatched; `objc_sealed` still rejects out-of-module subclassing regardless of method dispatch mode.



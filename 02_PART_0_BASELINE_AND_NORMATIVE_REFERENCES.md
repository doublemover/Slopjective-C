# Part 0 — Baseline and Normative References

## 0.1 Purpose
This part establishes:
1. The baseline language and de-facto dialect that Objective‑C 3.0 builds upon.
2. How this specification relates to existing compiler specifications and platform ABIs.
3. Common terms used throughout the specification.

Objective‑C 3.0 is defined as a *language mode* that extends a stable baseline. The baseline is the Objective‑C language as implemented by a conforming compiler, including widely deployed features (ARC, blocks, modules, nullability annotations, etc.).

## 0.2 Normative reference model
A specification may reference external documents in two ways:

- **Normative reference**: the referenced document defines required behavior as if included here.
- **Informative reference**: the reference is explanatory or historical and does not define requirements.

This draft treats certain de-facto specifications as normative because they are already written as technical specifications and widely implemented, notably:
- the ARC specification for Objective‑C in Clang (normative for ARC behavior),
- the blocks specification (normative for blocks behavior),
- module import behavior for `@import`,
- nullability qualifier semantics,
- direct methods semantics.

> Open issue: finalize the normative reference list once the ObjC 3.0 core document is published under a single authoritative repository.

## 0.3 Definitions
### 0.3.1 Translation unit
A translation unit is a single source file after preprocessing and module import processing, compiled as one unit.

### 0.3.2 Objective‑C 3.0 mode
Objective‑C 3.0 mode is the compilation mode in which the grammar, defaults, and diagnostics of Objective‑C 3.0 are enabled (see Part 1).

### 0.3.3 Conformance level
A conformance level is a set of additional requirements and diagnostics applied on top of the language mode (see Part 1).

### 0.3.4 Retainable pointer type
A retainable pointer type is a type for which ARC may insert retain/release operations. This includes:
- Objective‑C object pointers (`id`, `Class`, `NSObject *`, etc.),
- block pointers,
- additional “retainable families” declared by annotations (see Part 8).

### 0.3.5 Owner type vs borrowed type
- An **owner** is a value that controls a lifetime (e.g., a strong reference or a resource handle).
- A **borrowed** value is a value that is only valid while an owner remains alive (e.g., interior pointer views).

Objective‑C 3.0 introduces first-class borrowing annotations for interior pointers (Part 8) but does not attempt a universal borrow checker.

## 0.4 Baseline features (summary)
A conforming ObjC 3.0 implementation shall implement the baseline:
- Objective‑C 2.0 language features (properties, fast enumeration, optional protocol requirements, etc.).
- Blocks and block capture semantics.
- ARC (including ownership qualifiers and runtime hooks).
- Objective‑C literals and subscripting.
- Modules via `@import`.
- Nullability qualifiers.
- Objective‑C “direct methods” (if the implementation targets platforms where they are supported).

Where the baseline includes compiler extensions, ObjC 3.0 either:
- standardizes them as part of ObjC 3.0, or
- provides an equivalent feature with a stable spelling.

## 0.5 Baseline constraints
### 0.5.1 ABI stability
ObjC 3.0 shall not require ABI changes for existing compiled binaries to interoperate with ObjC 3.0 code. New features shall lower to:
- existing calling conventions, or
- additive runtime/library support that is versioned and optional.

### 0.5.2 Dynamic runtime model preserved
ObjC 3.0 preserves:
- dynamic dispatch via message sends,
- categories and method replacement mechanisms (where supported),
- runtime reflection/introspection.

New optimization and “static” controls are opt-in (Part 9).

## 0.6 Reference section (informative)
The following documents are expected to be referenced normatively or informatively. URLs are provided in code blocks for portability.

```text
Clang ARC technical specification:
https://clang.llvm.org/docs/AutomaticReferenceCounting.html

Clang Blocks specification:
https://clang.llvm.org/docs/BlockLanguageSpec.html

Clang Objective-C literals and subscripting:
https://clang.llvm.org/docs/ObjectiveCLiterals.html

Clang Modules and @import:
https://clang.llvm.org/docs/Modules.html

Clang nullability qualifiers:
https://clang.llvm.org/docs/AttributeReference.html#nullability-attributes
```

> Open issue: finalize which Clang docs are strictly normative vs informative in the ObjC 3.0 repository.

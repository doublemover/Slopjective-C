# Part 0 — Baseline and Normative References
_Working draft v0.10 — last updated 2025-12-28_

## 0.1 Purpose
This part establishes:

1. The baseline language and de‑facto dialect that Objective‑C 3.0 builds upon.
2. How this specification relates to existing compiler specifications and platform ABIs.
3. Common terms and definitions used throughout the specification.
4. The interpretation of normative language (“shall”, “should”, etc.).

Objective‑C 3.0 is defined as a *language mode* that extends a stable baseline. The baseline is the Objective‑C language as implemented by a conforming compiler, including widely deployed features (ARC, blocks, modules, nullability annotations, direct methods, etc.).

## 0.2 Normative reference model
A specification may reference external documents in two ways:

- **Normative reference**: the referenced document defines required behavior as if included here.
- **Informative reference**: explanatory or historical; does not define requirements.

This draft treats certain de‑facto specifications as normative because they are widely implemented and relied upon for ABI correctness:

- The Objective‑C ARC rules as implemented by Clang-family compilers (normative for ARC insertion, qualifiers, and ARC optimizations).
- The Blocks language specification / block ABI (normative for block representation and calling convention).
- Objective‑C runtime message sending semantics and object model (normative for dynamic dispatch behavior).

Where this draft extends or constrains baseline behavior, it does so only in ObjC 3.0 mode (Part 1).

> Note: This draft intentionally avoids rewriting existing ARC/Blocks specs verbatim; it specifies extensions and constraints needed by ObjC 3.0 features.

## 0.3 Normative terms
The terms **shall**, **must**, **shall not**, **must not**, **should**, and **may** are to be interpreted as in RFC-style standards documents.

## 0.4 Definitions and shared terminology

### 0.4.1 Translation unit
A *translation unit* is a single source file after preprocessing and module import processing, compiled as one unit.

### 0.4.2 Objective‑C 3.0 mode
*Objective‑C 3.0 mode* is the compilation mode in which the grammar, defaults, and diagnostics of Objective‑C 3.0 are enabled (Part 1).

### 0.4.3 Conformance level
A *conformance level* is a set of additional requirements and diagnostics applied on top of ObjC 3.0 mode (Part 1).

### 0.4.4 Retainable pointer type
A *retainable pointer type* is a type for which ARC may insert retain/release operations. This includes:
- Objective‑C object pointers (`id`, `Class`, `NSObject *`, etc.),
- block pointers,
- additional “retainable families” declared by annotations (Part 8).

### 0.4.5 Object pointer type
An *object pointer type* is a pointer to an Objective‑C object, including `id` and `Class`. In this draft, “object pointer” excludes CoreFoundation “toll-free bridged” pointers unless explicitly annotated as retainable families (Part 8).

### 0.4.6 Block pointer type
A *block pointer type* is a pointer to a block literal/closure as defined by the Blocks ABI.

### 0.4.7 Nullability
*Nullability* is a type property (for object and block pointer types) indicating whether `nil` is a valid value. Nullability kinds are defined in Part 3.

### 0.4.8 Optional type
An *optional type* is written `T?` (sugar) in ObjC 3.0 mode. Optional types are a type-system feature for object and block pointer types (v1), distinct from Objective‑C’s “message to nil returns zero” runtime behavior (Part 3).

### 0.4.9 IUO type
An *implicitly unwrapped optional* type is written `T!` (sugar). In v1, IUO exists primarily as a migration aid and is discouraged in strict modes (Part 3).

### 0.4.10 Carrier type
A *carrier type* is a type that represents either a success value or an alternative early-exit/absence value and participates in propagation (`?`). In v1, carriers include:
- optional types (`T?`), and
- `Result<T, E>` (Part 6).

### 0.4.11 Effects
*Effects* are part of a function’s type and govern call-site obligations. In v1, effects include `throws` (Part 6) and `async` (Part 7).

### 0.4.12 Executor
An *executor* is an abstract scheduler for task continuations (Part 7). Executors may represent:
- the main/UI executor,
- a global background executor,
- or custom executors defined by the runtime/library.

### 0.4.13 Task
A *task* is a unit of asynchronous work managed by the concurrency runtime/library. Tasks may be child tasks or detached tasks and may be cancellable (Part 7).

### 0.4.14 Actor
An *actor* is a concurrency-isolated reference type whose mutable state is protected by isolation rules (Part 7). Cross-actor interactions are mediated by `await` and Sendable-like constraints.

## 0.5 Relationship to C and C++
Objective‑C 3.0 remains a superset of C (and compatible with Objective‑C++) as implemented by the toolchain. Where syntax is extended, it is designed to avoid changing meaning in non‑ObjC3 translation units.

## 0.6 Open issues
- Finalize a public “normative references” index with stable URLs/identifiers once this draft is hosted in a canonical repository.
- Define a formal “abstract machine” section that unifies ordering rules across Parts 3/6/7/8.

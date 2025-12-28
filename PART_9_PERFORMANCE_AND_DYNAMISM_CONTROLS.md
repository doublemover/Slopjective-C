# Part 9 — Performance and Dynamism Controls
_Working draft v0.10 — last updated 2025-12-28_

## 9.1 Purpose
Objective‑C’s dynamic dispatch is a strength, but “open world” dynamism can inhibit optimization and complicate reasoning about safety.

Objective‑C 3.0 provides **opt-in** controls that:
- enable direct calls and devirtualization where safe,
- communicate intent to compilers and reviewers,
- preserve dynamic behavior by default,
- remain safe under separate compilation and across module boundaries (C.2).

This part is designed to accommodate both Cocoa-style dynamic patterns *and* system/library subsets where predictability and performance matter.

## 9.2 Direct methods

### 9.2.1 Concept
A *direct method* is a method that:
- is not dynamically dispatched by selector lookup,
- cannot be overridden,
- and is intended to be invoked via a direct call when statically referenced.

### 9.2.2 Canonical spelling
A direct method is declared with:

```c
__attribute__((objc_direct))
```

applied to an Objective‑C method declaration.

Toolchains may additionally support class-level defaults (e.g., “all members direct”) as an extension, but such defaults must be representable in emitted interfaces (B.7).

### 9.2.3 Semantics (normative)
1. A direct method shall not be overridden in any subclass.
2. A direct method shall not be introduced or replaced by a category with the same selector.
3. A direct method shall not participate in message forwarding semantics (`forwardInvocation:`) as part of selector lookup.

Calling model:
- A direct method call shall be formed only by a *statically-resolved* method reference (i.e., the compiler must know it targets a direct method declaration).
- Dynamic message sends (e.g., `objc_msgSend`, `performSelector:`) shall not be relied upon to find a direct method.

**Programmer model:** “If you want dynamic, do not mark it direct.”

### 9.2.4 Separate compilation notes (normative)
Because direct methods change call legality and dispatch surfaces, the `objc_direct` attribute shall be:
- recorded in module metadata (see D.3.1 Table A), and
- preserved in emitted interfaces.

Effect/attribute mismatches across modules are ill-formed (C.2).

### 9.2.5 Recommended lowering (informative)
Recommended lowering is described in C.8.
At a high level:
- the compiler emits a callable symbol for the method implementation, and
- calls to the method use a direct call rather than `objc_msgSend`.

## 9.3 Final methods and classes

### 9.3.1 Concept
A *final* declaration prohibits overriding/subclassing but does not necessarily remove the declaration from dynamic dispatch.

Final is primarily a **reasoning and optimization** tool:
- the compiler may devirtualize calls where it can prove the receiver type,
- but dynamic lookup semantics remain observable unless paired with `objc_direct`.

### 9.3.2 Canonical spelling
Final declarations use:

```c
__attribute__((objc_final))
```

applied to:
- classes, to prohibit subclassing (subject to module sealing rules), and/or
- methods, to prohibit overriding.

### 9.3.3 Semantics (normative)
- A `objc_final` class shall not be subclassed in any translation unit that sees the declaration.
- A `objc_final` method shall not be overridden by any subclass declaration.

Violations are ill-formed.

## 9.4 Sealed classes

### 9.4.1 Concept
A *sealed* class prohibits subclassing **outside** the defining module, but may allow subclassing **inside** the module.

This enables:
- whole-module reasoning about the subclass set, and
- optimization across module boundaries while still supporting internal extension points.

### 9.4.2 Canonical spelling
Sealed classes use:

```c
__attribute__((objc_sealed))
```

Toolchains may treat existing equivalent attributes (e.g., “subclassing restricted”) as aliases.

### 9.4.3 Semantics (normative)
- Subclassing a sealed class outside the owning module is ill-formed.
- Whether same-module subclassing is permitted is implementation-defined unless the attribute explicitly specifies (future extension).

At minimum, sealed shall be strong enough to support the “no external subclassing” contract.

## 9.5 Interaction with categories, swizzling, and reflection

### 9.5.1 Categories
- Categories may add methods to any class by default (baseline Objective‑C behavior).
- Categories shall not add or replace a `objc_direct` method selector; doing so is ill-formed.

### 9.5.2 Swizzling and IMP replacement (non-normative guidance)
Swizzling is a common dynamic technique. Objective‑C 3.0 does not outlaw it, but it makes the tradeoff explicit:
- Do not mark APIs `objc_direct` if swizzling/forwarding must be supported.
- `objc_final` and `objc_sealed` do not prevent swizzling by themselves; they primarily constrain subclassing/overriding.

## 9.6 ABI and visibility rules (normative for implementations)
Implementations shall ensure that direct/final/sealed remain safe under:
- separate compilation,
- LTO and non-LTO builds,
- and module boundaries.

Minimum requirements:
1. Attributes that affect dispatch legality (`objc_direct`) must be recorded in module metadata.
2. Interface emission must preserve the attributes using canonical spellings (B.7).
3. Calling a method with incompatible assumptions about directness/finality across module boundaries must be diagnosed when possible (Part 12).

## 9.7 Required diagnostics (minimum)
- Declaring `objc_direct` on a method that is declared in an Objective‑C protocol and implemented dynamically: error (direct methods are not dispatchable by selector).
- Overriding a `objc_direct` or `objc_final` method: error.
- Declaring a category method with the same selector as a `objc_direct` method: error.
- Attempting to subclass an `objc_final` class: error.
- Attempting to subclass an `objc_sealed` class from another module: error.
- In strict performance mode: warn when a direct-call-capable reference is forced through dynamic dispatch (e.g., via `performSelector:`), with note explaining semantics.

## 9.8 Open issues
- Whether to standardize a class-level “all members direct” attribute as part of v1 (or leave as implementation extension).
- Whether to standardize an attribute that *forces* dynamic dispatch (useful for debugging and swizzling-friendly APIs).

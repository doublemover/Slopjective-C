# Part 11 — Interoperability: C, C++, and Swift
_Working draft v0.6 — last updated 2025-12-28_

## 11.1 Purpose
Objective‑C’s ecosystem depends on interoperability:
- with **C** (system APIs),
- with **C++** (ObjC++ and mixed libraries),
- and with **Swift** (modern application frameworks).

This part defines interoperability rules so ObjC 3.0 features can be adopted without breaking the existing world.

A key principle is that ObjC 3.0 features shall:
- preserve existing ABIs where possible,
- and provide “overlay” forms that improve ergonomics without changing underlying calling conventions.

## 11.2 C interoperability

### 11.2.1 ABI compatibility (normative)
ObjC 3.0 shall preserve C ABI calling conventions:
- function call ABI and struct layout remain C ABI,
- ObjC 3.0 effects (`async`, `throws`) do not change the ABI of existing C functions.

### 11.2.2 Header annotations for semantics
C APIs may be annotated with:
- ownership transfer (`objc_consumed`, `objc_returns_owned`) (Part 4/8),
- error model annotations (NSError-out, status code mapping) (Part 6),
- cleanup requirements for handles/resources (Part 8),
- borrowed relationships (Part 8),
- completion-handler annotations enabling async overlays (this part).

These annotations are expressed as canonical `__attribute__((...))` spellings (Decision D‑007).

### 11.2.3 Overlay imports (informative but required capability)
Toolchains may present C APIs to ObjC 3.0 clients using overlays:
- **`throws` overlay** for functions with `NSError **` out or status-code annotations,
- **`Result` overlay** for status-code APIs,
- **`async` overlay** for completion-handler APIs.

Overlays shall not change the underlying ABI: they are source-level conveniences that lower to the original call pattern.

### 11.2.4 Completion-handler to `async` mapping (guidance)
A completion-handler API is eligible for an `async` overlay when:
- it has a completion parameter that is a block,
- the block is invoked exactly once along all paths (or the API is annotated to promise this),
- the completion carries either:
  - (value, error) style, or
  - status code style.

A conforming toolchain should support an annotation to state the “exactly once” completion contract.

> Open issue: finalize canonical attribute spellings for completion contracts (e.g., `objc_async_completion`).

## 11.3 C++ interoperability (ObjC++)

### 11.3.1 Mixed translation units
ObjC 3.0 features must remain usable in ObjC++ translation units, subject to grammar conflict resolution.

Because ObjC++ already reserves many tokens, ObjC 3.0 constructs that must appear in headers should prefer attribute spellings (Decision D‑007).

### 11.3.2 Exceptions and unwinding
Where C++ exceptions are enabled:
- `defer` and resource cleanup shall run during unwinding (Part 5/8),
- mixing C++ exceptions and ObjC `throws` is permitted but requires explicit bridging (this draft does not standardize implicit bridging).

### 11.3.3 RAII integration (informative)
Where C++ destructors exist, `@resource` cleanup may be lowered to RAII constructs, but the semantics must match Part 8 regardless of lowering.

### 11.3.4 Restricted profiles
Some environments (kernels/drivers) restrict C++ features. Profiles may enforce restricted subsets (Part 1/8).

## 11.4 Swift interoperability

### 11.4.1 Nullability and optionals (normative mapping intent)
ObjC 3.0 nullability and optional sugar shall map predictably to Swift optionals:

- nonnull (`T`) ↔ non-optional in Swift
- nullable (`T?`) ↔ optional in Swift
- IUO (`T!`) ↔ implicitly unwrapped optional in Swift (migration-only; discouraged)

Modules should prefer complete nullability annotations so Swift imports are accurate.

### 11.4.2 Errors: `throws`, `Result`, and NSError
- ObjC `throws` imports as Swift `throws`.
- ObjC `Result<T,E>` imports as Swift `Result` where representable.
- NSError-out APIs may be overlaid as `throws` in both directions when annotated (Part 6/11.2).

### 11.4.3 Concurrency: `async/await`, executors, and actors
- ObjC `async` imports as Swift `async`.
- Executor affinity annotations (`objc_executor(main)`) should map to Swift main-actor/main-executor intent where possible.
- ObjC actors map to Swift actors conceptually; cross-language calls obey isolation and Sendable constraints to the extent possible.

> Note: Precise mapping depends on toolchain/runtime integration; this draft specifies the intent and requires that metadata be preserved.

### 11.4.4 Typed key paths
ObjC 3.0 typed key paths should import as Swift key paths when representable:
- key paths over Objective‑C properties map naturally,
- key paths over dynamic/KVC-only members may import as string-based fallbacks.

### 11.4.5 Generics
Objective‑C pragmatic generics map to Swift generics as constraints allow:
- `NSArray<NSString *> *` maps to `[String]`-like types where bridging exists,
- generic constraints not representable in Swift are imported conservatively.

### 11.4.6 Module-qualified names
Module-qualified names (`@Module.Name`, Part 2) are primarily a source-level disambiguation feature in Objective‑C. Swift uses module qualification already; toolchains should map ObjC module qualification to Swift’s module qualification without introducing new runtime artifacts.

## 11.5 Open issues
- Finalize completion-handler annotations for async overlays (exactly-once, error position, cancellation semantics).
- Define how strict nullability completeness in ObjC modules affects Swift import defaults.
- Define a stable “interface file” format that preserves ObjC 3.0 metadata across distribution (Part 2).

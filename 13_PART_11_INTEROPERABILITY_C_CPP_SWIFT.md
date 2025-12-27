# Part 11 — Interoperability: C, C++, and Swift

## 11.1 Purpose
Objective‑C’s success depends on interop:
- with C (system APIs),
- with C++ (ObjC++),
- and with Swift (modern app frameworks).

This part defines interoperability rules so ObjC 3.0 features can be adopted without breaking the ecosystem.

## 11.2 C interoperability
### 11.2.1 ABI compatibility
ObjC 3.0 shall preserve C ABI calling conventions.

### 11.2.2 Ownership annotations in headers
C APIs may be annotated with:
- ownership transfer (`@consumes`, `@returns_owned`)
- resource cleanup requirements (Part 8)
- return-code-to-error mapping (Part 6)
- borrowed return relationships (Part 8)

### 11.2.3 Importing C APIs into ObjC 3.0 surfaces
Tooling may offer “overlay imports” that present C APIs as:
- `throws`,
- `Result`,
- or `async` forms where annotations allow.

These overlays must not change the underlying ABI.

## 11.3 C++ interoperability (ObjC++)
### 11.3.1 Mixed compilation unit rules
ObjC 3.0 features must remain usable in ObjC++ translation units, subject to:
- grammar conflict resolution,
- and profile restrictions (system/kernel profiles may restrict C++ features).

### 11.3.2 Resource handles and RAII
Where C++ destructors are available, `@resource` may lower to RAII, but semantics must match Part 8 regardless of lowering.

### 11.3.3 Profiles for restricted environments
The spec allows profiles that enforce restricted C++ subsets for environments like drivers/kernels.
(Defined as an extension point; detailed profile rules may live in Part 8 or a future part.)

## 11.4 Swift interoperability
### 11.4.1 Nullability and optionals
ObjC 3.0 nullability and optional sugar must map predictably to Swift optionals:
- nonnull ↔ non-optional
- nullable ↔ optional
- null-unspecified ↔ imported as optional or IUO depending on context (toolchain policy)

### 11.4.2 Errors
NSError-style and `throws`/`Result` should interoperate:
- ObjC `throws` imports as Swift `throws`.
- ObjC `Result<T,E>` imports as Swift `Result` where possible.

### 11.4.3 Concurrency
ObjC `async` imports as Swift `async`.
Completion-handler APIs annotated appropriately can import/export across languages safely.

### 11.4.4 Actors and isolation
Actor annotations must map:
- `@MainActor`/main executor semantics,
- sendable-like constraints for cross-language boundaries.

## 11.5 Open issues
- Exact mapping rules for “strict” nullability completeness in mixed-language modules.
- How to express Swift-only features (e.g., generics constraints) in ObjC headers without losing intent.

# Part 9 — Performance and Dynamism Controls
_Working draft v0.7 — last updated 2025-12-28_

## 9.1 Purpose
Objective‑C’s dynamic dispatch is a strength, but it inhibits optimization when everything is “open world”:
- any method might be swizzled,
- categories can replace implementations,
- message forwarding may intercept selectors.

Objective‑C 3.0 provides **opt‑in** controls that:
- enable devirtualization/direct calls where safe,
- make “this must be fast” intent explicit and reviewable,
- preserve baseline dynamic behavior by default.

This part is intentionally conservative: it does not outlaw swizzling globally; it provides explicit boundaries where optimizers and humans can rely on stronger guarantees.

## 9.2 Direct methods

### 9.2.1 Concept
A *direct method* is a method that:
- is not dynamically dispatched through `objc_msgSend` at call sites that can see its declaration,
- cannot be overridden by subclasses,
- is not dynamically replaced by categories outside the defining compilation boundary.

Direct methods exist to support:
- leaf performance hot paths,
- low-level frameworks that want predictable dispatch cost,
- and tighter inlining opportunities.

### 9.2.2 Canonical spelling
This draft defines an ObjC 3.0 surface spelling:

```objc
@direct - (int)foo;
```

**Canonical header spelling (illustrative, Decision D‑007):**
- `__attribute__((objc_direct))` on the method declaration.

Toolchains may treat `@direct` as sugar for the attribute in ObjC 3.0 mode.

### 9.2.3 Static semantics (normative)
- A direct method shall not be overridden.
- A direct method shall not be implemented in a category outside its defining module boundary.
- Taking a selector and sending it dynamically remains permitted, but such a send is not required to reach the direct implementation (see §9.2.5).

### 9.2.4 Dynamic semantics (model)
For a call that is statically known to target a direct method, the implementation shall:
- lower the call as a direct function call (or equivalent direct-call mechanism),
- and may inline/devirtualize as permitted by optimization.

### 9.2.5 Interactions with selector-based dynamism
Directness is a performance contract, not a change to the runtime’s selector model.
Therefore:
- the existence of a direct method does not remove the selector from the runtime,
- but a dynamic message send (`objc_msgSend`) is not guaranteed to land on the direct implementation.

If a framework requires both:
- direct-call performance internally, and
- selector-visible behavior externally,
it should expose a separate selector-visible wrapper method that forwards to the direct implementation.

## 9.3 `final` methods and `sealed` classes

### 9.3.1 `final` methods
A `final` method cannot be overridden in subclasses.
Calls to a final method may be devirtualized by the compiler when the static type is known.

Canonical spelling (Decision D‑010):
```objc
- (void)tick __attribute__((objc_final));
```

This attribute declares that overriding the method is forbidden outside the allowed boundary defined by the containing type’s rules.

> Note: A toolchain may optionally accept `@final` as sugar, but such sugar is not required in v1.

### 9.3.2 `sealed` / subclassing restrictions
A sealed class may be subclassed only within a constrained boundary (typically the defining module).

Canonical spelling (Decision D‑010):
```objc
__attribute__((objc_sealed))
@interface MyClass : NSObject
@end
```

> Note: A toolchain may optionally accept `@sealed` as sugar, but such sugar is not required in v1.

Canonical header spelling (illustrative):
- `__attribute__((objc_subclassing_restricted))` on the class.

A conforming implementation shall diagnose illegal subclassing in strict modes.

### 9.3.3 Friend modules (future extension)
A future revision may allow “friend modules” for sealed classes.

## 9.4 `@direct_members` for bulk directness (optional)
Framework authors often want an entire class’s methods to be direct by default.

Surface spelling (v1 spelling):
```objc
@direct_members
@interface FastThing : NSObject
@end
```

Canonical header spelling (illustrative):
- `__attribute__((objc_direct_members))` on the class.

Semantics:
- methods declared in the class interface are treated as direct unless explicitly declared otherwise.

This feature is optional in v1.

## 9.5 Static and dynamic regions

### 9.5.1 Motivation
Some codebases rely heavily on swizzling and forwarding; other codebases want to declare “no runtime replacement here.”

Objective‑C 3.0 defines region markers that allow a module or translation unit to state its intent.

### 9.5.2 Region markers (optional optimization hint)
A conforming implementation may provide pragmas to express dispatch intent for a region of code (see **01B_ATTRIBUTE_AND_SYNTAX_CATALOG.md**):

- `#pragma objc3 dispatch dynamic`
- `#pragma objc3 dispatch static`

These pragmas are optimization hints only; correctness shall not depend on them.

In a **static region**, the compiler is permitted to assume that:
- methods/classes marked with Part 9 controls are not dynamically replaced by external code,
- and therefore optimizations that depend on that assumption are allowed.

In a **dynamic region**, baseline Objective‑C runtime replacement rules apply.

### 9.5.3 Conformance behavior
- In permissive mode, region markers primarily affect optimization and warnings.
- In strict performance checking mode (if provided), violating a static region’s assumptions may be diagnosed (compile-time or runtime debug checks).

> Note: Because runtime replacement detection is platform-dependent, this specification does not require a specific enforcement strategy in v1.

## 9.6 ABI and visibility rules (normative intent)
Direct/final/sealed features must remain safe under separate compilation. Therefore:
- A compiler shall not assume “closed world” unless it has a declared boundary (sealed/static region/module visibility) or whole-program knowledge.
- A compiler may exploit whole-program/LTO when available, but shall not require it.

## 9.7 Required diagnostics
Minimum diagnostics include:
- overriding a `final` method (error),
- implementing a `@direct` method in an out-of-boundary category (error in strict modes),
- declaring a method direct but also exposing it as overridable (error),
- warnings for calling patterns that defeat directness (e.g., taking selector and dynamically sending it) in strict performance checking mode (if available).

## 9.8 Open issues
- Final surface spelling for `@final` and `@sealed` (keywords vs attributes).
- Interaction rules with categories in the same module vs different modules.
- Whether to standardize a debug-only runtime check mechanism for static regions.

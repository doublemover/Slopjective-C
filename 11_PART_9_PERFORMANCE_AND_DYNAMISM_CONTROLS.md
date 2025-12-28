# Part 9 — Performance and Dynamism Controls
_Working draft v0.6 — last updated 2025-12-28_

## 9.1 Purpose
Objective‑C’s dynamic dispatch is a strength, but it inhibits optimization when everything is “open world.”

Objective‑C 3.0 provides opt‑in controls that:
- allow direct calls and devirtualization where safe,
- communicate intent to the compiler and to reviewers,
- preserve dynamic behavior by default,
- make “this can be swizzled / overridden” vs “this is performance‑critical and closed” an explicit choice.

This part is intentionally conservative: these features must remain safe under separate compilation and must not require whole‑program analysis to preserve correctness.

---

## 9.2 Model: dynamic by default, closed by explicit promise
By default, Objective‑C methods:
- are dynamically dispatched,
- are overridable by subclasses,
- and may be replaced by categories and runtime method replacement mechanisms.

ObjC 3.0 adds *promises* that restrict one or more of those capabilities for specific declarations. The promise enables the compiler to generate faster code, but the promise must be checkable (diagnosable) at module boundaries.

---

## 9.3 Direct methods

### 9.3.1 Concept
A **direct method** is a method that:
- cannot be overridden by subclasses, and
- does not permit replacement by categories within the same module interface, and
- may be called by the compiler as a direct function call (bypassing `objc_msgSend`) when the static receiver type is known.

Direct methods are intended for:
- performance-critical internals,
- methods that are part of a “closed” implementation strategy,
- bridging shims where dynamic dispatch is undesirable.

### 9.3.2 Spelling (provisional)
This draft uses a Clang-compatible attribute spelling as the canonical form:

```objc
- (int)foo __attribute__((objc_direct));
```

Sugar spellings (e.g., `@direct`) are explicitly out of scope for v1 and may be added later.

### 9.3.3 Semantics (normative)
- A direct method shall not be overridden by a method of the same selector in any subclass that is visible within the same translation unit or module.
- A category or extension that declares a method with the same selector as a direct method on the same class is ill‑formed (at least in strict mode; permissive mode may warn with fix‑its).

Dynamic message sends to a selector that names a direct method are permitted, but:
- implementations may still choose to dispatch dynamically for such calls, and
- optimizations are permitted only when they preserve the semantics of a direct method (i.e., no override).

### 9.3.4 Interaction with runtime method replacement (informative)
Direct methods are intended to be “not swizzled.” However, fully preventing runtime replacement is outside the language’s static control.

Therefore:
- Attempts to replace a direct method via runtime APIs are **unspecified behavior** in this draft (toolchains/runtimes may ignore replacement, trap in debug, or allow it with loss of optimization).
- Toolchains should offer a diagnostic mode that warns on known replacement calls targeting direct methods.

> Open issue: should this be strengthened to “ill‑formed, no diagnostic required” or “undefined behavior” for security/perf reasons?

---

## 9.4 `final` methods and classes

### 9.4.1 Concept
A **final method** may be dynamically dispatched but is promised not to be overridden. A **final class** may not be subclassed.

Final declarations allow:
- devirtualization within the compilation unit,
- better inlining and ARC optimization,
- clearer API intent (this is not an extension point).

### 9.4.2 Spelling (provisional)
Canonical attribute spellings (provisional):

```objc
- (id)parse __attribute__((objc_final));
__attribute__((objc_final)) @interface Parser : NSObject
@end
```

### 9.4.3 Semantics (normative)
- Overriding a final method is ill‑formed.
- Subclassing a final class is ill‑formed.
- Categories may add methods to a final class (this does not violate finality), but categories shall not replace existing final methods (diagnosed where possible).

---

## 9.5 `sealed` declarations (module‑closed extension points)

### 9.5.1 Concept
A **sealed class** may be subclassed only within the defining module. Outside the module, it behaves like final.

Sealed enables:
- optimizations that rely on a closed set of subclasses within a module,
- explicit “you may extend this, but only here” boundaries.

### 9.5.2 Spelling (provisional)
```objc
__attribute__((objc_sealed)) @interface Token : NSObject
@end
```

### 9.5.3 Semantics (normative)
- Within the defining module, subclassing a sealed class is permitted.
- Outside the defining module, subclassing a sealed class is ill‑formed.
- The defining module’s extracted interface (Part 2) must preserve sealed/final/direct intent.

---

## 9.6 Explicit dynamic boundaries

### 9.6.1 Purpose
Some code intentionally relies on dynamic features (method replacement, message forwarding, KVC/KVO). ObjC 3.0 allows authors to make that explicit.

### 9.6.2 Spelling (provisional)
A method may be marked as “force dynamic”:

```objc
- (id)valueForKey:(NSString *)key __attribute__((objc_force_dynamic));
```

### 9.6.3 Semantics (normative)
When a declaration is marked force-dynamic:
- the compiler shall not devirtualize or direct-call that method based solely on static type knowledge,
- even if the method would otherwise qualify for a direct/final optimization.

This attribute exists to make performance tradeoffs explicit and to support tooling (profilers, analyzers).

---

## 9.7 Diagnostics
A conforming implementation shall diagnose:
- overriding a `final`/`direct` method (error in strict),
- subclassing a `final` class (error in strict),
- subclassing a `sealed` class from outside its defining module (error in strict),
- duplicate category method declarations that conflict with `direct`/`final` constraints (at least warning in permissive).

Toolchains should additionally warn when:
- a `direct` method is invoked through dynamic dispatch in performance-sensitive builds (warning group),
- a declaration is marked `direct`/`final` but is not eligible for any optimization due to signature/runtime constraints (QoI warning).

---

## 9.8 Open issues
- Final surface spelling and alignment with existing Clang/Apple attributes where possible.
- Stronger guarantees (or explicit UB) for runtime method replacement targeting `direct` methods.
- Interaction with Swift overlays and resilience: how to preserve sealed/final intent across language boundaries.

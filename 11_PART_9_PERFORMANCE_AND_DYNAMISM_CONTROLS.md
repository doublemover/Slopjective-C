# Part 9 — Performance and Dynamism Controls

## 9.1 Purpose
Objective‑C’s dynamic dispatch is a strength, but it inhibits optimization when everything is “open world.”
Objective‑C 3.0 provides opt-in controls that:
- allow direct calls and devirtualization where safe,
- communicate intent to the compiler and to code reviewers,
- preserve dynamic behavior by default.

## 9.2 Direct methods
### 9.2.1 Concept
A direct method is a method that:
- cannot be overridden,
- cannot participate in message forwarding/swizzling behavior in the same way,
- may be called directly as a function (bypassing objc_msgSend).

### 9.2.2 Syntax (provisional)
```objc
@direct - (int)foo;
```

### 9.2.3 Semantics
- Calls to direct methods shall be statically resolved.
- The implementation may inline direct calls.
- Taking the selector and sending it dynamically is permitted but may not reach the direct implementation unless explicitly bridged.

### 9.2.4 Constraints
- A direct method cannot be declared in a category outside the defining module (or must obey strict rules), to preserve linkage and visibility semantics.

## 9.3 `final` methods and `sealed` classes
### 9.3.1 `final`
A final method cannot be overridden in subclasses.
The compiler may devirtualize calls to final methods.

### 9.3.2 `sealed`
A sealed class can only be subclassed within the defining module (or a defined set of friend modules).
This enables stronger optimization across module boundaries.

## 9.4 Static vs dynamic regions
Objective‑C 3.0 introduces an attribute/pragma (provisional):
- `@dynamic_region` / `@static_region`

In a static region:
- the compiler is permitted to assume no swizzling/forwarding changes for marked methods/classes.
- violations (e.g., runtime replacement detected) are undefined behavior in strict performance mode.

In a dynamic region:
- normal Objective‑C runtime rules apply.

> Open issue: whether to provide runtime checks in debug builds to detect violations.

## 9.5 ABI and visibility rules
Direct/final/sealed must be specified with respect to:
- symbol visibility
- LTO (link-time optimization)
- module boundaries

The spec shall require that these features remain safe under separate compilation.

## 9.6 Diagnostics
- Calling a direct method via dynamic dispatch should be warned in strict performance mode.
- Declaring a method direct but also exposing it as overridable is ill-formed.

## 9.7 Open issues
- Exact syntax spelling and mapping to existing compiler attributes.
- Interaction with categories and method replacement tools.

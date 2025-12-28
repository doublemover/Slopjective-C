# Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths
_Working draft v0.8 — last updated 2025-12-28_

## 3.0 Overview

### v0.8 resolved decisions
- Optional chaining and optional message sends are **reference-only** in v1 (object/block/void only). Scalar/struct optional chaining is ill-formed.
- Optional message sends are **conditional calls**: argument expressions are evaluated only if the receiver is non-`nil`.
- Generic methods/functions are **deferred** in v1; generic *types* remain supported.
- Canonical nullability default regions use `#pragma objc assume_nonnull begin/end` (01B).

Objective‑C 3.0 improves type safety without abandoning Objective‑C’s model:

- **Nullability** becomes a core, enforceable type property, with module/region defaults.
- **Optionals** provide explicit “maybe” semantics with ergonomic binding (`if let`, `guard let`) and explicit optional access.
- **Pragmatic generics** extend today’s lightweight generics into a more uniform system that can express generic APIs and constraints while remaining ABI-compatible.
- **Typed key paths** provide compiler-checked alternatives to stringly typed KVC/KVO-style access.

This part defines **syntax**, **static semantics**, **dynamic semantics**, and **required diagnostics** for these features.

---

## 3.1 Lexical and grammar additions

### 3.1.1 New keywords
In Objective‑C 3.0 mode, the following keywords are reserved by this part:

- `guard`, `let`, `var`

(Additional reserved keywords are specified in Part 1.)

### 3.1.2 New punctuators
This part introduces new punctuators in specific grammatical contexts:

- `?` and `!` as **type suffixes** in declarations (optional sugar)
- `?.` as an **optional member access** token in expressions
- `??` as a **nil-coalescing operator** in expressions

The tokens shall be recognized only where specified by the grammar; in all other contexts, existing C/Objective‑C meaning applies.

---

## 3.2 Nullability

### 3.2.1 Nullability kinds
Objective‑C 3.0 recognizes the following nullability kinds for Objective‑C object pointer types and block pointer types:

- **Nonnull**: value is not `nil`.
- **Nullable**: value may be `nil`.
- **Unspecified**: nullability is unknown (legacy).

The **unspecified** kind exists only for compatibility and shall be treated as “potentially nil” for safety diagnostics.

### 3.2.2 Where nullability applies
Nullability applies to:
- Objective‑C object pointer types (`id`, `Class`, `NSObject *`, `NSString *`, `id<Proto>`, etc.)
- block pointer types (e.g., `void (^)(void)`)

Nullability does **not** apply directly to:
- scalar types (`int`, `BOOL`, `NSInteger`, etc.)
- pointers that are not object/block pointers (`int *`, `void *`, etc.)

(Those may be covered by separate annotations; out of scope for this part.)

### 3.2.3 Nullability spelling forms
Objective‑C 3.0 supports three equivalent spelling forms for object/block pointer nullability:

1. **Qualifier form** (baseline spelling):
   - `_Nonnull`, `_Nullable`, `_Null_unspecified` (or implementation-provided equivalents)
2. **Objective‑C contextual keywords** (when supported by implementation):
   - `nonnull`, `nullable`
3. **Optional sugar** (ObjC 3.0):
   - `T?` (nullable)
   - `T!` (implicitly unwrapped optional; see §3.3.3)

#### 3.2.3.1 Canonicalization rule
For type-checking purposes, implementations shall canonicalize:
- `T?` → `T _Nullable`
- `T!` → `T _Nullable` **plus** “implicitly-unwrapped” marker

If a type already includes an explicit nullability qualifier, the sugar form shall be rejected as redundant.

### 3.2.4 Nonnull-by-default regions

#### 3.2.4.1 Region introduction
A translation unit in ObjC 3.0 mode may establish a nonnull-by-default region using one of:

- a module-level default (recommended for frameworks),
- a pragma region, or
- an explicit language directive.

The canonical spelling is the pragma form (01B):

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Implementations may additionally support aliases such as `#pragma clang assume_nonnull begin/end` or `NS_ASSUME_NONNULL_BEGIN/END`, but emitted interfaces shall use the canonical spelling.

#### 3.2.4.2 Region semantics
Inside a nonnull-by-default region:

- Any unannotated object/block pointer type in a **declaration** is treated as **nonnull**.
- `nullable` must be explicitly spelled.

Outside such a region:
- unannotated object/block pointer types are treated as **unspecified**.

> Rationale: this allows headers to become “safe by default” while preserving compatibility with legacy code.

### 3.2.5 Nullability completeness

#### 3.2.5.1 Public interface completeness
A module’s exported interface shall be considered **nullability-complete** if every exported object/block pointer type has an effective nullability (explicit or default-region-derived) that is not “unspecified”.

In **strict** mode (Part 1):
- exporting “unspecified” nullability is ill-formed.

In **permissive** mode:
- it is permitted but diagnosed.

#### 3.2.5.2 Local completeness
Within a translation unit, incomplete nullability is permitted but should be diagnosed when it impacts type safety (e.g., assigning “unspecified” to “nonnull”).

---

## 3.3 Optionals

### 3.3.1 Optional type sugar (`T?`)
A type written with the suffix `?` denotes a nullable object/block pointer type.

Examples:

```objc
NSString*? maybeName;
id<NSCopying>? maybeKey;
void (^? callback)(int);
```

### 3.3.2 Optional binding: `if let` and `guard let`

#### 3.3.2.1 Grammar
```text
if-let-statement:
    'if' let-binding-list compound-statement ('else' compound-statement)?

guard-let-statement:
    'guard' let-binding-list 'else' compound-statement

let-binding-list:
    'let' let-binding (',' let-binding)*
  | 'var' let-binding (',' let-binding)*

let-binding:
    identifier '=' expression
```

#### 3.3.2.2 Semantics
For `if let`:

- Each binding expression is evaluated in source order.
- If any bound value is `nil`, the `then` block is skipped and the `else` block (if present) executes.
- On success, each bound identifier is introduced in the then-scope with **nonnull** type.

For `guard let`:

- Each binding expression is evaluated in source order.
- If any bound value is `nil`, the else block executes.
- The else block **must exit** the current scope by one of:
  - `return`, `throw`, `break`, `continue`, or other scope-exiting statement defined by this spec.
- After the guard, each bound identifier is in scope and has **nonnull** type.

#### 3.3.2.3 `let` vs `var`
- `let` bindings are immutable.
- `var` bindings are mutable.

Implementations shall diagnose mutation of `let` bindings as an error.

### 3.3.3 Implicitly unwrapped optionals (`T!`)
`T!` denotes a nullable value that is **implicitly unwrapped** at use sites in certain contexts.

#### 3.3.3.1 Intended use
`T!` exists primarily for interop boundaries where the API is “logically nonnull” but not yet fully annotated, or where legacy behavior is relied upon.

#### 3.3.3.2 Static semantics
In ObjC 3.0 strict mode:
- implicit unwrapping of `T!` without an explicit proof is diagnosed.
- implementations should offer fix-its to convert to `T?` + binding or add nullability to the API.

#### 3.3.3.3 Dynamic behavior (optional)
An implementation may provide a runtime trap when an implicitly unwrapped optional is observed to be `nil` at a use site, but this is not required by this draft.

> Recommendation: treat IUO primarily as a static-typing aid and migration tool, not as a runtime “bomb”.

### 3.3.4 Nil-coalescing operator (`??`)

#### 3.3.4.1 Grammar
```text
nil-coalescing-expression:
    logical-or-expression
    nil-coalescing-expression '??' nil-coalescing-expression
```

`??` is right-associative.

#### 3.3.4.2 Semantics
`a ?? b` evaluates `a`:
- if `a` is nonnull, yields `a` and does not evaluate `b`;
- otherwise evaluates and yields `b`.

#### 3.3.4.3 Typing
If `a` has type `T?` and `b` has type `T` (nonnull), then `a ?? b` has type `T` (nonnull).

More generally:
- The result type is the least upper bound of the operand types, with nullability resolved to nonnull if the right operand is nonnull and `a` is tested.

In strict mode, `b` must be type-compatible with the unwrapped type of `a`; otherwise error.

---

## 3.4 Nullable receivers and optional access

Objective‑C historically allows messaging `nil` and returns zero/`nil`. ObjC 3.0 preserves this behavior for compatibility but provides **explicit, typed optional access** and enables strict diagnostics.

### 3.4.1 Optional member access: `?.`

#### 3.4.1.1 Grammar
```text
postfix-expression:
    ...
    postfix-expression '?.' identifier
```

#### 3.4.1.2 Semantics
For `x?.p` where `p` resolves to a property access:

- Evaluate `x` once.
- If `x` is `nil`, yield `nil`.
- Otherwise perform the normal property access and yield the value.

#### 3.4.1.3 Typing restrictions
`?.` is permitted only when the property type is an **object/block pointer type**.
- The result type is the property type made nullable (i.e., `T?`).

If the property type is scalar/struct:
- `?.` is ill-formed in strict mode and diagnosed in permissive mode (because there is no scalar optional type in this draft).

### 3.4.2 Optional message send: `[receiver? selector]`

#### 3.4.2.1 Grammar
```text
message-expression:
    '[' receiver-expression optional-send-indicator? message-selector ']'

optional-send-indicator:
    '?'
```

The optional-send indicator applies to the receiver position only.

Example:
```objc
id? x = ...;
NSString*? s = [x? description];
```

#### 3.4.2.2 Semantics
- Evaluate the receiver expression once.
- If receiver is `nil`, the optional send yields:
  - `nil` for object/block pointer returns,
  - no effect for `void` returns.

#### 3.4.2.3 Typing restrictions

#### 3.4.2.4 Argument evaluation (normative)
For an optional message send of the form:

```objc
[receiver? method:arg1 other:arg2]
```

the implementation shall:
1. Evaluate `receiver` exactly once.
2. If `receiver` is `nil`, the expression yields the “nil case” result (§3.4.2.2) and the argument expressions `arg1`, `arg2`, … shall **not** be evaluated.
3. If `receiver` is non-`nil`, evaluate argument expressions in the same order as ordinary message sends, then perform the message send.

This differs intentionally from ordinary Objective‑C message sends, where arguments are evaluated even if the receiver is `nil`.


Optional message send is permitted only if the method return type is:
- `void`, or
- an object/block pointer type.

If the method returns scalar/struct, optional send is **ill-formed** in v1 (all conformance levels).

> Rationale: nil-messaging returning 0 for scalars is a major source of silent logic bugs; ObjC 3 strictness is allowed to reject it.

### 3.4.3 Ordinary sends on nullable receivers
In **permissive** mode:
- sending a message to a nullable receiver using ordinary syntax (`[x foo]` where `x` is nullable) is permitted but diagnosed.

In **strict** mode:
- ordinary sends require the receiver type be nonnull, *unless* the send is written as an optional send (`[x? foo]`).

This rule is the cornerstone of “nonnull actually means something” in ObjC 3.

### 3.4.4 Flow-sensitive receiver refinement
If control flow proves a receiver nonnull, ordinary sends are permitted within that scope:

```objc
if (x != nil) {
  [x foo]; // OK: x refined to nonnull
}
```

This refinement also applies to `if let` / `guard let`.

---

### 3.4.5 v1 restriction: no scalar/struct optional chaining (normative)
In Objective‑C 3.0 v1, optional chaining constructs in §3.4.1 and §3.4.2 are defined **only** for:

- members/methods that return an Objective‑C object pointer type;
- members/methods that return a block pointer type; and
- `void` return type for optional message send only.

If the selected property or method returns any scalar, vector, enum, or struct/union type, use of `?.` or `[receiver? ...]` is **ill‑formed**.

**Note:** Ordinary Objective‑C message sends to `nil` receivers that yield scalar zero values remain part of the baseline language behavior. However, in ObjC 3.0 **strict** mode, ordinary sends on nullable receivers are rejected (see §3.4.3), pushing scalar-return calls into explicitly nonnull contexts and eliminating silent fallbacks.



## 3.5 Pragmatic generics

### 3.5.1 Goals and constraints
Objective‑C 3.0 generics shall:
- improve compile-time checking,
- preserve ABI via type erasure unless explicitly reified,
- interact predictably with covariance/contravariance where declared.

### 3.5.2 Generic type declarations

#### 3.5.2.1 Grammar (provisional)
```text
generic-parameter-clause:
    '<' generic-parameter (',' generic-parameter)* '>'

generic-parameter:
    variance? identifier generic-constraints?

variance:
    '__covariant'
  | '__contravariant'

generic-constraints:
    ':' type-constraint (',' type-constraint)*
```

Example:
```objc
@interface Box<__covariant T> : NSObject
@end
```

#### 3.5.2.2 Constraints
A `type-constraint` may be:
- an Objective‑C protocol type: `id<NSCopying>`
- a class bound: `NSObject`
- a composition: `id<Proto1, Proto2>`

The constraint grammar is intentionally limited for implementability.

### 3.5.3 Generic method/function declarations (future extension)

Objective‑C 3.0 v1 defers generic methods/functions.
Toolchains may reserve the syntax, but in v1 it is ill‑formed.

Rationale: integrating generic method syntax with Objective‑C selector grammar and redeclaration rules introduces significant complexity for separate compilation and interface emission.

### 3.5.4 Type argument application
Type arguments may be applied to generic types using angle brackets:
```objc
Box<NSString*>* b;
```

### 3.5.5 Erasure and runtime behavior
Unless a declaration is explicitly marked `@reify_generics` (future extension), generic arguments are erased at runtime:
- they do not affect object layout,
- they do not affect message dispatch,
- they exist for type checking and tooling.

### 3.5.6 Variance
If a generic parameter is declared covariant:
- `Box<NSString*>*` is a subtype of `Box<NSObject*>*`.

If invariant (default):
- no such subtyping is allowed.

Variance is enforced purely statically.

### 3.5.7 Diagnostics
In strict mode, the compiler shall diagnose:
- passing a `Box<NSObject*>*` where `Box<NSString*>*` is required (covariance direction),
- using a generic parameter without satisfying constraints,
- downcasting across generic arguments without explicit unsafe spelling.

---

## 3.6 Typed key paths

### 3.6.1 Key path literal

#### 3.6.1.1 Syntax
Objective‑C 3.0 introduces a key path literal:

```objc
@keypath(RootType, member.chain)
```

Examples:
```objc
auto kp1 = @keypath(Person, name);
auto kp2 = @keypath(Person, address.street);
```

Within instance methods, `self` may be used as root:
```objc
auto kp = @keypath(self, title);
```

#### 3.6.1.2 Grammar
```text
keypath-literal:
    '@keypath' '(' keypath-root ',' keypath-components ')'

keypath-root:
    type-name
  | 'self'

keypath-components:
    identifier ('.' identifier)*
```

### 3.6.2 KeyPath types
The standard library shall define:

- `KeyPath<Root, Value>`
- `WritableKeyPath<Root, Value>` (if writable)

A key path literal has type `KeyPath<Root, Value>` where:
- `Root` is the specified root type,
- `Value` is the type of the final member.

If any component is writable (settable) and the entire chain is writable, the literal may be typed as `WritableKeyPath`.

### 3.6.3 Static checking
The compiler shall validate at compile time:
- each member exists,
- accessibility rules allow access,
- each step’s type matches the next component’s root.

If validation fails, the program is ill-formed.

### 3.6.4 Application and bridging
The standard library shall provide:
- `Value KeyPathGet(KeyPath<Root, Value> kp, Root root);`
- `void KeyPathSet(WritableKeyPath<Root, Value> kp, Root root, Value v);`

The spec permits (but does not require) syntactic sugar such as:
- `root[kp]` for get and set.

Key paths may be explicitly converted to string keys for KVC where safe:
- `NSString* KeyPathToKVCString(KeyPath kp);`

Such conversions shall be explicit and diagnosed in strict mode if they may lose safety.

### 3.6.5 KVO/Observation integration
Where an observation API accepts key paths, it shall prefer typed key paths over strings.
(Actual observation APIs are library-defined; this spec defines the language value representation and checking.)

---

## 3.7 Required diagnostics and fix-its

### 3.7.1 Nullability
Required diagnostics (at least warnings; errors in strict):
- assigning nullable/unspecified to nonnull without check,
- returning nullable from a function declared nonnull,
- exporting unspecified nullability in strict mode.

### 3.7.2 Optional misuse
Required diagnostics:
- using a `T?` value where `T` is required without binding/unwrapping,
- optional member access used on scalar members,
- ordinary message sends on nullable receivers in strict mode (suggest optional send or binding).

### 3.7.3 Generics
Required diagnostics:
- constraint violations,
- unsafe generic downcasts requiring explicit spelling.

### 3.7.4 Key paths
Required diagnostics:
- invalid key path components,
- converting key paths to strings without explicit conversion.

---

## 3.8 Examples

### 3.8.1 Optional binding and optional send
```objc
NSString*? maybeName = [user? name];   // optional send returns nullable
NSString* name = maybeName ?? @"(none)";

guard let u = user else { return; }
NSString* n = [u name];               // safe: u is nonnull
```

### 3.8.2 Key paths
```objc
auto kp = @keypath(Person, address.street);
NSString* s = KeyPathGet(kp, person);
```

---

## 3.9 Open issues
1. Whether to standardize `id`/`Class` optional sugar and how it maps to nullability qualifiers.
2. Whether to introduce a first-class value optional ABI (`Optional<T>`) in a future revision.
3. Whether and how to add generic methods/functions in a future revision without breaking selector grammar.

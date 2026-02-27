# Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths {#part-3}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 3.0 Overview {#part-3-0}

### v0.8 resolved decisions {#part-3-v0-8-resolved-decisions}

- Optional chaining and optional message sends are **reference-only** in v1 (object/block/void only). Scalar/struct optional chaining is ill-formed.
- Optional message sends are **conditional calls**: argument expressions are evaluated only if the receiver is non-`nil`.
- Generic methods/functions are **deferred** in v1; generic _types_ remain supported.
- Canonical nullability default regions use `#pragma objc assume_nonnull begin/end` ([B](#b)).

Objective‑C 3.0 improves type safety without abandoning Objective‑C’s model:

- **Nullability** becomes a core, enforceable type property, with module/region defaults.
- **Optionals** provide explicit “maybe” semantics with ergonomic binding (`if let`, `guard let`) and explicit optional access.
- **Pragmatic generics** extend today’s lightweight generics into a more uniform system that can express generic APIs and constraints while remaining ABI-compatible.
- **Typed key paths** provide compiler-checked alternatives to stringly typed KVC/KVO-style access.

This part defines **syntax**, **static semantics**, **dynamic semantics**, and **required diagnostics** for these features.

---

## 3.1 Lexical and grammar additions {#part-3-1}

### 3.1.1 New keywords {#part-3-1-1}

In Objective‑C 3.0 mode, the following keywords are reserved by this part:

- `guard`, `let`, `var`
- `optional`, `some`, `none` (reserved for future value-optional syntax)

In ObjC 3.0 mode, the type-constructor spelling `Optional<...>` is reserved for a future value-optional ABI.
User declarations shall not occupy unescaped `Optional`/`optional`/`some`/`none` spellings; use the raw-identifier escape in [§1.3.3](#part-1-3-2) when source compatibility requires these names.

(Additional reserved keywords are specified in [Part 1](#part-1).)

### 3.1.2 New punctuators {#part-3-1-2}

This part introduces new punctuators in specific grammatical contexts:

- `?` and `!` as **type suffixes** in declarations (optional sugar)
- `?.` as an **optional member access** token in expressions
- `??` as a **nil-coalescing operator** in expressions

The tokens shall be recognized only where specified by the grammar; in all other contexts, existing C/Objective‑C meaning applies.

---

## 3.2 Nullability {#part-3-2}

### 3.2.1 Nullability kinds {#part-3-2-1}

Objective‑C 3.0 recognizes the following nullability kinds for Objective‑C object pointer types and block pointer types:

- **Nonnull**: value is not `nil`.
- **Nullable**: value may be `nil`.
- **Unspecified**: nullability is unknown (legacy).

The **unspecified** kind exists only for compatibility and shall be treated as “potentially nil” for safety diagnostics.

### 3.2.2 Where nullability applies {#part-3-2-2}

Nullability applies to:

- Objective‑C object pointer types (`id`, `Class`, `NSObject *`, `NSString *`, `id<Proto>`, etc.)
- block pointer types (e.g., `void (^)(void)`)

Nullability does **not** apply directly to:

- scalar types (`int`, `BOOL`, `NSInteger`, etc.)
- pointers that are not object/block pointers (`int *`, `void *`, etc.)

(Those may be covered by separate annotations; out of scope for this part.)

### 3.2.3 Nullability spelling forms {#part-3-2-3}

Objective‑C 3.0 supports three equivalent spelling forms for object/block pointer nullability:

1. **Qualifier form** (baseline spelling):
   - `_Nonnull`, `_Nullable`, `_Null_unspecified` (or implementation-provided equivalents)
2. **Objective‑C contextual keywords** (when supported by implementation):
   - `nonnull`, `nullable`
3. **Optional sugar** (ObjC 3.0):
   - `T?` (nullable; includes `id?`, `id<...>?`, and `Class?`)
   - `T!` (implicitly unwrapped optional; includes `id!`, `id<...>!`, and `Class!`; see [§3.3.3](#part-3-3-3))

#### 3.2.3.1 Canonicalization rule {#part-3-2-3-1}

For type-checking purposes, implementations shall canonicalize:

- `T?` → `T _Nullable`
- `T!` → `T _Nullable` **plus** “implicitly-unwrapped” marker
- `id?` → `id _Nullable`
- `id!` → `id _Nullable` **plus** “implicitly-unwrapped” marker
- `Class?` → `Class _Nullable`
- `Class!` → `Class _Nullable` **plus** “implicitly-unwrapped” marker

If a type already includes an explicit nullability qualifier, the sugar form shall be rejected as redundant.

#### 3.2.3.2 `id` / `Class` sugar scope {#part-3-2-3-2}

`id` and `Class` are in-scope for optional sugar in v1.

- `id?`/`id!` and `Class?`/`Class!` are well-formed and follow [§3.2.3.1](#part-3-2-3-1) canonicalization.
- `id<Proto>?`/`id<Proto>!` are also well-formed.
- Header import/export and module metadata shall preserve the same effective nullability for these spellings as for equivalent qualifier forms.

### 3.2.4 Nonnull-by-default regions {#part-3-2-4}

#### 3.2.4.1 Region introduction {#part-3-2-4-1}

A translation unit in ObjC 3.0 mode may establish a nonnull-by-default region using one of:

- a module-level default (recommended for frameworks),
- a pragma region, or
- an explicit language directive.

The canonical spelling is the pragma form ([B](#b)):

```c
#pragma objc assume_nonnull begin
// declarations
#pragma objc assume_nonnull end
```

Implementations may additionally support aliases such as `#pragma clang assume_nonnull begin/end` or `NS_ASSUME_NONNULL_BEGIN/END`, but emitted interfaces shall use the canonical spelling.

#### 3.2.4.2 Region semantics {#part-3-2-4-2}

Inside a nonnull-by-default region:

- Any unannotated object/block pointer type in a **declaration** is treated as **nonnull**.
- `nullable` must be explicitly spelled.

Outside such a region:

- unannotated object/block pointer types are treated as **unspecified**.

> Rationale: this allows headers to become “safe by default” while preserving compatibility with legacy code.

### 3.2.5 Nullability completeness {#part-3-2-5}

#### 3.2.5.1 Completeness scope and effective nullability {#part-3-2-5-1}

For completeness checking, implementations shall inspect each exported type position where nullability applies ([§3.2.2](#part-3-2-2)), including:

- function/method/block parameters and returns,
- exported property/ivar types,
- typedef/typealias expansions referenced by exported declarations,
- nested object/block pointer positions inside block signatures and generic arguments.

Each inspected position has an **effective nullability** determined in this order:

1. explicit qualifier spelling (`_Nonnull`, `_Nullable`, `_Null_unspecified`, equivalent contextual keywords, or `T?`/`T!`);
2. enclosing nonnull-by-default region ([§3.2.4](#part-3-2-4));
3. otherwise **unspecified**.

A position whose effective nullability is unspecified is **incomplete**.
An exported declaration with one or more incomplete positions is **nullability-incomplete**.
Declarations with no inspected positions are **not applicable** for completeness scoring.

#### 3.2.5.2 Public interface/module completeness {#part-3-2-5-2}

A module interface is **nullability-complete** for an import view if all exported, applicable declarations in that view are complete.
A module interface is **nullability-incomplete** if any exported, applicable declaration in that view is nullability-incomplete.

Intentional use of `_Null_unspecified` for compatibility is permitted, but still counts as incompleteness for this classification.

#### 3.2.5.3 Mixed C/ObjC/ObjC++ import views {#part-3-2-5-3}

In mixed-language modules, completeness shall be evaluated against the declarations visible in the importer's active language view:

- **C view**: this part's completeness rules are not applicable to declarations that do not use Objective-C object/block pointer types.
- **ObjC view**: includes Objective-C declarations and any C declarations visible to ObjC that use Objective-C object/block pointer types.
- **ObjC++ view**: includes the ObjC view plus ObjC++-visible declarations (including C++ entities) that use Objective-C object/block pointer types.

A mixed-language module may be nullability-complete in one view and nullability-incomplete in another; diagnostics shall be based on the active importer view.

#### 3.2.5.4 Profile-specific diagnostic severity {#part-3-2-5-4}

For conformance-profile behavior ([Part 1](#part-1), [E](#e)):

- Exported declaration is nullability-incomplete in the active import view:
  - Core: recoverable diagnostic; build/import may continue with unspecified assumptions.
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.
- Imported module is nullability-incomplete in the active import view:
  - Core: recoverable diagnostic; import may continue with unspecified assumptions.
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.
- Missing/mismatched nullability/default metadata, or interface/metadata mismatch for nullability [D Table A](#d-3-1) items:
  - Core: follow [D.3.5](#d-3-5) (recoverable unless ABI-significant).
  - Strict: hard error.
  - Strict Concurrency: hard error.
  - Strict System: hard error.

#### 3.2.5.5 Local completeness {#part-3-2-5-5}

Within a translation unit, incomplete nullability is permitted but should be diagnosed when it impacts type safety (e.g., assigning “unspecified” to “nonnull”).
When such incompleteness crosses a module/public boundary, diagnostics shall follow [§3.2.5.4](#part-3-2-5-4).

### 3.2.6 Completeness representation in metadata and emitted interfaces {#part-3-2-6}

Nullability completeness across separate compilation shall be represented and reconstructed as follows:

- The semantic source of truth is the nullability-related [D Table A](#d-3-1) entries (qualifiers, nonnull-by-default regions, optional spellings).
- Module metadata and textual interfaces shall preserve those entries so importers can recompute effective nullability and completeness deterministically.
- Interface emission shall preserve canonical nonnull-region spellings ([B.2](#b-2), [B.7](#b-7)) and shall not rewrite nullability-incomplete declarations into complete ones.
- If source uses `id?`/`Class?` (or `!` variants), emitted interfaces may canonicalize to qualifier spellings, but importers shall observe equivalent effective nullability.
- Implementations may additionally encode derived completeness summaries (for example, per-declaration or per-view completeness flags) as ignorable extension metadata under [D.2](#d-2)/[D.3.4](#d-3-4); if such summaries are absent, importers shall recompute from preserved nullability metadata.

---

## 3.3 Optionals {#part-3-3}

### 3.3.1 Optional type sugar (`T?`) {#part-3-3-1}

A type written with the suffix `?` denotes a nullable object/block pointer type.

Optional sugar (`?` and `!`) is permitted only on types where nullability applies ([§3.2.2]), including `id`, `id<...>`, `Class`, Objective‑C object pointers, and block pointers.
Applying optional sugar to any other type is ill-formed.

Examples:

```objc
NSString*? maybeName;
id<NSCopying>? maybeKey;
id? anyObj;
Class? maybeMetaClass;
void (^? callback)(int);
```

### 3.3.2 Optional binding: `if let` and `guard let` {#part-3-3-2}

#### 3.3.2.1 Grammar {#part-3-3-2-1}

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

#### 3.3.2.2 Semantics {#part-3-3-2-2}

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

#### 3.3.2.3 `let` vs `var` {#part-3-3-2-3}

- `let` bindings are immutable.
- `var` bindings are mutable.

Implementations shall diagnose mutation of `let` bindings as an error.

### 3.3.3 Implicitly unwrapped optionals (`T!`) {#part-3-3-3}

`T!` denotes a nullable value that is **implicitly unwrapped** at use sites in certain contexts.

#### 3.3.3.1 Intended use {#part-3-3-3-1}

`T!` exists primarily for interop boundaries where the API is “logically nonnull” but not yet fully annotated, or where legacy behavior is relied upon.

#### 3.3.3.2 Static semantics {#part-3-3-3-2}

In ObjC 3.0 strict mode:

- implicit unwrapping of `T!` without an explicit proof is diagnosed.
- implementations should offer fix-its to convert to `T?` + binding or add nullability to the API.

#### 3.3.3.3 Dynamic behavior (optional) {#part-3-3-3-3}

An implementation may provide a runtime trap when an implicitly unwrapped optional is observed to be `nil` at a use site, but this is not required by this draft.

> Recommendation: treat IUO primarily as a static-typing aid and migration tool, not as a runtime “bomb”.

### 3.3.4 Nil-coalescing operator (`??`) {#part-3-3-4}

#### 3.3.4.1 Grammar {#part-3-3-4-1}

```text
nil-coalescing-expression:
    logical-or-expression
    nil-coalescing-expression '??' nil-coalescing-expression
```

`??` is right-associative.

#### 3.3.4.2 Semantics {#part-3-3-4-2}

`a ?? b` evaluates `a`:

- if `a` is nonnull, yields `a` and does not evaluate `b`;
- otherwise evaluates and yields `b`.

#### 3.3.4.3 Typing {#part-3-3-4-3}

If `a` has type `T?` and `b` has type `T` (nonnull), then `a ?? b` has type `T` (nonnull).

More generally:

- The result type is the least upper bound of the operand types, with nullability resolved to nonnull if the right operand is nonnull and `a` is tested.

In strict mode, `b` must be type-compatible with the unwrapped type of `a`; otherwise error.

### 3.3.5 Future value-optional ABI reservation (v1 guardrails) {#part-3-3-5}

Objective‑C 3.0 v1 does not define a first-class value optional ABI (`Optional<T>` layout/tagged payload semantics).

The following spellings are reserved for a future revision and are ill-formed in v1 user code unless escaped per [§1.3.3](#part-1-3-2):

- `optional<...>` in type positions.
- `Optional<...>` in type positions.
- `.some(...)` and `.none` in optional-constructor/pattern positions.

#### 3.3.5.1 Future-compat constraints (v1) {#part-3-3-5-1}

To avoid blocking a future value optional design:

- `T?`/`T!` in v1 remain reference-nullability sugar only and shall not imply a value layout contract.
- v1 parser and interface emitters shall keep the reserved spellings above unavailable for unrelated language/library features.
- Module metadata and textual interfaces shall preserve optional/nullability semantics via extensible encoding so a future value-optional kind can be added without redefining existing v1 fields.
- Diagnostics for non-reference optional operations should be worded as “not supported in v1” rather than “never supported,” preserving forward migration paths.

#### 3.3.5.2 Canonical future spelling policy (v0.11 decision) {#part-3-3-5-2}

Per [D-013](DECISIONS_LOG.md#decisions-d-013), any future value-optional feature shall use
`Optional<T>` as the canonical source spelling.

- `optional<T>` remains reserved in v1 and shall not be treated as canonical.
- If a future implementation mode offers `optional<T>` as a compatibility alias, conforming modes shall still diagnose it and provide a fix-it to `Optional<T>`.
- Canonical textual interface emission for future value-optionals shall use `Optional<T>`.

---

## 3.4 Nullable receivers and optional access {#part-3-4}

Objective‑C historically allows messaging `nil` and returns zero/`nil`. ObjC 3.0 preserves this behavior for compatibility but provides **explicit, typed optional access** and enables strict diagnostics.

### 3.4.1 Optional member access: `?.` {#part-3-4-1}

#### 3.4.1.1 Grammar {#part-3-4-1-1}

```text
postfix-expression:
    ...
    postfix-expression '?.' identifier
```

#### 3.4.1.2 Semantics {#part-3-4-1-2}

For `x?.p` where `p` resolves to a property access:

- Evaluate `x` once.
- If `x` is `nil`, yield `nil`.
- Otherwise perform the normal property access and yield the value.

#### 3.4.1.3 Typing restrictions {#part-3-4-1-3}

`?.` is permitted only when the property type is an **object/block pointer type**.

- The result type is the property type made nullable (i.e., `T?`).

If the property type is scalar/struct:

- `?.` is ill-formed in strict mode and diagnosed in permissive mode (because there is no scalar optional type in this draft).

### 3.4.2 Optional message send: `[receiver? selector]` {#part-3-4-2}

#### 3.4.2.1 Grammar {#part-3-4-2-1}

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

#### 3.4.2.2 Semantics {#part-3-4-2-2}

- Evaluate the receiver expression once.
- If receiver is `nil`, the optional send yields:
  - `nil` for object/block pointer returns,
  - no effect for `void` returns.

#### 3.4.2.3 Typing restrictions {#part-3-4-2-3}

#### 3.4.2.4 Argument evaluation (normative) {#part-3-4-2-4}

For an optional message send of the form:

```objc
[receiver? method:arg1 other:arg2]
```

the implementation shall:

1. Evaluate `receiver` exactly once.
2. If `receiver` is `nil`, the expression yields the “nil case” result ([§3.4.2.2](#part-3-4-2-2)) and the argument expressions `arg1`, `arg2`, … shall **not** be evaluated.
3. If `receiver` is non-`nil`, evaluate argument expressions in the same order as ordinary message sends, then perform the message send.

This differs intentionally from ordinary Objective‑C message sends, where arguments are evaluated even if the receiver is `nil`.

Optional message send is permitted only if the method return type is:

- `void`, or
- an object/block pointer type.

If the method returns scalar/struct, optional send is **ill-formed** in v1 (all conformance levels).

> Rationale: nil-messaging returning 0 for scalars is a major source of silent logic bugs; ObjC 3 strictness is allowed to reject it.

### 3.4.3 Ordinary sends on nullable receivers {#part-3-4-3}

In **permissive** mode:

- sending a message to a nullable receiver using ordinary syntax (`[x foo]` where `x` is nullable) is permitted but diagnosed.

In **strict** mode:

- ordinary sends require the receiver type be nonnull, _unless_ the send is written as an optional send (`[x? foo]`).

This rule is the cornerstone of “nonnull actually means something” in ObjC 3.

### 3.4.4 Flow-sensitive receiver refinement {#part-3-4-4}

If control flow proves a receiver nonnull, ordinary sends are permitted within that scope:

```objc
if (x != nil) {
  [x foo]; // OK: x refined to nonnull
}
```

This refinement also applies to `if let` / `guard let`.

---

### 3.4.5 v1 restriction: no scalar/struct optional chaining (normative) {#part-3-4-5}

In Objective‑C 3.0 v1, optional chaining constructs in [§3.4.1](#part-3-4-1) and [§3.4.2](#part-3-4-2) are defined **only** for:

- members/methods that return an Objective‑C object pointer type;
- members/methods that return a block pointer type; and
- `void` return type for optional message send only.

If the selected property or method returns any scalar, vector, enum, or struct/union type, use of `?.` or `[receiver? ...]` is **ill‑formed**.

**Note:** Ordinary Objective‑C message sends to `nil` receivers that yield scalar zero values remain part of the baseline language behavior. However, in ObjC 3.0 **strict** mode, ordinary sends on nullable receivers are rejected (see [§3.4.3](#part-3-4-3)), pushing scalar-return calls into explicitly nonnull contexts and eliminating silent fallbacks.

## 3.5 Pragmatic generics {#part-3-5}

### 3.5.1 Goals and constraints {#part-3-5-1}

Objective‑C 3.0 generics shall:

- improve compile-time checking,
- preserve ABI via type erasure unless explicitly reified,
- interact predictably with covariance/contravariance where declared.

### 3.5.2 Generic type declarations {#part-3-5-2}

#### 3.5.2.1 Grammar (provisional) {#part-3-5-2-1}

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

#### 3.5.2.2 Constraints {#part-3-5-2-2}

A `type-constraint` may be:

- an Objective‑C protocol type: `id<NSCopying>`
- a class bound: `NSObject`
- a composition: `id<Proto1, Proto2>`

The constraint grammar is intentionally limited for implementability.

### 3.5.3 Generic method/function declarations (v2 design path; reserved in v1) {#part-3-5-3}

#### 3.5.3.1 v1 status {#part-3-5-3-1}

Objective‑C 3.0 v1 defers generic methods/functions.
Toolchains shall reserve the syntax in [§3.5.3.2](#part-3-5-3-2), but in v1 source that uses it is ill‑formed.

#### 3.5.3.2 Candidate syntax (future revision) {#part-3-5-3-2}

A generic parameter clause appears before the return type for both Objective‑C methods and C/ObjC functions.

```text
generic-method-declaration:
    ('-' | '+') generic-parameter-clause method-type-and-selector

generic-function-declaration:
    generic-parameter-clause declaration-specifiers declarator
```

Examples:

```objc
- <T: id<NSCopying>> (T)coerce:(id)value;
+ <T> (NSArray<T>*)singleton:(T)value;

<T> T OCIdentity(T value);
<T: NSObject> NSArray<T>* OCCollect(T first, ...);
```

#### 3.5.3.3 Lowering and mangling path (future revision) {#part-3-5-3-3}

The viable path is **erased execution with preserved generic signatures**:

- Type checking uses declared generic parameters/constraints.
- ABI lowering erases generic parameters to their bounds (or `id` when unconstrained), unless a future explicit reification mode is enabled.
- Objective‑C method dispatch remains selector-based with one runtime IMP per declaration; type arguments do not create selector variants.
- Generic free functions use one emitted body per declaration and shall satisfy stable mangling invariants that include:
  - base function name,
  - generic arity,
  - normalized constraint signature (or digest),
  - and deterministic reproduction for identical declarations under one toolchain/policy.

Per [D-014](DECISIONS_LOG.md#decisions-d-014), v0.11 standardizes semantic invariants and policy
stability, not one byte-for-byte mangling string across all toolchains:

- Conforming toolchains shall publish a stable mangling policy identifier for enabled generic free-function support.
- Module metadata and textual interfaces shall preserve the semantic generic signature required for cross-tool conformance assertions.
- Conformance assertions shall validate semantic equivalence and policy-id stability; literal symbol-byte equality is only required within a single declared policy.

#### 3.5.3.4 Selector and metadata interaction (future revision) {#part-3-5-3-4}

- Selector identity excludes the generic parameter clause.
- Two methods in the same class/protocol hierarchy shall not differ only by generic parameter names or constraints if selector pieces are identical.
- Overrides/redeclarations shall match selector, generic arity, and normalized constraints.
- Module metadata and textual interfaces shall preserve the generic signature for each generic method/function (parameter list plus constraints) so importers can type-check and validate redeclarations consistently.
- Import/redeclaration mismatches in preserved generic signatures are diagnosed per [§3.7.3](#part-3-7-3).

### 3.5.4 Type argument application {#part-3-5-4}

Type arguments may be applied to generic types using angle brackets:

```objc
Box<NSString*>* b;
```

### 3.5.5 Erasure and runtime behavior {#part-3-5-5}

Per [D-015](DECISIONS_LOG.md#decisions-d-015), future explicit reification mode is declaration-scoped.
Unless a declaration is explicitly marked `@reify_generics` (future extension), generic arguments are erased at runtime:

- they do not affect object layout,
- they do not affect message dispatch,
- they exist for type checking and tooling.

Additional constraints for any future reification-capable mode:

- Reification controls shall apply per declaration; enabling a module/profile mode alone shall not implicitly reify unrelated declarations.
- Module/profile controls may gate whether declaration-scoped reification syntax is accepted, but shall not change meaning of declarations that omit reification markers.
- Mixed modules that contain both erased and reified declarations shall preserve this distinction in metadata and textual interfaces.

### 3.5.6 Variance {#part-3-5-6}

If a generic parameter is declared covariant:

- `Box<NSString*>*` is a subtype of `Box<NSObject*>*`.

If invariant (default):

- no such subtyping is allowed.

Variance is enforced purely statically.

### 3.5.7 Diagnostics {#part-3-5-7}

In strict mode, the compiler shall diagnose:

- passing a `Box<NSObject*>*` where `Box<NSString*>*` is required (covariance direction),
- using a generic parameter without satisfying constraints,
- downcasting across generic arguments without explicit unsafe spelling.

---

## 3.6 Typed key paths {#part-3-6}

### 3.6.1 Key path literal {#part-3-6-1}

#### 3.6.1.1 Syntax {#part-3-6-1-1}

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

#### 3.6.1.2 Grammar {#part-3-6-1-2}

```text
keypath-literal:
    '@keypath' '(' keypath-root ',' keypath-components ')'

keypath-root:
    type-name
  | 'self'

keypath-components:
    identifier ('.' identifier)*
```

### 3.6.2 KeyPath types {#part-3-6-2}

The standard library shall define:

- `KeyPath<Root, Value>`
- `WritableKeyPath<Root, Value>` (if writable)

A key path literal has type `KeyPath<Root, Value>` where:

- `Root` is the specified root type,
- `Value` is the type of the final member.

If any component is writable (settable) and the entire chain is writable, the literal may be typed as `WritableKeyPath`.

### 3.6.3 Static checking {#part-3-6-3}

The compiler shall validate at compile time:

- each member exists,
- accessibility rules allow access,
- each step’s type matches the next component’s root.

If validation fails, the program is ill-formed.

### 3.6.4 Application and bridging {#part-3-6-4}

The standard library shall provide:

- `Value KeyPathGet(KeyPath<Root, Value> kp, Root root);`
- `void KeyPathSet(WritableKeyPath<Root, Value> kp, Root root, Value v);`

The spec permits (but does not require) syntactic sugar such as:

- `root[kp]` for get and set.

Key paths may be explicitly converted to string keys for KVC where safe:

- `NSString* KeyPathToKVCString(KeyPath kp);`

Such conversions shall be explicit and diagnosed in strict mode if they may lose safety.

### 3.6.5 KVO/Observation integration {#part-3-6-5}

Where an observation API accepts key paths, it shall prefer typed key paths over strings.
(Actual observation APIs are library-defined; this spec defines the language value representation and checking.)

---

## 3.7 Required diagnostics and fix-its {#part-3-7}

### 3.7.1 Nullability {#part-3-7-1}

Required diagnostics:

- assigning nullable/unspecified to nonnull without check,
- returning nullable from a function declared nonnull,
- producing or importing a nullability-incomplete public interface in the active language view (severity per [§3.2.5.4](#part-3-2-5-4)),
- missing/mismatched nullability-related module/interface metadata as validated by [D.3.5](#d-3-5).

### 3.7.2 Optional misuse {#part-3-7-2}

Required diagnostics:

- using a `T?` value where `T` is required without binding/unwrapping,
- applying optional type sugar (`?`/`!`) to a type outside [§3.2.2](#part-3-2-2),
- applying optional type sugar (`?`/`!`) when nullability is already explicitly spelled on the same type position,
- in v1 mode, parsing value-optional constructor spellings (`optional<...>`, `Optional<...>`) in type positions emits `OPT-SPELL-RESERVED-V1`,
- in future value-optional compatibility mode, parsing lowercase `optional<...>` emits `OPT-SPELL-NONCANON`,
- in future value-optional canonical-only mode, parsing lowercase `optional<...>` emits `OPT-SPELL-NONCANON-UNSUPPORTED`,
- when a required optional-spelling fix-it cannot be applied because the token originates in non-rewritable macro expansion text, emit companion note `OPT-SPELL-NOFIX-MACRO`,
- declaring unescaped identifiers that occupy reserved spellings `Optional`, `optional`, `some`, or `none` in ObjC 3.0 mode,
- optional member access used on scalar members,
- ordinary message sends on nullable receivers in strict mode (suggest optional send or binding).

Required optional-spelling fix-it behavior (`optional<T>` -> `Optional<T>`):

1. For `OPT-SPELL-NONCANON` and `OPT-SPELL-NONCANON-UNSUPPORTED`, rewrite only the identifier token `optional` to `Optional`.
2. Preserve generic argument text and punctuation exactly (`<...>` unchanged).
3. Preserve leading/trailing trivia around the rewritten identifier token.
4. Emit one fix-it per noncanonical occurrence, including nested occurrences.
5. If the source range is not rewritable (for example, macro expansion text), suppress the edit and emit `OPT-SPELL-NOFIX-MACRO`.

Profile severity behavior for optional-spelling diagnostics:

| Validation condition | Core | Strict | Strict Concurrency | Strict System |
| --- | --- | --- | --- | --- |
| `OPT-SPELL-NONCANON` (future mode, compatibility alias accepted) | Warning with required fix-it in migration mode; error with required fix-it otherwise | Error with required fix-it | Error with required fix-it | Error with required fix-it |
| `OPT-SPELL-NONCANON-UNSUPPORTED` (future mode, canonical-only acceptance) | Error with required fix-it | Error with required fix-it | Error with required fix-it | Error with required fix-it |
| `OPT-SPELL-RESERVED-V1` (v1 reservation guardrail) | Error | Error | Error | Error |
| `OPT-SPELL-NOFIX-MACRO` companion note when rewrite unavailable | Note (paired with owning warning/error) | Note (paired with owning error) | Note (paired with owning error) | Note (paired with owning error) |

Conforming interface emission for future value-optionals shall print canonical `Optional<...>` spellings only.

### 3.7.3 Generics {#part-3-7-3}

Required diagnostics:

- constraint violations,
- unsafe generic downcasts requiring explicit spelling,
- use of reserved generic method/function declaration syntax in v1,
- selector collisions where declarations differ only by generic signature,
- redeclaration/import mismatch in generic arity or constraints.

### 3.7.4 Key paths {#part-3-7-4}

Required diagnostics:

- invalid key path components,
- converting key paths to strings without explicit conversion.

### 3.7.5 Nullability completeness conformance matrix ideas {#part-3-7-5}

Conforming suites should include profile- and language-view matrix tests such as:

- `NC-01`: Public ObjC API fully annotated via `assume_nonnull` plus explicit `nullable` overrides.
  - Expected completeness status: complete (ObjC and ObjC++).
  - Core expectation: no completeness diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic.
- `NC-02`: Intentionally incomplete public ObjC method with unannotated object pointer outside any nonnull-default region.
  - Expected completeness status: incomplete.
  - Core expectation: recoverable diagnostic; import may continue with unspecified assumptions.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-03`: Public API explicitly uses `_Null_unspecified` on an object/block pointer.
  - Expected completeness status: incomplete (intentional).
  - Core expectation: recoverable diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-04`: Incomplete nested position (for example, object pointer inside exported block typedef signature).
  - Expected completeness status: incomplete.
  - Core expectation: recoverable diagnostic.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-05`: Mixed C+ObjC module where C partition has only non-ObjC pointers/scalars and ObjC partition is complete.
  - Expected completeness status: complete in ObjC/ObjC++; not applicable in C view.
  - Core expectation: no completeness diagnostic in ObjC/ObjC++; none for C-view non-applicable declarations.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic in ObjC/ObjC++.
- `NC-06`: Mixed module with ObjC view complete, but an ObjC++-only declaration uses an unannotated ObjC object pointer.
  - Expected completeness status: ObjC view complete; ObjC++ view incomplete.
  - Core expectation: recoverable diagnostic only for ObjC++ imports.
  - Strict / Strict Concurrency / Strict System expectation: hard error for ObjC++ imports.
- `NC-07`: Metadata/interface mismatch for nullability items in [D Table A](#d-3-1) (for example, dropped nonnull-default region tag).
  - Expected completeness status: validation failure.
  - Core expectation: recoverable diagnostic unless classified ABI-significant by [D.3.5](#d-3-5).
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-08`: Metadata payload drops required nullability qualifiers/defaults so importer cannot reconstruct effective nullability.
  - Expected completeness status: treated as incomplete/invalid semantic metadata.
  - Core expectation: recoverable diagnostic with conservative unspecified assumptions.
  - Strict / Strict Concurrency / Strict System expectation: hard error.
- `NC-09`: Public header exports `id?`/`Class?` spellings and is imported via textual interface in a clean build.
  - Expected completeness status: complete when effective nullability matches qualifier form.
  - Core expectation: no completeness diagnostic; importer behavior matches equivalent `id _Nullable`/`Class _Nullable` declarations.
  - Strict / Strict Concurrency / Strict System expectation: no completeness diagnostic.
- `NC-10`: Module metadata/interface round-trip for declarations that used `id?`/`Class?` in source.
  - Expected completeness status: complete when metadata preserves equivalent effective nullability.
  - Core expectation: mismatch or dropped optional/nullability metadata is diagnosed per `NC-07`/`NC-08`.
  - Strict / Strict Concurrency / Strict System expectation: hard error on required-metadata mismatch.

These tests should be run at least across ObjC and ObjC++ import modes; C-mode imports should assert non-applicability for declarations outside [§3.2.2](#part-3-2-2).

### 3.7.6 Future value-optional spelling conformance ideas {#part-3-7-6}

Conforming suites should include reserved-spelling occupancy and migration-path tests such as:

- `VO-01`: Declaring `typedef int Optional;` in ObjC 3.0 mode is rejected; an escaped raw identifier form is accepted.
- `VO-02`: Declaring unescaped `some`/`none` identifiers in ObjC 3.0 mode is rejected; escaped raw identifier forms are accepted.
- `VO-03`: Parsing `Optional<int>` or `optional<int>` in a type position produces a reserved-for-future-extension diagnostic in v1.
- `VO-04`: Module import of APIs that used escaped raw identifiers for reserved spellings preserves identity without enabling unescaped spellings.
- `VO-05`: Future value-optional-enabled mode parses canonical `Optional<int>` and accepts with no optional-spelling diagnostic.
- `VO-06`: Future compatibility mode accepts `optional<int>` and emits `OPT-SPELL-NONCANON` with severity per [§3.7.2](#part-3-7-2), plus one-step fix-it to `Optional<int>`.
- `VO-07`: Future canonical-only mode parsing `optional<int>` emits `OPT-SPELL-NONCANON-UNSUPPORTED` as a hard error with one-step fix-it to `Optional<int>`.
- `VO-08`: v1 mode parsing `Optional<int>` or `optional<int>` emits `OPT-SPELL-RESERVED-V1` with reserved-for-future wording (severity per [§3.7.2](#part-3-7-2)).
- `VO-09`: Interface/module emission after parsing noncanonical source in compatibility mode serializes canonical `Optional<...>` only.
- `VO-10`: Noncanonical spelling originating in non-rewritable macro expansion emits the owning spelling diagnostic plus `OPT-SPELL-NOFIX-MACRO`, and no invalid edit.
- `VO-11`: Batch migrator over source files containing `optional<...>` rewrites occurrences to `Optional<...>` with no semantic delta and no unrelated token changes.

### 3.7.7 Generic method/function conformance ideas (future-feature gate) {#part-3-7-7}

Conforming suites should include generic-method path tests such as:

- `GM-01`: In v1 mode, parsing the [§3.5.3.2](#part-3-5-3-2) generic method/function syntax produces a reserved-for-future-extension diagnostic.
- `GM-02`: In an implementation mode that enables generic methods/functions, same-selector declarations that differ only by generic signature are rejected.
- `GM-03`: Module/interface round-trip preserves generic method/function signatures (arity and constraints), and mismatch is diagnosed on import.
- `GM-04`: Generic Objective‑C methods keep selector identity independent of type arguments (single selector string, no type-argument selector variants).
- `GM-05`: Generic free functions produce deterministic mangling for identical declarations under the same toolchain and mangling policy ID.
- `GM-06`: Different generic signatures for the same base name produce distinct mangling outcomes and distinct preserved semantic signature records.
- `GM-07`: Cross-tool conformance compares semantic signatures and declared mangling policy IDs; direct string equality of symbols is not required across different policy IDs.
- `GM-08`: Enabling a module/profile generic mode without declaration-scoped `@reify_generics` markers does not reify declarations by default.

---

## 3.8 Examples {#part-3-8}

### 3.8.1 Optional binding and optional send {#part-3-8-1}

```objc
NSString*? maybeName = [user? name];   // optional send returns nullable
NSString* name = maybeName ?? @"(none)";

guard let u = user else { return; }
NSString* n = [u name];               // safe: u is nonnull
```

### 3.8.2 Key paths {#part-3-8-2}

```objc
auto kp = @keypath(Person, address.street);
NSString* s = KeyPathGet(kp, person);
```

---

## 3.9 Open issues {#part-3-9}

No open issues are tracked in this part for v0.11.

Resolved by decisions [D-013](DECISIONS_LOG.md#decisions-d-013),
[D-014](DECISIONS_LOG.md#decisions-d-014), and
[D-015](DECISIONS_LOG.md#decisions-d-015).



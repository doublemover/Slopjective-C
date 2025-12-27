# Part 3 — Types: Nullability, Optionals, Pragmatic Generics, and Typed Key Paths

## 3.1 Purpose
Objective‑C 3.0 treats “type safety” as practical correctness:
- make nullability explicit and enforceable,
- add optional binding to remove boilerplate,
- improve generics without turning ObjC into a wholly different type system,
- make key paths typed and compiler-checked to reduce stringly-typed API hazards.

## 3.2 Nullability as a core type property

### 3.2.1 Nullability qualifiers
Objective‑C 3.0 recognizes nullability qualifiers on object pointer types:

- nonnull (not nil)
- nullable (may be nil)
- null-unspecified (legacy/unknown; treated as “may be nil” for safety diagnostics)

### 3.2.2 Nonnull-by-default regions
Objective‑C 3.0 introduces the concept of a **nonnull-by-default region**.

#### 3.2.2.1 Syntax
A nonnull-by-default region is introduced by one of:
- module-level default,
- pragma region markers,
- or attributes on interfaces.

Illustrative region markers:

```objc
@assume_nonnull_begin
// declarations
@assume_nonnull_end
```

> Open issue: the canonical spelling may reuse existing macro conventions, but ObjC 3.0 defines the semantics at the language level.

#### 3.2.2.2 Semantics
Inside a nonnull-by-default region:
- all unannotated object pointer types in declarations are treated as nonnull.
- nullable must be explicitly spelled.

Outside such a region:
- defaults are implementation-defined but should match today’s pragmatic behavior (often null-unspecified unless annotated).

### 3.2.3 Nullability completeness requirements
For a public API surface (module export):
- all exported object pointer types shall have explicit nullability by the time the module is distributed in strict mode.

In permissive mode:
- incomplete nullability is permitted but diagnosed.

### 3.2.4 Flow-sensitive nonnull refinement
A conforming compiler shall apply flow-sensitive refinement:
- `if (x != nil)` refines `x` to nonnull in the `true` branch.
- `if (x == nil)` refines `x` to null in the `true` branch.
- After `guard x != nil else { exit }`, `x` is refined to nonnull after the guard.

Refinement applies to:
- local variables and parameters,
- properties accessed through stable receivers (implementation may require `self` is not mutated concurrently unless proven).

### 3.2.5 Diagnosing nil misuse
In strict mode, the compiler shall reject:
- dereferencing a nullable pointer without an explicit check or unwrap,
- passing a nullable value to a parameter declared nonnull unless explicitly coerced via unsafe cast.

## 3.3 Optional type sugar and binding

### 3.3.1 Optional sugar types
In ObjC 3.0, object pointer types may be written with optional sugar:

- `T?` denotes “nullable T”
- `T!` denotes “implicitly unwrapped optional T” (IUO)

#### 3.3.1.1 Semantics
- `T?` is semantically equivalent to `T _Nullable`.
- `T!` behaves as:
  - nullable for storage and interop,
  - but implicitly unwrapped at use sites (like “assume nonnull, trap if nil” or “diagnose if nil flows are detected”).

> Open issue: decide whether IUO traps are runtime traps (like Swift) or compile-time diagnostics only. This draft recommends compile-time diagnostics by default with optional runtime trapping mode.

### 3.3.2 Optional binding: `if let`
#### 3.3.2.1 Syntax (provisional)
```objc
if let y = x {
   ...
} else {
   ...
}
```

#### 3.3.2.2 Semantics
- Evaluate `x` once.
- If `x` is nonnull, bind `y` as a nonnull value within the `then` block.
- Otherwise execute the `else` block.

`y` is immutable by default. A `var` modifier may allow mutability:

```objc
if var y = x { ... }
```

### 3.3.3 Optional binding: `guard let`
#### 3.3.3.1 Syntax (provisional)
```objc
guard let y = x else {
   ... // must exit current scope
}
```

#### 3.3.3.2 Semantics
- If `x` is null, execute the else block, which must perform a scope-exiting statement (`return`, `throw`, `break`, `continue` as appropriate).
- After the guard, `y` is available and nonnull.

### 3.3.4 Unwrap operator
Objective‑C 3.0 may introduce an explicit unwrap operator for optionals:

- postfix `!` to force unwrap (unsafe; errors in strict mode unless proven nonnull)

> Open issue: this conflicts with IUO sugar; exact spelling is unresolved. Alternative: `unwrap(x)`.

## 3.4 Pragmatic generics

### 3.4.1 Goals
- Make generics useful for APIs and local reasoning.
- Keep runtime compatibility (type erasure permitted).
- Provide a constraint system sufficient for most Cocoa patterns (protocol constraints, class bounds).

### 3.4.2 Generic parameters on types
Objective‑C 3.0 standardizes generic parameters on interfaces:

```objc
@interface Array<T> : NSObject
@end
```

Type arguments are compile-time only unless explicitly reified.

### 3.4.3 Generic functions and methods
Objective‑C 3.0 introduces generic parameters on methods/functions:

```objc
- (U)map<U:(id)>(U (^)(T value))f;
```

> Syntax is provisional; the spec shall define the grammar.

### 3.4.4 Constraints and where-clauses
A generic parameter may be constrained:

- protocol conformance: `T : id<NSCopying>`
- class bounds: `T : NSObject`
- equality constraints: `T == SomeType`

A `where` clause may further constrain:

```objc
- (void)foo<T>(T value) where T : id<NSCopying>;
```

### 3.4.5 Type inference
Type inference is permitted in local contexts:
- method calls,
- block literals,
- initialization where expected type is known.

Type inference is not permitted to change ABI:
- public API signatures must explicitly name generic parameters.

### 3.4.6 Diagnostics
In strict mode:
- type argument mismatch shall be an error,
- missing required constraints shall be an error,
- “unchecked generic downcast” shall require explicit unsafe spelling.

## 3.5 Typed key paths

### 3.5.1 Key path literal
Objective‑C 3.0 defines a key path literal syntax (provisional):

- `\Type.property`
- `\Type.property.subproperty`

The compiler shall validate:
- property existence,
- accessibility,
- and type correctness.

### 3.5.2 KeyPath type
Key paths are values typed as:

- `KeyPath<Root, Value>`
- `WritableKeyPath<Root, Value>` (if writable)

### 3.5.3 Interop with KVC/KVO
A key path may be converted to:
- an equivalent selector chain or string key when permitted.

Conversions that lose type information shall be explicit and diagnosed in strict mode.

### 3.5.4 Performance requirements
Key path access may be optimized:
- direct ivar access when proven safe,
- direct method calls if `final`/`direct` constraints apply (Part 9),
- otherwise dynamic dispatch.

## 3.6 Open issues
- Final syntax for optionals and binding.
- How far flow-sensitive typing applies across property access and concurrency boundaries (depends on Part 7 actor isolation).
- Whether to add pattern matching over optionals here or in Part 5.

# Public Linux Optional and Result values

The public carrier syntax uses native Clang types and lifetime operations. It is
available in explicitly enabled ObjC3 mode on Linux x86-64 with GNUstep 2.2 and
the compatibility profile. Objective-C ARC owns object and Block payloads;
plain scalar carriers retain their native C/C++ value semantics. The [synchronous error contract](linux_public_errors.md) composes these values
with public checked calls and handlers. Async/await and actors remain reserved.

```objc
Optional(int) find(int input) {
  return input >= 0 ? Some(int, input) : None(int);
}

Result(int, int) checked(int input) {
  return input >= 0 ? Ok(int, int, input) : Err(int, int, -1);
}

int consume(int input) {
  Optional(int) value = find(input);
  return match(value) {
    case .Some(let item) => item;
    case .None => 0;
  };
}
```

`Optional(T)` and `Result(T, E)` take complete native type-names, including
ownership qualifiers and nested carriers. A typedef can name a complex pointer
or Block type. The constructors `Some(T, value)`, `None(T)`, `Ok(T, E, value)` and
`Err(T, E, error)` state the payload type explicitly; construction converts the
expression into that selected arm. A `void` arm has no stored payload.
`hasValue(value)` and `isOk(value)` query the tag once without exposing storage.
An absent Optional and an Optional containing a nil object are distinct values.

In statement context, `match` uses compound statement arms:

```objc
match(value) {
  case .Some(let item): { use(item); }
  case .None: { missing(); }
}
```

Expression arms use `=> expression;` and produce one joined result. Result arms
use `.Ok(let item)` and `.Err(let error)`. `_` ignores a payload, `let` creates a
constant binding, and `var` creates a mutable binding. `where` guards are checked
in source order; a guarded case alone does not establish exhaustiveness. There
is no fallthrough. Overlapping or unreachable cases and nonexhaustive matches
are diagnosed under the existing native coverage rules.

The scrutinee is evaluated once and an active-arm snapshot owns the selected
payload before a guard or body can mutate the source. Bindings refer to that
snapshot. Capturing a managed binding into a Block creates its own owned copy.
Expression result lifetime and native exception cleanup follow Clang's ordinary
full-expression and lexical rules. Explicit move-pattern extraction remains
outside the public subset.

In C++ and Objective-C++, `copyValue(value)` and `moveValue(value)` expose
active-arm construction. C and Objective-C use ordinary initialization and
assignment instead. Copies
preserve source values; a mutable move clears native strong/weak payloads while
preserving the source tag. A const move copies. Ordinary initialization and
assignment use those same operations. Assignment snapshots the RHS before LHS
evaluation or release of prior storage and returns the destination lvalue in
Objective-C++. Never inspect or depend on the hidden representation fields.

Supported payloads use the existing complete native representation contract:
scalar values, pointers, qualified object/Block pointers, supported trivial data
records and recursive Optional/Result values. Declaration-only dependent or
incomplete types defer representation until use. Function, reference, array,
variably modified, atomic and unsupported nontrivial object payloads are
rejected. A fixed-size array of carriers is supported as enclosing storage.
Managed record payloads and arbitrary C++ class payloads require their own
qualified operations; enclosing records containing managed carriers have the
separate [native record contract](linux_managed_records.md).

Managed values compose through ordinary functions, callbacks, C++ members,
Objective-C messages and Blocks; returns initialize fresh caller-owned storage.
The qualified native storage includes fixed-size arrays, bounded C/C++ records,
ivars, nonatomic properties, closure captures and shared Block forwarding cells.
Weak fields retain native registration and zeroing; unsafe fields retain native
non-owning behavior. Partial construction and Objective-C/C++ unwinding destroy
only completed values in reverse order. Managed MRC transfer, volatile carriers,
unsupported records and incompatible semantic import modes are rejected.

The public names are contextual only in ObjC3 carrier grammar. Ordinary native
identifiers, declarations, members, selectors, macros and qualified C++ lookup
retain their inherited meaning elsewhere. Public source requires no macro
wrappers. Canonical type and expression printing uses these public spellings;
semantic interfaces preserve canonical type, ownership, runtime, target,
language, standard and compiler identity. The installed SDK seal and package
content identities require regeneration after a compiler cutover.

`__has_feature(objc3_optionals)`, its `objc3_optional` alias, and
`__OBJC3_FEATURE_OPTIONALS__` / `__OBJC3_FEATURE_OPTIONAL__` describe this bounded
Optional contract. `__has_feature(objc3_result)` and `__OBJC3_FEATURE_RESULT__`
describe the Result contract. Queries remain false outside the qualified target,
runtime and profile or without ObjC3 opt-in. Private compiler entry points used
by AST owner tests are implementation details with no public compatibility claim.

Acceptance requires dedicated public parsing, diagnostics, native execution,
printing and PCH tests; all installed ownership/ABI/interface/storage/package
witnesses use the public grammar. Live clangd probes, ASan/LSan configurations,
DWARF and LLDB sessions, and relocated Debian 13 and Ubuntu 26.04 consumers bind
the exact published archive. Feature predicates are promoted with that evidence.

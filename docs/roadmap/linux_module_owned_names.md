# Linux module-owned native names

Source packages bind native function, global, Objective-C class, protocol and
category names to their exact module and package identity. Public and private
roots use the same ownership rule. Separate providers can reuse private source
names without sharing linker symbols or Objective-C runtime registrations.

The source builder enables `--module-owned-names` for providers and passes their
package directory as `--module-source-root`. The ordinary application keeps its
platform entry point. Direct native invocations can select the same contract:

```sh
objc3c provider.mm --objc3 --objcxx --arc --module Example.Values \
  --package values-v1 --module-owned-names --module-source-root ./provider \
  --emit-object ./Values.o --emit-interface ./Example.Values.objc3i \
  --export-declaration value -- -std=c++23
```

The owning tree includes package headers and macro expansions. System headers
and imported semantic interfaces retain their original bindings. A definition
cannot silently replace an unbound foreign ABI declaration; its owning header
must be inside the selected tree. A source root must contain the main input.

Native names have the form `__objc3_m_<owner digest>_<declaration digest>`. The
owner digest covers length-delimited module and package identities. The
declaration digest covers the native Clang USR. Paths, optimization, source
contents and the semantic fingerprint do not enter the name. Private edits and
relocation preserve bindings. A package identity change intentionally creates
new bindings and invalidates importers.
The `.__ownership_producer` module suffix is reserved for the embedded native
projection contract.

Functions and globals carry native assembly labels. Objective-C classes and
protocols carry native runtime names. Category ownership uses a reserved native
annotation. GNUstep code generation uses those names for definitions,
references, ivar offsets, inheritance, adopted protocols, typed exceptions and
direct methods. Method symbols encode class, category and selector boundaries,
so underscores cannot merge distinct selectors. Source selectors keep their
ordinary Objective-C dispatch behavior. Runtime reflection reports the owned
class name. Source-name string literals are not rewritten, and providers do not
register source-name aliases; use native class references for class lookup.

Canonical interfaces preserve these native attributes. Sidecar declaration
identities include `native_symbol_name` and require
`objc3.module-owned-names.v1`. Native import validation recomputes ownership
names, reparses canonical bindings and rejects mismatches before AST mutation.
Opposite ARC/MRC projections retain the producer's bindings. Immutable packages
carry these interfaces and objects together under the exact SDK seal.

DWARF preserves the source spelling and native runtime name. LLDB reconstructs
that binding for ivar and class expressions and preserves direct-method linker
names, including category ownership. Module-owned C functions retain their native
binding in expression calls. GNUstep JIT expressions initialize typed selectors
before message dispatch, including selectors stored in constant arrays. Installed
debugger probes verify ivars, instance and class messages, direct calls, category
direct calls and native function calls in
Objective-C17 and Objective-C++20/23 under ARC and MRC.

The contract covers externally visible global functions and variables and
Objective-C runtime roots. Internal-linkage declarations retain ordinary native
linkage. Private function and variable template specializations receive owned
names when instantiated and referenced. Private namespace functions and variables
also receive owned names. C++ members, RTTI and non-data public types retain their existing C++ ABI and
remain outside this module naming contract. Explicit conflicting ABI labels,
runtime aliases, exported inline variables and provider entry points are
diagnosed.

Installed qualification links providers with identical private function, global,
class, protocol, category and template source names. It checks native object
symbols, source/header edits, complete reuse, package identity changes and
relocated immutable consumers after deleting producer sources. The matrix covers
Objective-C17 ARC/MRC, Objective-C++20/23 ARC/MRC and C++20/23 at O0/O2.

This implementation is under development; its installed qualification and
archive acceptance are required before the current Linux acceptance receipt
advances.

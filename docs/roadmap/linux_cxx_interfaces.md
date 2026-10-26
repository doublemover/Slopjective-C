# Linux C++ semantic interfaces

Extension ABI 21 and semantic interface schema 2.1 define bounded C++ and Objective-C++ semantic interfaces on
Linux x86-64 with GNUstep 2.2. The module identity includes the language,
standard, Blocks mode, ownership, semantic profile, runtime, target and compiler
patch identity. Regenerate every dependent object and interface for this cutover.
An artifact compiled for C++20 cannot satisfy a C++23 or C-language lock.

The exported subset includes global functions, overloads, C and C++ linkage,
variables, aliases, enums and public data records. Record layout and field
ownership participate in identity; enum identity includes scopedness and fixed
underlying representation. Canonical source preserves linkage and reparses under
the original mode. Consumers reproduce the declaration and module identities
before importing any declarations. The actual target AST must match its
prepared import session.

Public, out-of-line method declarations on aggregate records are also modeled
by [the C++ async callable contract](linux_async_cxx_callables.md). They require
`objc3.cxx-member-interfaces.v1`, canonical static/instance and effect identity,
and module-owned native symbols.

The [2.8.0 delivery](linux_cxx_class_lifecycles.md) admits bounded nonvirtual
classes with private fields and out-of-line lifecycle members under the required
`objc3.cxx-class-lifecycles.v1` capability. Bases, virtual dispatch, concrete inline
or template methods, member initializers and nested declarations remain rejected. Public templates, named
namespaces, default arguments, inline definitions and constant-expression
exports require further modeling. Private implementation declarations inside an
anonymous namespace do not become exports. Unsupported public declarations
suppress both interface outputs. The required `objc3.cxx-data-interfaces.v1`
semantic capability cannot be erased from C++ sidecars.

Objective-C++ source imports retain position-sensitive visibility and qualified
lookup, including overload selection. C++20 preprocessing defers registered
names to the semantic registry before native module loading. Unregistered names
retain native loading and macro import behavior. Combined external semantic
providers forward claimed imports and qualified lookups; a claimed error stops
fallback. Imports supplied through the embedding API merge after the compiler's
ARC prelude has been parsed, avoiding duplicate builtin type-trait definitions.

Installed qualification covers C++20 and C++23 at O0, O2 and ThinLTO. Plain C++
consumers import locked scalar data APIs; Objective-C++ consumers execute ARC
record transfers, weak zeroing, C linkage, C++ linkage, overloads and qualified
names. Rejections cover unsupported declarations, mismatched standards and
ownership, premature lookup and native-loader fallback. The record and heap
suites also execute C++ semantic consumers at O0 and O2, alongside PCH and every
mixed C/C++ object combination.

```sh
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-cpp-interfaces.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./cxx-interface-evidence
```

The [current class lifecycle receipt](../support/linux_cxx_class_lifecycle_acceptance.json)
records the exact current archive and qualification. The earlier
[interface acceptance receipt](../support/linux_cxx_interface_acceptance.json)
retains the historical ABI 12 aggregate-interface, sanitizer and debugger scope.

This declaration subset uses the existing native call and ownership bounds.
Public Optional/Result, synchronous throws, async/await and actors remain
reserved until their public syntax and complete contracts are qualified.

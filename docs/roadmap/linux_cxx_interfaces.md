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
`objc3.cxx-class-lifecycles.v1` capability. The 2.9.0 delivery below extends this
with nonvirtual bases and in-class defaulted special members. That accepted SDK still rejects virtual dispatch, concrete
inline or template methods, member initializers and nested declarations. Public templates, named
namespaces, default arguments, inline definitions and constant-expression
exports require further modeling. Private implementation declarations inside an
anonymous namespace do not become exports. Unsupported public declarations
suppress both interface outputs. The required `objc3.cxx-data-interfaces.v1`
semantic capability cannot be erased from C++ sidecars.

The [accepted 2.9.0 delivery](linux_cxx_class_structure.md) additionally models
nonvirtual bases and in-class defaulted special members. Its base/defaulting
capability is mandatory. All installed gates and the exact-archive focused
Debian/Ubuntu class-package checks pass; the
[acceptance receipt](../support/linux_cxx_class_structure_acceptance.json) binds
the compiler, SDK and qualification evidence.

The [2.10.0 source milestone](linux_cxx_class_dispatch.md) adds virtual bases,
virtual dispatch, pure/final/covariant overrides and module-owned C++ type ABI
names. It requires `objc3.cxx-class-dispatch.v1`; SDK 2.9.0 remains active until
that source batch receives coordinated installed/archive qualification.

The [2.11.0 source milestone](linux_cxx_namespaces.md) adds named/nested/inline
namespace exports and namespace aliases under `objc3.cxx-namespaces.v1`, with
namespace-wide or qualified leaf selection. Its source checks and deferred
validation remain separate from the accepted SDK.

The [2.12.0 source milestone](linux_cxx_nested_types.md) additionally admits
nested named class, enum and alias declarations under `objc3.cxx-nested-types.v1`
and preserves namespace SPI/private-dependency lookup boundaries.

The [2.13.0 source milestone](linux_cxx_using.md) adds using declarations,
inherited constructors, using-enum and using-directives under `objc3.cxx-using.v1`.

The [2.8.1 correction](linux_cxx_deleted_members.md) includes deleted ordinary
aggregate methods in the lifecycle capability and canonical member identity.
Changing an ordinary method to a deleted declaration invalidates the module
semantic fingerprint.

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

The [2.14.0 source milestone](linux_cxx_declaration_expressions.md) binds concrete
default arguments and member initializers, resolved expression dependencies and
native expression identity under `objc3.cxx-declaration-expressions.v1`.

The [2.15.0 source milestone](linux_cxx_definitions.md) preserves concrete inline,
constexpr/consteval function and constructor bodies and inline/constant data
under required `objc3.cxx-definitions.v1` semantics.

The [2.17.0 source milestone](linux_cxx_defaulted_comparisons.md) adds native
in-class defaulted member/hidden-friend comparisons with required
`objc3.cxx-defaulted-comparisons.v1` semantics and record-bound comparison identity.

The [2.18.0 source milestone](linux_cxx_out_of_line_defaulting.md) preserves
out-of-line defaulted special members, native user-provided lifecycle ABI and
provider-owned definitions under `objc3.cxx-out-of-line-defaulting.v1`.

The [2.19.0 source milestone](linux_cxx_immediate_defaulting.md) preserves
consteval defaulted members and explicit out-of-line constexpr state under
`objc3.cxx-immediate-defaulting.v1` semantics.

The [2.20.0 source milestone](linux_cxx_namespace_attributes.md) preserves modeled
namespace attributes, contextual visibility and inherited namespace SPI policy
under `objc3.cxx-namespace-attributes.v1`.

The [2.21.0 source milestone](linux_cxx_lambda_expressions.md) binds local
non-generic lambda expressions, captures and copied closures in inline/default
contracts under `objc3.cxx-lambda-expressions.v1`.

The [2.22.0 source milestone](linux_cxx_thread_local.md) preserves C++ thread-local
global/class-static/inline data and local static storage under
`objc3.cxx-thread-local.v1`, including native per-thread lifetime and shared
provider/consumer storage.

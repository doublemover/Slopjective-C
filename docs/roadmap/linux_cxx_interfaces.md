# Linux C++ semantic interfaces

Extension ABI 21 and semantic interface schema 2.2 define bounded C++ and Objective-C++ semantic interfaces on
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

The current **2.29.0 SDK** delivers the C++ class, namespace, lookup and executable
expression contracts described below. The
[current acceptance receipt](../support/linux_source_batch_2_29_acceptance.json)
binds all 33 installed gates and the exact-archive Debian/Ubuntu class-package
checks. Earlier [2.8.0](linux_cxx_class_lifecycles.md) and
[2.9.0](linux_cxx_class_structure.md) deliveries established the initial lifecycle,
base and defaulting contracts; their receipts retain those historical bounds.

Public dependent/template definitions, generic/public closure types, unnamed
local/nested types and unmodeled namespace attributes remain separate work.
Private declarations inside anonymous namespaces do not become exports.
Unsupported public declarations suppress both interface outputs. Required
semantic capabilities cannot be erased from C++ sidecars.

The [2.10.0 source milestone](linux_cxx_class_dispatch.md) adds virtual bases,
virtual dispatch, pure/final/covariant overrides and module-owned C++ type ABI
names. It requires `objc3.cxx-class-dispatch.v1` and is delivered in SDK 2.29.0.

The [2.11.0 source milestone](linux_cxx_namespaces.md) adds named/nested/inline
namespace exports and namespace aliases under `objc3.cxx-namespaces.v1`, with
namespace-wide or qualified leaf selection. Its source and installed validation are included in the current SDK.

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

The [current batch receipt](../support/linux_source_batch_2_29_acceptance.json)
records the exact current archive and qualification. The earlier
[interface acceptance receipt](../support/linux_cxx_interface_acceptance.json)
retains the historical ABI 12 aggregate-interface, sanitizer and debugger scope.

This declaration subset uses the existing native call and ownership bounds.
Public feature-test predicates for Optional/Result, synchronous throws,
async/await and actors remain reserved until their complete profile contracts
are qualified.

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

The [2.23.0 source milestone](linux_cxx_friends.md) preserves concrete friend
functions/types, native access and ADL, including hidden and out-of-line inline
definitions, under `objc3.cxx-friends.v1`.

The [2.24.0 source milestone](linux_cxx_local_expressions.md) preserves published
Block bodies/captures and GNU statement expressions under required
`objc3.cxx-block-expressions.v1` and `objc3.cxx-statement-expressions.v1` contracts.

The 2.28.0 source milestone binds each declaration to its native export root in
schema 2.2. Class members and friend functions retain their owning class policy;
namespace, alias and using declarations retain their native scopes. Binary
packages validate those roots and selected namespace ancestry instead of
inferring public roots from declaration kinds. The schema cutover rejects 2.1
interfaces; regenerate interfaces, packages and compiler-dependent build outputs.

The [2.29.0 source milestone](linux_cxx_local_types.md) retains concrete named
local classes and enums, nested members and executable lifecycles under required
`objc3.cxx-local-types.v1` semantics. Lexical type/member identities survive
provider relocation and interface regeneration.

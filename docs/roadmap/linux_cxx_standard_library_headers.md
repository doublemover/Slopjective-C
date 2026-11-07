# Native C++ standard-library interface dependencies

Concurrency source 2.112 adds the required
`objc3.cxx-standard-library-headers.v1` interface capability. Public declarations
can use native standard-library types and algorithms without republishing their
implementation as package-owned declarations. The compiler extension ABI remains
24. Accepted SDK 2.62 remains unchanged while implementation continues.

The dependency walker recognizes declarations supplied by system headers reached
through a closed list of public C++ standard headers. Namespace spelling alone
does not grant that status: main-file declarations in `std` retain ordinary
publication checks. Template arguments and enclosing class arguments retain
their own visibility requirements, including arguments of standard typedefs.
The [specialization span](linux_cxx_standard_specializations.md) adds a separate
publication contract for user-authored customizations.

Each affected declaration binds the sorted public header set and a SHA-256
inventory of the actual include closure, including C, target and configuration
headers reached through it. Per-file preprocessing profiles bind implementation
and platform configuration macros at each header's end, excluding header guards.
Referenced compiler/caller macros and native feature-query predicates are included;
helper and feature-test macros published by other system headers remain outputs.
Their values remain independent of later template instantiation and source macro
changes. Source locations and absolute installation paths do not participate.
The canonical interface emits the checked includes before its
declarations. Metadata admits only bounded, sorted, unique public header names
and lowercase SHA-256 values; the marker and required capability must agree.

Both eager and source-directed imports prepare the standard headers before AST
merging. Each provider is reparsed and checked independently. A consumer may
combine different providers' header sets; every provider's header bytes must be
present in the native consumer prelude before its declaration identities are
admitted. Additional checked standard includes do not change another provider's
identity. Standard-header content caching lasts for one emission/import operation.
Source-directed imports retain the checked prelude for that parser, including
across template materialization. No cache survives an AST lifetime. Native C++
publication receives the producer's preprocessor; AST-only callers cannot invent
a missing configuration proof.

Canonical reparses preserve effective directory order/classification, macros,
sanitizer selections, optimization/inlining, PIC/PIE and ARC standard-library mode. Import preparation also uses
the completed driver invocation, including options loaded from SDK configuration
files. Matching [SDK header distribution](linux_native_standard_headers.md) is
implemented separately without repeatedly rebuilding the accepted archive.

The importer reuses declarations already parsed from matching native headers.
Matching uses header bytes, spelling offsets, native identities and printed AST
definitions, rather than merging another standard-library implementation into
the consumer. Native template identities use canonical Clang USRs, independent
of inline-namespace printing. Written friend declarations introduced by native
class instantiation reuse the matching header entity; each declaration receives
one mapping, while function imports own their parameters. Anonymous storage fields
bind their native owner and field index when Clang provides no USR. Exact imported function
redeclarations remain linked even when the two ASTs have different native template
instantiation state. The LLVM patch stack also fixes unresolved-member import to retain
the base type instead of the overload-set type, with implicit-this, explicit-arrow
and explicit-dot template-instantiation regressions in C++20 and C++23.

The compiler also imports explicit-instantiation declarations, retaining their
specialization links, lexical contexts, template arguments and context index.
Native type-location import preserves template/tag qualifiers and arguments,
function/array ranges, written builtin specifiers and function parameters.
C++20/23 source/PCH regressions pass ten commands. The AST unit checks full and
minimal imports, source text/ranges and repeated imports in four configurations;
all twelve AST infrastructure tests and the twelve affected frontend groups pass.
Libc++ execution and broader header/configuration checks continue separately.

The maintained frontend controls cover strings, vectors, arrays, callable wrappers,
shared ownership, optional values, spans, tuples, variants, durations, integral
constraints and inline algorithms. They exercise C++20/23 and Objective-C++
ARC/MRC, source-free object consumers, combined providers, source-directed imports,
location/include-order independence, private dependencies, malformed metadata,
changed header bytes and incompatible library ABI selection. All six profiles pass in the instrumented development frontend. These results
do not count as matched SDK qualification.

Twelve existing frontend control groups pass. A maintained native executable test
removes the provider source, compiles two consumers, links the three objects and
checks native values, callable ownership and common symbol addresses. Its C++20
O0 and Objective-C++23 ARC O2 configurations pass with ASan/UBSan using libstdc++.
The same two native modes also pass with `_GLIBCXX_ASSERTIONS=1` and with
exceptions/RTTI disabled. Six header profiles reject consumers with changed
header bytes, assertion configuration, exception/RTTI settings or library ABI.
A focused synthetic-header test covers inline-namespace USRs, instantiated friends,
source-free overload resolution and exact function redeclaration chains. The LLVM importer also preserves unnamed-field instantiation origins and reuses
existing template records and anonymous fields. Thirteen AST infrastructure tests
pass, including eight full/minimal C++20/23 imports into empty and prepopulated
contexts. The reduced source-free anonymous-storage consumer and both complete
libc++ provider/two-consumer executables pass: C++20 plain O0 and Objective-C++23
ARC O2, instrumented with ASan/UBSan. Provider sources are removed before import.

CTest maintains `objc3_standard_library_identity`,
`objc3_standard_library_assertions` and `objc3_standard_library_no_rtti_exceptions`.
Configuring `OBJC3_TEST_LIBCXX_ROOT` registers `objc3_standard_library_libcxx`
against that installation's headers and libraries. Configuration and vendor
execution entries use the `extended` label.

Other permitted standard-library customization forms, broader dependent lookup contracts,
consolidated package/runtime verification and the replacement SDK remain tracked.

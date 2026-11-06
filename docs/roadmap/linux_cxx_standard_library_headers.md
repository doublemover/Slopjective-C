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
User-authored specializations require a separate publication contract.

Each affected declaration binds the sorted public header set and a SHA-256
inventory of the actual include closure, including C, target and configuration
headers reached through it. Per-file preprocessing profiles bind implementation
and platform configuration macros at each header's end, excluding header guards.
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
the consumer. The LLVM patch stack also fixes unresolved-member import to retain
the base type instead of the overload-set type, with implicit-this, explicit-arrow
and explicit-dot template-instantiation regressions in C++20 and C++23.

The maintained frontend controls cover strings, vectors, arrays, callable wrappers,
shared ownership, optional values, spans, tuples, variants, durations, integral
constraints and inline algorithms. They exercise C++20/23 and Objective-C++
ARC/MRC, source-free object consumers, combined providers, source-directed imports,
location/include-order independence, private dependencies, malformed metadata,
changed header bytes and incompatible library ABI selection. Their current run
is in progress; this document does not count it as completed qualification.

Twelve existing frontend control groups pass. A maintained native executable test
removes the provider source, compiles two consumers, links the three objects and
checks native values, callable ownership and common symbol addresses. Its C++20
O0 and Objective-C++23 ARC O2 configurations pass with ASan/UBSan using libstdc++.
A pinned libc++ build is available; its consumer import exposed an unsupported
explicit-instantiation AST node, whose compiler fix and regression are in progress.

Other standard-library configurations, user specializations,
broader dependent lookup contracts,
consolidated package/runtime verification and the replacement SDK remain tracked.

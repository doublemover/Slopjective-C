# Public async source spellings

Source 2.239 / System 2.165 accepts contextual `async` suffixes and `await`
expressions in Objective-C 3.0 development mode. Free functions, function-pointer
and Block types, Objective-C methods, C++ methods and lambdas share the existing
checked function-effect machinery. Function declarators, lambda specifiers,
method-selector termination and C++ tentative parsing recognize the same syntax.
Ordinary `async`/`await` names and calls outside active effect contexts remain
ordinary identifiers; unambiguous await prefixes outside async bodies diagnose.

`async` selects the persistent shared `objc3_async_frame` physical entry in C and
C++, matching `__objc3_async_frame`. Existing internal C++ task entries retain
their separate `__objc3_async` representation. Canonical publication normalizes
the public spelling to the established ABI-specific intrinsic form; no new frame
layout or runtime service ABI is introduced. Type printing also keeps the C++
task spelling explicit, preventing a printed task type from reparsing as the
new public frame spelling. PCH and imported interfaces preserve that distinction.

This is source implementation, not full portable async acceptance. Public
async/actor feature predicates remain reserved. Source 2.240 adds
[explicit C completion exports](linux_async_c_completion_exports.md). Broader
ownership and isolation cases, full Part 7/11 profiles and matching SDK and
native ARM qualification remain open.

Maintained compiler controls cover public free/indirect/Block/method/lambda
forms, ordinary identifier reuse, duplicate effects, outside-context diagnostics,
PCH replay and distinct frame/task type printing. Native source-free controls
compile public provider objects, prove intrinsic interface equivalence, remove
the provider source and execute lazy nested/indirect/void/cancelled frames in
C17/C23/C++20/C++23 ARC/MRC. All eight native programs, 96 source/PCH commands,
126 AST controls, six selected publisher groups and 100 focused tooling checks
pass. Native execution also uncovered a C23 opaque-tag import failure: separately
checked translation units now match corresponding named incomplete tags while
retaining tag, attribute, incompatible-definition and same-AST scope rejection.
The preceding structural comparator fails the new positive C23 unit control;
all four new opaque/type-printing controls pass after the repair. Existing async
AST dump expectations retain the explicit task spelling. Fresh maintained-patch
reconstruction reproduces every changed compiler/control file.

The installed C17 witness exercises an async Objective-C method with an owned
receiver, a consumed argument, and an object result. It checks live suspension,
cancellation both before entry and while suspended, result identity, and exact
destruction. The MRC fixture releases the consumed argument from a native
`defer` after activation. The ARC and MRC O0/O2 configurations run from source,
PCH, and relocated source-free packages. This witness measures the installed
C17 ownership contract within the development profile.

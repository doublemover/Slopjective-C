# Native conditional overloads on Linux

Source **2.94** adds public `enable_if` contracts to the
[2.93 native parameter batch](linux_native_parameter_contracts.md).
The [source receipt](../support/linux_cxx_conditional_overloads_progress.json)
records the completed checks. SDK **2.62** remains unchanged.

Ordinary, template and member overloads retain native condition order and
multiplicity, including prefix-based overload priority. Native USRs bind a
canonical Sema expression profile with lexical parameter slots, types and
resolved declaration identities in place of AST pointers. Diagnostic messages
remain declaration metadata. Renaming parameters or moving source files preserves
identity; distinct operators, reference owners and condition sequences stay
separate. Parentheses retain Clang's native overload distinctions.

Import compares canonical expression shapes and their corresponding typed
bindings across AST contexts, including declaration scopes. Polished printing
preserves condition order and renamed bindings with the distinct native
assembly-label grammar for free functions and inline C++ members. Inherited diagnostic predicates also survive definitions that omit
parameter names, without changing their authored body locals.

Publication uses `objc3.native-declaration-attributes.v1`. Typed conditions and
messages bind public dependency owners; private predicate dependencies remain
rejected. Source-free witnesses exercise provider overload dispatch, template and
member calls, ordered priority, renamed/unnamed bindings and disabled calls or
address-taking. Single C entry predicates have a C17 replay witness; C overloadable
package publication is outside this batch.

All 33 frontend suites, 239 native unit tests, 412 compiler regressions and 27
focused tooling tests pass. Three platform compiler cases remain unsupported.
Source-free packages pass all 32 configurations / 224 commands across normal,
eager, ASan/UBSan and TSan. The complete async fixture adds three smoke
configurations / 21 commands. Reports bind final compiler, checker and fixture
inputs and retain earlier failures. Runtime binaries are reused unchanged;
the full five-family source/PCH runtime matrix remains recorded at source 2.91.

Regenerate compiler-dependent objects, PCH, interfaces and packages for the
cutover. This batch does not claim full profiles, general standard-library
publication, the remaining ownership/concurrency/isolation/key-path/language work,
or native Linux AArch64 qualification.

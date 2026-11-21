# Native array construction and automatic destruction groups

System source 2.43 admits direct zero-argument construction of bounded,
one-dimensional C++ arrays when an element constructor may throw. The native
construction group includes reverse cleanup of the completed element prefix.
Only successful allocation registers matched array deallocation; a construction
failure finishes prefix cleanup before that free and enclosing lexical cleanup.
A second exception from prefix destruction terminates before free.

Automatic array destruction represents the entire reverse element group. After
one element destructor throws, native cleanup completes the remaining prefix
before enclosing defer/owner cleanup and catch entry. A second escaping element
exception terminates before those outer actions. Empty arrays evaluate neither
constructors/arguments nor destructors, including noreturn element operations.
One-element arrays have no secondary-element termination path.

No repeated source operands are hidden by this zero-argument construction recipe.
Effectful/default arguments, throwing explicit list fillers, dynamic or
multidimensional allocation, placement failure and managed argument temporaries
retain their separate proof boundaries. The operation group represents native
callee-owned element cleanup; it does not expose a fabricated whole-array
destructor on failed construction.

Publication makes a hard cutover to `objc3.native-allocation-cleanup.v2` and adds
`objc3.native-array-lifetimes.v1` for published local record-array lifetimes.
Canonical identity and all semantic readers enforce exact agreement. Private CFG
changes preserve physical extension ABI 24 and current runtime ABIs.

Source/PCH controls cover C++20/23 ARC/MRC, matched failure deallocation, automatic
array cleanup, local owner expiry and zero/noreturn behavior. Native witnesses
fail each of three constructors in both heap and automatic storage, check exact
reverse prefixes and allocation/free counts, and check both secondary-construction
and secondary-destruction termination in child processes. All 60 affected compiler cases, four new source-free profiles, seven adjacent
frontend groups, codec/isolation controls, 26 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.
Matched SDK delivery follows consolidation.

# Native C++ carrier default storage and computed arrays

Source 2.132 / System 2.58 extends pure C++ Optional storage to explicit
value initialization: `Maybe{}` and `Maybe()` establish None without constructing
an inactive payload. The same policy applies to member defaults, fixed and
multidimensional arrays, partial initializer lists and explicit heap/placement
initialization. Selected-payload copy/move/destruction remains native. An omitted
initializer still cannot acquire an unknown active arm; Result storage still
requires an explicit selected arm.

Direct carrier `new[]` now accepts computed bounds and C++20 parenthesized lists,
including trailing empty Optional elements and multidimensional rows. Existing
completed-prefix cleanup handles typed error propagation and native constructor
faults independently at each element. Standard destruction and array cookies
retain the complete base-element count.

The size calculator's invalid-bound/overflow sentinel no longer reaches an
allocator for this qualified storage. Throwing allocation reports
`std::bad_array_new_length` before allocator/initializer evaluation; nonthrowing
allocation returns null. Its conditional region preserves native cleanup of
placement arguments and initializer temporaries. Negative and undersized bounds
are evaluated once, and no payload producer runs on failure. Sanitizer allocators
therefore never receive a SIZE_MAX request for these invalid bounds.

Source-free published storage operations require
`objc3.cxx-carrier-storage.v1`. Local-only empty multidimensional arrays and member
defaults retain the marker even when their implicit filler is an array row.
Declaration-only types do not invent a body requirement. Older readers and
missing/invented capabilities are rejected, with relocation and replay identity
controls in four C++20/23 ARC/MRC profiles.

All 56 dedicated positive/negative compiler commands and twelve adjacent compiler
cases pass. Seven source-free frontend groups and 300 exact production decoder
replays pass. Four selected ASan/UBSan configurations / fourteen commands cover
ARC source, optimized MRC PCH without elision and both ownership modes with
C++-only exception handling. The native witness covers empty/default/copy/move
storage, zero-length arrays, partial/multidimensional/parenthesized construction,
placement reuse, throwing/nonthrowing bound failures and typed/native initializer
failure at every element position, with exact owner balance.

Extension ABI 24, native frame 1, services 4 and schema 2.2 retain their layouts.
Mixed ARC/C++ storage and additional unqualified leaf/lifetime policies remain
separate work. Final matched analysis and one replacement SDK follow the
implementation batch.

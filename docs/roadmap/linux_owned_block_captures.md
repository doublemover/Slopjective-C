# Linux owned Block captures

Extension ABI 8 extends the Linux GNUstep 2.2 ARC contract to carrier value
captures. A Block owns an active-arm copy of each captured Optional/Result,
including nested strong, weak and Block leaves. Capturing a match binding copies
its selected value into the closure; the closure can outlive the match snapshot.
Reference, `__block`, array and enclosing-record captures need separate operations
and remain outside this contract.

## Capture lifetime

Stack literals initialize carrier fields with the same active-arm operations as
locals. Their cleanups follow Clang's native capture lifetime policy. Moving a
literal to the heap initializes a separate owned copy; heap disposal destroys
only its selected arms. Weak leaves receive registrations at their final capture
addresses. Weak captures remain weak and observe object zeroing.

Copy/dispose helper identity binds field offsets, alignment and a SHA-256 of the
complete versioned ownership encoding. Equal-sized strong and weak captures
cannot share an incompatible helper. Nested closures copy from their enclosing
capture storage. Non-escaping Blocks retain normal stack cleanup.

After each successful capture, cleanup protects that field if a later captured
C++ copy constructor throws. LLVM's ARC Block-copy intrinsic is nonthrowing;
GNUstep copy helpers can execute throwing C++ constructors. The Linux compiler
therefore calls `_Block_copy` with native exception edges so lexical and stack
capture cleanups remain active while heap copying runs.

## Runtime unwind ownership

The runtime is pinned libobjc2 2.3 plus the Linux patch inventory in
[`runtime-profile.json`](../../scripts/objc3c_toolchain_runtime/runtime-profile.json).
Preparation and verification require the exact pinned commit and patch bytes.
Unowned edits, extra files, changed patch checksums and mismatched revisions fail
without replacing source. The builder verifies source again after installation.
The runtime itself now compiles with GNUstep 2.2 and explicit exception support.

When a heap Block's copy helper throws, generated code destroys its completed
captures and the runtime frees the uncommitted allocation. Objective-C exceptions
adapted to C++ also have one owner for both the native header and C++ wrapper.
Native handlers that win after a C++ search frame discard the unused adapter and
balance its in-flight exception count. Rethrow failure preserves the object
before releasing the header.

AddressSanitizer exposed both the failed Block-copy allocation leak and leaked
native exception headers. The capture witness checks stack/heap construction
failure, Objective-C/C++ unwinding, a C producer crossing a C++ search frame into a
C handler, final object destruction and `std::uncaught_exceptions()` balance.

## Qualification

SDK publication requires `qualify-captured-carriers.py`: 77 commands, nine
executions and six deliberate forced-failure traps. Separate compilation covers
Objective-C C17 and Objective-C++ C++20 at O0/O2/ThinLTO, semantic imports at
O0/O2, C++ PCH and a producer always compiled as C17. Each executable passes
DWARF verification. The five previous installed call suites remain mandatory.

Live clangd probes inspect a captured managed value in both languages. Fresh
Debian 13 and Ubuntu 26.04 archive qualification repeats the installed suites as
an ordinary user after relocation. Additional sanitizer evidence records the
host's separately pinned LLVM 23 AddressSanitizer runtime.

The [acceptance receipt](../support/linux_owned_capture_acceptance.json) binds the
compiler, runtime patch, archive and evidence. Arrays, enclosing records,
ivars/properties and the bounded public synchronous language contract remain
the next implementation work. Public Optional/Result/throws predicates remain
reserved.

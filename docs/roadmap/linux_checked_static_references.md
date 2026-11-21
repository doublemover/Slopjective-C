# Static and thread-local checked references

Source 2.128 / System 2.54 extends checked native C++ and pure C++ carrier
references to static and thread storage. Each referent has one provisional owner
until the entire initializer full expression finishes. Initializer temporary
failure destroys that owner; successful completion registers permanent native or
active-payload destruction once. TLS registration uses the thread lifetime of the
extending declaration. Repeated access preserves referent identity.

A completion-only entry in the existing lifetime-extension cleanup queue commits
static registration without leaving a lexical cleanup above the C++ guard.
Reference initialization forces its full-expression cleanups before releasing the
guard. Typed-error propagation and native-frame cancellation execute guard abort;
ordinary success deactivates abort and commits guard release. Consequently an
interrupted initializer can retry instead of deadlocking or exposing incomplete
storage. Existing ordinary C++ behavior is retained outside Objective-C 3.0.

The maintained native witness exercises two failed attempts for static/TLS native
and optional references, None reuse, successful TLS native/Some ownership, exact
thread-exit destruction, nested typed-failure retry and two cancellation/retry
cycles followed by successful commitment. Dedicated compiler controls exercise
all four profiles, C++20/23, ARC/MRC, source/PCH and disabled elision. Source-free
publication/replay preserves static/TLS references and semantic fingerprints in
four configurations. Throwing-destructor checked results retain their explicit
diagnostic. All 53 compiler commands, four source-free profiles and the complete sixteen-
configuration ASan/UBSan native matrix / 56 commands pass, including source/PCH,
C17/C++20/23, ARC/MRC, C++-only exceptions and disabled elision.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
These are development-source receipts; final matched analysis and one replacement
SDK remain batched after implementation.

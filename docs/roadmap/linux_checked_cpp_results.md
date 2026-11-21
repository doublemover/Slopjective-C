# Native C++ checked results and initialization ownership

Source 2.124 / System 2.51 admits synchronous checked calls returning native,
indirect C++ records with nonthrowing destruction. Direct and indirect functions,
C++ virtual/member calls, Blocks and dependent templates use their native result
storage plus the existing error output. Isolated synchronous property getters now
use the same result ABI. Register-passed/trivial-ABI records, throwing destructors
and general nullable synchronous Objective-C sends retain explicit gates.

A checked failure returns no constructed object. The callee preserves live
return storage until ordinary lexical cleanup finishes, then destroys a completed
result if error cleanup supersedes its return. Named return value optimization
hands ownership from its local cleanup to the return guard at scope exit.
Exceptional exits before that handoff retain the local's destructor. Native
constructor-prefix cleanup remains responsible for failed return construction.

The caller binds result temporaries after checking the error output. Fresh local
and array initialization retains a provisional owner while source temporaries
are destroyed, then transfers to the permanent lexical cleanup. Raw synchronous
calls cannot mark that owner initialized before their error check. Reference
materialization separates its early exceptional owner from the later lifetime
extension. Native awaits protect each copied record across child/argument cleanup,
including an array element not yet registered in its completed prefix. These
paths share the existing construction guard and require no public layout change.

The maintained property witness now exercises exact owner/resource counts across
success and typed/native failures before construction, after construction, during
return cleanup and in parameter destructors. It includes relays, function pointers,
virtual adjustment, Block and template calls, discarded/forced results, move-only
heap owners, partially throwing moves, partial arrays, completed arrays, reference
binding and native-await copies. Direct synchronous error tests drain their own
autorelease pool before checking object destruction.

All 156 selected compiler cases pass, including 57 dedicated commands across
Core, Strict, Strict Concurrency and Strict System. All 48 AST controls, sixteen
ASan/UBSan native configurations / 56 commands, four new native-result source-free
profiles, twelve updated property profiles, nine adjacent frontend groups and
codec/isolation controls pass. The native matrix includes C17/C++20/23, ARC/MRC,
source/PCH, C++ exception-only and elision-disabled execution. Nine real artifact
families pass 180 exact production decoder replays, with changed-byte and
unsupported-capability controls.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 remain
unchanged. These are development-source receipts. General nullable synchronous
Objective-C record sends, register-passed/trivial-ABI results, throwing record
destructors, broader aggregate/placement and global lifetime regions, full matched
analysis and the replacement SDK remain tracked separately.

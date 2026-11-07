# Completed managed initialization prefixes

System source 2.39 tracks ownership acquired by completed ARC fields and capture
slots while a later initializer runs. Primitive managed records, constant arrays,
nested layouts and local C++ owning lambdas preserve the already initialized
prefix. A failed initializer releases that prefix in reverse initialization order
before enclosing cleanup, without destroying the incomplete whole object. Normal
completion transfers ownership to the completed variable or closure.

The lifetime proof snapshots both owner identity and possible unowned state at
the actual expression evaluation. Later initializers can clear or replace the
source binding without erasing an earlier field's ownership. Conversely, copying
an already expired source does not restore a valid loan. Flow joins preserve
possible unowned state. Owned field/element aliases and closure-held loans retain
matching identities when a temporary initialization pin ends. Reference fields
and reference captures own no claim on their referents.

The lambda CFG now follows the native capture initialization order. Reverse graph
construction visits captures backwards so that execution visits them forwards.
A capture copied before a mutation can keep its object alive; a capture read after
that mutation cannot fabricate the earlier claim. Local direct lambda
initialization also recognizes its elided outer temporary, avoiding duplicate
destruction while retaining the variable's actual native lifetime.

Private metadata cloning copies values before insertion can grow its map. The
new nested ownership controls exposed and defend a compiler use-after-free at
that boundary. Runtime prefix pins reuse existing CFG lifetime nodes, with no
public AST layout change. Physical extension ABI 24 and native service/frame
layouts remain unchanged.

Published managed initialization and owning-capture bodies carry
`objc3.initialized-owner-prefixes.v1` with matching canonical identity. All
artifact, module, import and codec readers enforce agreement. Source-free replay
reconstructs the private prefix registration from the original body.

The admitted recipe contains primitive ARC ownership and trivial scalar/reference
leaves. User destructors, base/union construction, opaque nontrivial captures,
unmodeled full-expression temporaries and wider constructor/destructor partial
object cleanup retain their separate gates. Discovery is bounded by 64 owner
pins, 256 leaf visits per initializer, 4,096 prefix entries, 8,192 source visits
and the existing 4,096-origin proof envelope. Exhaustion fails closed.

Controls cover C17/C++20/23, ARC/MRC and source/PCH, including normal completion,
failed first/later initializers, nested records/arrays, outer pins, finalizers,
owning value/reference captures, capture order and expiry after unwind. The
installed optional `runtime-scopes` witness clears the original owner during a
later initializer and optionally throws, checking live payloads and exact
destruction in records, arrays, nested layouts and C++ lambdas. Source-free and
all 77 affected compiler cases, six source-free profiles, five adjacent frontend
groups, codec/isolation and all nineteen AST controls pass. Twelve ASan/UBSan
native configurations / 42 commands pass after the final compiler rebuild.

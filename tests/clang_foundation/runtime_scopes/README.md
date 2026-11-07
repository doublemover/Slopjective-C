# Managed runtime scopes

Strict System C17/C++20/23, ARC/MRC, source/PCH, O0/O2. Optional
`--feature runtime-scopes` installs and runs the complete 24-configuration matrix.

The native witness checks pool return/goto/break/continue, deferred consumption
before destruction, and exact queued MRC release counts. Synchronization evaluates
its operand once and retains its ARC receiver after the original binding is
cleared. MRC uses an explicit lifetime pin. Deferred consumers run before unlock
and object destruction on return, goto, fallthrough and native exception paths.
Runtime interposition checks one enter/exit and verifies that the object is
still live at the actual unlock. A catch verifies the same exception object
and completed owner cleanup. A separate exception witness observes that an
ordinary native pool did not pop, then explicitly closes its recorded token
to leave no leaked pool. Pointer
witnesses come directly from inline storage, avoiding an accessor's separate
ARC autorelease lifetime extension. Every source object is destroyed exactly once.

Compiler controls separately check normal-only synchronous pool cleanup,
async pool drains, suspended lock rejection, nested pins, pending release counts,
callback effects and expiry. A native synchronous pool's exception semantics are
not changed by the ownership CFG model.

The finally witness checks exact defer/handler/finalizer order, return/goto and
loop exits, finalizer return replacing an earlier return or exception, outward
backward goto, replacement exception identity and mixed C++ unwinding. Each
transfer checks borrowed payload access and exact object destruction.

ARC partial-initialization witnesses initialize records, arrays, nested records
and C++ owning lambdas before a later initializer clears the original owner and
optionally throws. Borrowed payload access stays live through the initialized
prefix. Every successful or exceptional path destroys the object exactly once.

C++ disposal witnesses check destructor/deallocator/defer/catch order, exact
allocation/free counts, a null operand with a noreturn destructor and controlled
fault detection when a deallocator violates its nonthrowing contract during
unwind. The termination case
runs in a child process with an explicit exit-code witness.

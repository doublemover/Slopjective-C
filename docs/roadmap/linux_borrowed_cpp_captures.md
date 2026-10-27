# Borrowed provenance through C++ lambda captures

The System 2.1.0 source span extends mandatory Strict System borrowed-pointer
checking to nonmutable, nongeneric C++ lambda value captures. Ordinary pointer
aliases, init-captures, record fields and nested lambdas retain the provenance
present at capture creation. Closure copies and calls through `operator()` or a
qualified `noescape` parameter preserve the obligation. Passing the closure to
an unqualified parameter, returning it or hiding an escape inside its body is
rejected.

The enclosing flow rechecks each lambda body with its converged capture facts,
including aliases that lost the static `borrowed` qualifier. Init-capture values,
owner identities and local address targets enter that nested analysis. ARC
object captures can independently retain the borrowed value's owner, so replacing
the original owner does not invalidate a locally held closure. Without such an
owner or explicit lifetime pin, calls after owner expiry are rejected, including
explicit `operator()` calls.

Reference captures retain a conservative boundary. A currently borrowed
reference/address capture is rejected. Capturing an initially empty slot cannot
hide a later borrowed assignment: reference targets include transitive local
address chains and aggregate address fields. Assigning a borrowed value into
such storage after closure creation is rejected. Nested checking also rejects
borrowed values stored into persistent lambda captures, including mutable fields
in a const closure. This applies even when borrowing first occurs inside the
lambda body.

Mutable or generic lambdas that capture a borrowed value require further lifetime
contracts. General callback effects, precise mutable/reference capture state,
nontrivial capture constructor effects, interprocedural MRC releases and broader
alias/union precision remain open. No full Strict System conformance claim is
made. Native extension ABI 21 and the existing runtime ABI are unchanged; the
compiler patch identity requires rebuilding dependent PCH, objects and packages.

Maintained witnesses cover C++20/23 under ARC and MRC, direct source and PCH,
local/copy/noescape invocation, init/nested/record captures, captured ARC owners,
escapes and expired calls, future reference/address mutations and persistent
capture storage. SDK 2.33 remains the accepted installation while this compiler
span is qualified with the next coherent source batch.

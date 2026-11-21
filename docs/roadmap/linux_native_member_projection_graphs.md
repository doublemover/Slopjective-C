# Closed native member projection graphs

Source 2.215 / System 2.141 composes receiver-independent C++ helpers across
member calls. A method can forward its receiver to another proved member body;
ordinary arguments still retain their exact original scalar/reference/Block and
constructed-environment sources. Qualified virtual calls, final methods and
methods declared in final classes select a closed body. Open virtual dispatch,
operator/member-pointer/explicit-object calls, opaque targets, recursion,
receiver field state/mutation and escaping or captured `this` remain unproved.

The receiver graph shares 1,024 statement/work units, a bounded pending frontier,
a 16-level depth limit and an active-definition cycle set. Weak/interposable,
variant, async, executor/actor and other unsupported bodies cannot hide receiver
use through a helper. Existing value/alias, factory, transfer and MRC copy checks
remain independent and active.

Concrete already-used implicit method-template specializations can require their
selected body before Clang's usual deferred instantiation. The proof materializes
only visible, nondependent, valid, pending specializations whose patterns contain
dependent calls through the receiver. It retains the current-callable guard and
uses the same targeted Sema entry point as the borrowed-result proof. Preparation
has separate 256-node work/frontier limits and a 16-instantiation budget; it does
not drain unrelated pending work or treat an unresolved pattern call as a concrete
callee. This covers scalar and work-factory chains without losing caller resource
contracts.

All 72 source/PCH commands pass across C17/C++20/C++23, ARC/MRC and all four
profiles. Controls include generic factory/scalar chains, exact reference-cell
writes, qualified/final virtual forwarding, resource copies, unknown callbacks,
opaque/recursive helpers, receiver state and open virtual delegation. The four
optimized C++20/C++23 × ARC/MRC native configurations pass as a complete
configuration union: 35 work shapes / 4,480 scenarios across two eager and two
source-directed consumers, with source removal and all four cancellation routes.
Exact retirement, owner, work, defer, pause and terminal results remain checked.
The selective-run receipts are a complete configuration union, rather than a raw
full-suite CTest qualification.

Fresh maintained-patch application and 16 adjacent tooling checks pass. ABI 24,
interface schema 2.2 and reader capabilities are retained. Full-profile, native ARM
and matching SDK/archive acceptance remain separate work.

# Single-provider C inline definitions

Concurrency source 2.111 completes ISO C inline publication, including explicit
external-inline definitions, a later external declaration and a definition whose
inline specifier comes from an earlier declaration. The compiler extension ABI
remains 24 and the interface capability remains `objc3.c-inline-definitions.v1`.
The accepted SDK 2.62 archive is unchanged.

A semantic module owns one strong provider definition for each public C inline
function. Paired consumers temporarily supply external storage only while native
code generation observes provider definitions, then restore the source AST.
Parsing, Sema, interface identity and source publication see the original source
storage. Imported/foreign declarations and ordinary non-module C compilation
retain their existing emission rules.

Canonical interfaces normalize every consumer redeclaration to ISO inline
without external storage. Definition selection and inherited inline flags retain
written bodies and parameter names. This prevents a later plain declaration from
silently forcing another external definition in every consumer. Scope guards
restore both explicit and inherited inline bits after printing.

The contract requires ISO inline semantics. GNU inline modes diagnose and leave
no output product. Mutable local static/thread objects cannot be projected into
independent inline bodies; immutable local static storage retains the C contract.
Unknown attributes and private/unresolved dependencies keep their existing checks.

A maintained native test passes six configurations: plain C17, Objective-C C17
MRC and ARC, each at O0 and O2. Each checks four strong provider symbols in LLVM
IR, removes provider source, compiles two consumers from the canonical interface,
checks their IR for absence of duplicate external definitions, and links/runs the
result. Both consumers call all four functions and observe the same function
address. GNU rejection also verifies output rollback. Non-module ISO inline
emission is independently checked. Objective-C witnesses link the matching
GNUstep runtime. The test is registered in the normal assertion-enabled CTest
inventory with a bounded timeout.

The C-inline, immutable borrowed storage, memory alias/byte-range and concurrent
Block frontend groups also pass after the projection change. These are current
development-driver/header/DSO controls, not installed SDK qualification. Broader
package integration, remaining implementation, wider instrumentation and final
SDK delivery remain pending.

The native provider test also passes four Objective-C ARC/MRC O0/O2 configurations
with four inline native-frame functions: ordinary, external, later external and
an awaiting chain. Each source-free consumer builds factories whose frames start
and complete with the expected result; unused frames dispose before start. The
runtime witness checks 32 completions, 16 unused-frame disposals, common function
addresses and eight strong provider symbols per Objective-C configuration. The
plain-C configurations retain their four-symbol controls. Subprocess linkage
uses the current runtime archive and configured instrumentation flags. This is
focused development execution; matched SDK/package qualification remains pending.

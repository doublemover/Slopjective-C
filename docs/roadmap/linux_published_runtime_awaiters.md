# Published SDK runtime awaiters

The concurrency 2.36.0 source span extends concrete C++ inline async definitions
to the pinned SDK's checkpoint, yield, executor/actor destination, typed task
handle and task-group operations. Canonical source keeps the written await
operand instead of publishing the SDK promise's generated await protocol.
Existing logical call and isolation projections remain in use.

Public runtime service declarations can now appear in exported signatures and
bodies. Qualified task types and operations select the complete C++ bridge;
service-only declarations select the C runtime header. Its integer/size aliases
require the matching target builtin representation without added attributes.
Both bind the exact
embedded service header, checked native frame layout and required
`objc3.sdk-runtime-declarations.v1` semantics. Codec, contract, artifact, module
resolution and import readers enforce the capability. Removing the capability
or its runtime-header requirement is rejected. Changed/lookalike headers and
application definitions of opaque runtime records fail qualification.

The task declaration boundary admits the maintained public task, handle, group,
destination and failure APIs from the exact task header. SDK-owned result ABI
alias sugar is accepted without admitting the private traits record or its
members. Private implementation bodies do not contribute public runtime
requirements. Nested coroutine callables still require their own publication
contract.

Prepared imports own their virtual header buffers until after AST destruction.
Clang Tooling borrows those strings; freeing temporary mappings previously left
deferred source-location imports reading invalid bytes. A service-only import
regression also removes the original headers before consuming the interface.

An inline definition with a module-owned asm label can also carry a task-group
attribute. Canonical printing temporarily removes the label while preserving
and restoring the complete attribute set, including repeated serialization.
The CLI now includes diagnostic subjects so rejected dependency names are
visible during interface emission.

The source-free package fixture publishes runtime requests and signatures,
removes provider source, then compiles and executes consumers. It checks distinct
manual-executor hops, inline class methods and exported checkpoint constants,
actor identity before and after yield, task-handle and raw
C-handle joins, completion-order group iteration and scope cleanup. The emitter
fixture checks repeated canonical round trips, required semantics, exact header
provenance, opaque definitions and private SDK dependencies.

This is a source implementation checkpoint. SDK 2.33 remains the accepted
installed archive. Service ABI 4, frame ABI 1, native extension ABI 21 and
interface schema 2.2 are unchanged. Dependent/generic bodies, public closure
types and arbitrary custom coroutine promises remain separate work.

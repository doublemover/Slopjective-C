# Linux managed typed-error calls

Extension ABI revision 9 composes direct ARC Optional/Result parameters and
results with the private synchronous typed-error transport on Linux x86-64,
GNUstep 2.2. Providers, consumers, PCH and semantic interfaces must be rebuilt
with one sealed SDK. Public Optional/Result/throws predicates remain reserved.

## Ownership and control flow

Managed parameters keep the ordinary caller-owned snapshot contract. The
callee borrows each slot and may replace its selected arm. Caller cleanup
destroys the final contents on success, typed propagation, catch recovery and
native exception unwinding, including when a later argument fails.

A successful managed result initializes fresh caller-provided storage. A typed
failure initializes an empty result: Optional is none; Result is Err with a
zero payload. Propagating and forced `try` inspect the error output before the
result becomes a source value. Assignment preserves its old destination when
propagation exits the expression. Discarded and forwarded results retain the
ordinary ownership and full-expression cleanup rules.
Tag queries on forced or propagating managed results use an owned temporary,
with the same cleanup after a successful query or an interrupted expression.

The callee owns a completed result until its return-expression, lexical and
parameter cleanups finish. If a native exception interrupts return, an EH
cleanup destroys the initialized result. The caller never adopts that result
and preserves its previous assignment target. The same protection applies to
ordinary managed returns.

A pending typed error holds an independent strong reference through callee
cleanup and autorelease-pool exit. Only the actual return path publishes it to
the caller's pool. Propagation retains it before caller argument and lexical
cleanup. Catch handlers own their error through their body. Native Objective-C
and C++ exceptions retain their separate control flow and unwind both pending
errors and any completed managed result.

## Callable and metadata contract

The qualified subset includes free functions, function pointers, C++ static and
virtual members, adjustment thunks and member pointers, heap Blocks with
ordinary object captures, and class/instance/protocol/superclass messages.
Signatures use the ordinary nonvariadic C calling convention. A hidden
autoreleasing object-pointer output follows the source arguments. Method and
Block reflection includes that output as `^@`; the indirect result pointer is
not a source argument. Managed encodings use revision-5 nominal names and keep
strong, weak, unsafe and Block leaf ownership distinct.

Nil message receivers return an empty managed result without a typed error;
their caller-owned argument snapshots still clean up.
[True direct dispatch](linux_direct_dispatch.md) now uses the same ownership
and error contract under GNUstep 2.2.

The [recursive carrier contract](linux_recursive_carriers.md) adds optionalization
of managed carrier results, including weak and Block leaves, failure and discarded
results. [Owned Block value captures](linux_owned_block_captures.md) now compose
with these calls. Carrier arrays, enclosing records, reference captures and ivars, MRC,
volatile carriers, alternate calling conventions, variadics, plain aggregate
throws results, typed-error constructors and reference-bound returned temporaries retain their existing
boundaries. Generic aggregate FFI and invocation adapters need explicit owned
value semantics.

## Installed qualification

SDK publication runs `share/objc3c/examples/native/qualify-managed-throws.py`
outside the sealed prefix in paths containing spaces and quotes. It requires
nine successful executions: Objective-C and Objective-C++ at O0, O2 and ThinLTO,
two locked semantic-interface consumers, and a C++ PCH consumer. Every program
passes DWARF verification. Eighteen subprocess checks verify that forced typed
failures trap across functions, messages and Blocks in all six native build
configurations.

The witnesses exercise strong/weak/unsafe/Block leaves, both Result arms,
parameter replacement, failed assignment, direct result queries, throwing
template forwarding, failure in a later argument, nested propagation and pools, nil
receivers, metadata, register exhaustion, native Objective-C unwinding and C++
destructors throwing after success or during typed failure. Fresh Debian and
Ubuntu archive qualification repeats the gate as an ordinary user, without a
checkout mounted. Live clangd probes inspect a managed forced-try result in
both languages.

The [acceptance receipt](../support/linux_managed_typed_error_acceptance.json)
binds the compiler, SDK archive, source inputs, installed executions, live editor
and debugger checks, and fresh-distribution logs for this milestone.

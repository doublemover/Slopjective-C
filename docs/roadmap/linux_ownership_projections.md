# Explicit Linux ownership projections

An ownership projection publishes a consumer-mode view of a closed native C ABI.
The implementation object is compiled in its original ARC or MRC mode. The
projected semantic interface carries its canonical producer proof and uses the
consumer's exact language, standard, target, runtime, profile and compiler identity.

```sh
# Compile an MRC provider and emit its ARC-facing interface.
objc3c provider.m --objc3 --project-ownership arc \
  --emit-object provider.o --emit-interface Projected.objc3i \
  --module Projected --package provider-v1 -- -std=c17

# Compile an ARC provider and emit its MRC-facing interface.
objc3c provider.mm --objc3 --objcxx --arc --project-ownership mrc \
  --emit-object provider.o --emit-interface Projected.objc3i \
  --module Projected --package provider-v1 -- -std=c++23
```

The qualified boundary is Objective-C17 and Objective-C++20/23 on Linux x86-64,
GNUstep 2.2, in the ObjC3 compatibility profile. Objective-C++ functions require
C linkage. Provider and consumer preserve the same language and standard; only
the ownership mode changes. A projection must select the opposite mode and have
no interface dependencies. Consumers import its new fingerprint with their
ordinary eager or source-level module locks.

## Native ownership contract

The public API contains only prototyped, nonvariadic C ABI functions. Supported
value types are real scalar types, one-level pointers to scalars or void, and
unqualified native `id`. Object parameters are borrowed according to native
calling rules. Object returns require one explicit native return attribute:

```objc
id create_value(void) __attribute__((ns_returns_retained));
id borrowed_value(void) __attribute__((ns_returns_not_retained));
id autoreleased_value(void) __attribute__((ns_returns_autoreleased));
int inspect_value(id borrowed);
```

ARC callers adopt retained results and retain borrowed results when their own
storage needs ownership. MRC callers perform the corresponding explicit native
retain/release operations. Providers implement the declared native return
contract in their own mode. Autorelease pools retain their native semantics.
ARC consumers can place a returned object in a public Optional and use native
matching, assignment and weak references without transferring a carrier across
the boundary.

Exported records, typedefs, variables, Objective-C containers, callbacks, Blocks,
references, ownership-taking parameters, weak/unsafe API types, variadics,
unprototyped functions, C++ exception specifications and ObjC3 async/throws
signatures are rejected. Named class/protocol object types and managed carrier
transfers need separate contracts. Private implementation headers and classes
may implement the closed function API.

## Provenance and publication

Semantic interface schema 2 requires an `ownership_projection` field. It is empty
for an ordinary interface. A projected interface contains the canonical producer
sidecar as a bounded JSON string and requires `objc3.ownership-projection.v1`.
Its producer module name is the projected name followed by
`.__ownership_producer`; its package identity is identical. Producer and consumer
share the exact canonical declaration bytes. The embedded producer sidecar is
limited to 64 KiB; nested projections and dependency closures are rejected.

The semantic fingerprint includes the producer proof. Import reconstructs the
producer in the opposite ownership mode, checks the bounded API, re-emits its
canonical source and sidecar, and requires exact agreement. It then reconstructs
the consumer interface using normal native import validation. Target, compiler,
standard, package, return ownership and declaration changes cannot be hidden by
editing the proof. Projection emission shares the existing transactional object,
interface and sidecar publication boundary.

Source workspace/package v3 manifests require explicit `provider_ownership` on
every package, alongside the consumer `ownership`. Both use `none` for plain C++.
A closed projection package selects opposite ARC/MRC modes; its dependents keep
normal consumer-mode locks and may re-export that interface. The build report
and installation inventory record the actual provider mode. Binary package v2
preserves it through relocation and source-free consumption and checks its
agreement with native provenance before invoking the compiler. Earlier interface
and package formats are rejected and must be rebuilt with the current SDK.

Installed qualification covers both ownership directions in all three language
modes at O0/O2. Each of twelve cases executes its initial graph, verifies and
executes complete compilation reuse, then relocates an immutable package,
deletes producer sources and build outputs, and executes a source-free consumer.
The gate records 36 executions and 96 provenance rejection checks, including
optimization-stable fingerprints and transitive re-export visibility. Runtime
witnesses observe retained factories, borrowed and autoreleased returns, final
release counts, ARC Optional lifetime and weak zeroing. Separate sanitizer runs
instrument the mixed-mode provider and consumer code.

# Linux async C++ callables

The reserved Objective-C++20/23 async stage extends logical-result task lowering
to ordinary C++ member functions, static members, const and ref-qualified members,
member pointers and overloaded operators. Virtual calls preserve the physical
`objc3::task<R>` return ABI, including secondary-base `this` adjustment thunks.
Native await remains an expression in conditions and standalone statements,
including indirect member calls. The canonical callable type keeps its logical
result, async effect and exact
checked error type. Checked calls surround the await with the same checked forms
as [free functions](linux_async_checked_errors.md).

Implicit receivers are borrowed C++ objects. Keep the receiver alive until its
member tasks finish or are cancelled and released. Explicit object parameters
passed by value in C++23 use ordinary coroutine parameter ownership and survive
the original object's later mutation or destruction. Suspension, private-child
cancellation, native exceptions and explicit actor hops retain the existing task
runtime rules. These calls do not provide compiler actor isolation.

PCH imports preserve member pointer identity, inline member templates and checked
member effects. Source-free packages additionally export public, out-of-line
method declarations on aggregate records with public fields. Those interfaces
require `objc3.cxx-member-interfaces.v1`, preserve static/instance identity and
assign module-owned symbols to methods and member operators. Method signature,
error type and static/instance changes participate in the semantic fingerprint.
Consumers reject missing or unknown required interface capabilities.

The package surface retains the existing aggregate restrictions: no bases,
virtual dispatch, explicit special members, inline methods, member templates,
default arguments, field initializers or private fields. Native header/PCH
consumers support virtual dispatch and member templates. Covariant async returns
are diagnosed because task carriers are invariant in their logical result.
Constructors, destructors, conversion functions and allocation/deallocation
operators do not acquire a task ABI.

The current extension ABI is 17 and the task service remains ABI 3. The
[checked value result stage](linux_async_value_results.md) adds records, complex
values and member pointers with a reserved preflight/resume handshake. Rebuild
compiler-dependent objects, providers, PCH files, semantic interfaces and packages
with the new compiler. Pure Objective-C async lowering, Objective-C methods,
Blocks and compiler actor isolation remain separate milestones.
Public async/await and actor capabilities remain reserved.

The [installed SDK acceptance receipt](../support/linux_async_value_results_acceptance.json)
binds this contract to the exact compiler, runtime, inputs and sealed archive.
It records 211 compiler tests, 57 AST tests, 24 frontend tests, eight GNU runtime
tests, 318 current tooling tests, 37 installed driver/embedding checks and all
22 installed package gates. Sixteen C++20/23 O0/O2 configurations execute every
ARC/MRC provider-consumer pairing; eight LLDB cases inspect resumed member `this`
and local values alongside scalar, owned-result, checked-handler and checked
C++ value frames.
Eight source-free package configurations execute 24 times after provider source
removal, publication and relocation.

The same SDK passes 74 instrumented configurations and 356 commands, plus
standalone C++/extension-ABI checks and six post-hop debugger cases. Fresh Debian
13 and Ubuntu 26.04 consumers run as ordinary users under the explicit
`task-destinations` scope; all 22 package gates run on the publication host.
Sanitizers instrument services and clients; GNU libobjc is loaded as shipped.
The [latency report](../support/linux_async_value_results_latency.json) records
five quiet repetitions against the existing release budgets.

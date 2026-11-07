# Catch binding ownership and native storage views

Compile `main.m` under Strict System as C17 or C++20/23, ARC/MRC, source or PCH.
For PCH use `OBJC3_CATCH_BINDING_OWNERSHIP_PCH`. Optional feature
`--feature catch-bindings` maintains the full 24-case O0/O2 matrix and installs
this fixture with the SDK.

Each native handler drops the original caller claim and acquires a declared
borrow from its catch owner. Return, rethrow, outward goto and fallthrough must
run the deferred consumer while the object remains live. The consumer reads the actual inline payload address under the owner contract.
Receiver-owned native getters and property reads, inline array/nested-record
loans, C++ reference record copies and builtin
address-taking also remain valid while a lifetime pin holds the object after
its original claim is dropped. Exact destruction counters verify native object
lifetime and ordering.
Every source object is destroyed exactly once after handler cleanup and the
per-case autorelease pool. Objective-C and Objective-C++ can retire the throw
operand's autorelease claim at different permitted points.

Compiler controls independently reject loans used after catch exit/rebinding,
rethrow cleanup and opaque borrowed exception escapes. A CFG path oracle checks
body work, deferred work and the single catch lifetime end on every handler exit.
Source-free publication/replay/relocation and valid/expired consumers keep these
contracts under the existing borrowed ownership semantics.

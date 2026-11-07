# Awaited isolated property operations

Compile `main.m` as C17 or C++20/23, under ARC/MRC, with matching source/PCH.
For PCH transport define `OBJC3_ISOLATED_PROPERTIES_PCH` in the consumer.

The fixture checks actor and executor getters, assignment result values,
compound updates, pre/post increment, custom accessors, record and object values,
caller-side operand evaluation order, actor-turn continuity, class/super dispatch,
actor backing-executor affinity, same-actor access and exact ownership. Independent
record snapshots survive receiver/later-argument mutation, including ARC fields.
A normal property expression and a native-frame expression independently check
ordering against ordinary CodeGen. The inherited layout reuses superclass tail
padding and defends signed GNUstep ivar metadata.
Nil cases must finish using only the caller executor, including an unregistered
destination. Constructed nil reads fail without construction; nil assignments
preserve their independently valid RHS. C++ task callers, unstarted disposal and cancellation before parent entry,
before child entry and after the actor hop retain exact accessor/argument counts.

`checked.h` adds synchronous checked accessors, compatible getter/setter error
contracts, exact getter/setter failure boundaries, identity-preserving recovery,
class/super/actor dispatch, nil, cancellation and nontrivial C++ setter arguments.
It runs in the same maintained source/PCH witness and installed example.

`async.h` covers suspended getter/setter composition, mixed accessors, same-actor
and same-executor checks, class/super dispatch, C++ task accessors, owned results,
nil and cancellation between accessors and during setter suspension.

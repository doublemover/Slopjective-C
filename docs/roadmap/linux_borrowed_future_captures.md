# Owner requirements of later-invoked closures

The System 2.7.0 source span tracks owners used to acquire borrowed data inside
concrete synchronous C++ lambda and Block bodies. Creating a closure before
consuming its raw owner no longer hides a later borrowing operation. Known
consumption or owner-binding expiry invalidates subsequent invocation, copying
or escape of that dependent closure, even when it captured no interior pointer
at construction.

Each body receives symbolic capture roots and runs through the existing CFG
solver. The resulting owner-use summary is cached once per closure syntax and
shared by nested analyses. This preserves the frontend's polynomial dataflow
bound rather than recursively inlining bodies. Ordinary local pointer aliases,
assignments and conditional joins contribute their actual possible owner roots.
Overwriting an alias before borrowing does not require an unrelated captured
owner to stay live. Copied, init and nested captures retain frozen dependencies.

Independent ARC object captures and explicit strong/move Block owners keep their
own ownership. Existing object lifetime pins can protect nonowning captures;
ending the last pin expires that protection. Calls before consumption and
qualified noescape invocation remain accepted. Owner-dependent escaping closures
are rejected. Reference/mutable/generic captures retain explicit qualification
boundaries, and raw owner init-captures without a stable source binding diagnose.
Specific nested-body errors take precedence over a redundant generic capture
boundary diagnostic.

This span does not implement whole-program effects, global/indirect owner
mutation, general MRC/foreign ownership counts, arbitrary record-field owners or
lazy async result lifetime requirements. Unqualified closure and borrowed-return
relationships remain separate work. Full Strict System conformance is not
claimed. Regenerate dependent objects, PCH, interfaces and packages for the
compiler identity cutover; extension ABI 21 and runtime layouts remain unchanged.

Maintained witnesses cover C++20/23 under ARC/MRC, source/PCH, value/init/copied/
nested captures, local and overwritten/conditional aliases, Block bodies,
independent ARC/explicit Block owners, object pins and pin expiry, escape
boundaries and a twelve-level nested summary. Frontend witnesses add source-free
publication/reparse of a concrete local borrowing closure.

The [combined ownership receipt](../support/linux_closure_family_ownership_progress.json)
records this span with the other System 2.7–2.9 spans, including complete native/frontend checks
and focused source-free package execution. SDK 2.33 remains the accepted archive.

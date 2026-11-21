# Automatic cleanup callbacks in published bodies

Source 2.155 / System 2.81 publishes native Objective-C 3 automatic cleanup
callbacks and C++ cleanup attributes. Canonical bodies retain the selected
callback and its invocation by value, reference or address. The body identity
binds its declaration, function type and native invocation mode. Dependency
checking rejects callbacks excluded by the export policy.

Canonical printing qualifies callback names and preserves explicit template
arguments. A diagnostic scope suppresses the GCC compatibility warning caused
by the generated qualified spelling; the user's source diagnostics remain
unchanged. Template bodies with concrete local storage preserve the same
cleanup contract. The [Source 2.156 span](linux_dependent_automatic_cleanup.md) subsequently adds
dependent and deduced storage with concrete instantiation-time revalidation.

AST import now imports a C++ cleanup attribute's callback declaration and
argument location, rather than cloning pointers into the source AST. This
repairs source-free template instantiation reporting two identical printed
pointer types as incompatible, and prevents the callback from depending on
source-AST lifetime. A compiler unit control checks the destination context,
parameter type and imported source location after source-AST destruction.

The `objc3.native-storage-lifetime.v2` hard reader cutover requires rebuilding
interfaces. Encoders and decoders reject namespace-owned storage/proof claims.
Runtime service, frame and extension ABI layouts are unchanged.

Six publication profiles check canonical replay, relocation, changed-callback
fingerprints, private-dependency rejection, consumer instantiation and older
reader rejection. Four native source-free provider/two-consumer configurations
check value/reference/address callbacks, callback-before-destructor order,
normal and exceptional exits, original cleanup exceptions, failed construction
without cleanup, and template instantiation. Decoder/isolation controls also
pass. These are focused checks with changed production compiler translation
units; complete rebuilt compiler/SDK acceptance remains pending.

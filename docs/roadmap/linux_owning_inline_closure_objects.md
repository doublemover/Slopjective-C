# Owning inline closure objects

Source 2.172 / System 2.98 extends source-free C++ inline closure-object publication
under `objc3.cxx-inline-closure-objects.v2`. Version 1 readers are rejected;
extension ABI 24 and runtime layouts stay unchanged.

An externally visible inline namespace object or static data member of a concrete
published class binds its closure type to the stable variable owner. Owning
init-captures retain the direct lambda through native temporary/cleanup wrappers;
an arbitrary call or constructor does not become a lambda definition. Copies of
the closure retain the original type owner. Aliases, references, mutable/generic
operators, TLS storage and native lifetime policies retain their physical C++
semantics. Dependent class/variable-template contexts continue to use their
existing template publication contracts; non-inline or excluded closure owners
remain outside this admission.

The expanded controls publish copyable and move-only owners, generic/mutable
state, namespace/class-static/TLS objects, copies, aliases and reference results.
They remove provider source before compiling two consumers. Native checks verify
successful copy/move counts, failed-copy suppression, partial capture destruction,
original generic exceptions, persistent state, global/TLS address identity and
exact complete shutdown destruction. Explicit `always_destroy` policies are
checked with ordinary and suppressed implicit static destruction.

Import controls destroy the producer AST before print/reparse and preserve the
actual native capture shape and variable mangling contexts. Publication controls
check canonical replay, source relocation, changed captured values, required
capability admission, excluded dependencies and unsupported owner forms. Full
SDK, vendor/profile, instrumentation and native-host acceptance remain separate
consolidation gates.

The focused checkpoint passes all 88 maintained LLVM AST infrastructure tests,
six owning-closure publication configurations and six adjacent borrowed-result
publication profiles, four source-free native storage configurations, decoder
controls and 25 related tooling checks. Every native configuration verifies six
live persistent owners after TLS initialization, exact successful copy/move and
failure cleanup counts, shared global/TLS identity and zero owners at shutdown.
Maintained patch application reproduces all changed compiler/control files and
all manifest digests match. These are focused source checks against existing SDK
libraries, not acceptance of a newly sealed SDK archive.

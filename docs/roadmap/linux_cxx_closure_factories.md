# Published C++ closure factories on Linux

Source **2.78** extends complete deduced closure publication to ordinary and generic
C++ call operators. A factory must publish its complete checked definition, use an
`auto` or `decltype(auto)` result and return a closure owned by that same callable.
The closure body, template head, captures and referenced declarations remain part
of the published owner contract. Unresolved and private dependencies stay rejected.

Native replay preserves authored deduction syntax and reconstructs the closure
under its factory. Canonical result identity excludes physical file locations;
body changes still change the semantic fingerprint. The capability
`objc3.cxx-closure-factories.v1` follows complete callable/record owners. Missing
capabilities and namespace-only owners are rejected with `O3IFC087`.

Implementation witnesses cover concrete and generic factories, mutable and
reference captures, nested returned closures and owned record captures in ordinary
and member factories. The [focused source receipt](../support/linux_cxx_closure_factories_progress.json)
records the optimized emitter suite, 84 tooling tests and four representative
source-free configurations / 24 commands passing. Full Cartesian and sanitizer
matrices remain batched. This source work does not rebuild SDK 2.62 or establish full profile
conformance, arbitrary public unnamed types or native Linux AArch64 qualification.

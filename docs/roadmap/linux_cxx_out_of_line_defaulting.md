# C++ out-of-line defaulted special members

The concurrency 2.18.0 source milestone publishes out-of-line defaulted special
members under required `objc3.cxx-out-of-line-defaulting.v1` semantics.
Constructors, copy/move assignment and destructors retain native user-provided,
inline, exception, access and lifecycle state. Containing records independently
bind their defaulted member set.

Canonical source preserves the original in-class declarations followed by the
out-of-line defaulted definitions in native declaration order and namespace
scope. Definitions inherit module-owned labels from their declarations.
Out-of-line bodies belong to the provider; consumer backend registration remains
limited to inline definitions. This preserves native calling/destruction ABI and
avoids manufacturing a second strong definition in consumers.

Source fixtures cover namespace-scoped default construction, copy/move
construction and assignment, destruction and a pure virtual destructor.
Source-free execution checks native type traits, calls through provider-owned
structor variants, and nontrivial class values passed and returned by a native
async frame.

Consteval defaulted special members, dependent/templates and broader
ownership/profile contracts retain their explicit boundaries. The accepted SDK
remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad
source/sanitizer matrices and installed/archive delivery remain deferred for the
combined source batch.

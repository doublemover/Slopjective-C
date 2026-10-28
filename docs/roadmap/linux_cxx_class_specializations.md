# Explicit namespace class-template specializations

Source 2.59 publishes explicit namespace class-template specializations under
their primary template's interface and native ABI owner. Complete definitions,
exact argument keys, private storage, inline member patterns and initializers
participate in the primary contract. Incidental instantiations add no contract.

Canonical replay preserves forward declarations and original declaration order,
including public dependencies introduced between a primary and its explicit
specialization. Qualified namespace definitions retain valid namespace wrappers.
Consumer import brings in all checked explicit definitions before comparing the
primary contract or instantiating consumer code.

The required `objc3.cxx-class-specializations.v1` capability depends on the
primary class-template capability. Unsupported consumers, inconsistent metadata,
private dependencies, opaque specializations and detached bodies reject. Partial
specializations, nested/member specialization forms and effectful definitions
remain separate work.

Maintained witnesses cover source/reprint/relocation, changed bodies, capability
rejection, namespace qualification, forward declarations, exact signed-zero
arguments, member-template calls, source-free virtual copy/delete lifetimes and
shared specialization state. Capability discovery includes features used only
inside explicit definitions. Native import retains lexical order even when the
specialization contains no member templates. The [focused receipt](../support/linux_cxx_class_specializations_progress.json)
records six native import tests in C++20/23, the emitter suite, 19 tooling tests
and 16 source-free configurations / 80 commands across source-directed and eager
imports passing. Combined validation and SDK delivery remain batched;
SDK 2.52 is the accepted archive. Full profile and native Linux AArch64
qualification remain open.

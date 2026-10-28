# Namespace function-template specializations

Source 2.57 publishes explicit namespace function-template specializations with
exact argument identities, including floating-point signed zero. Each owned
specialization has an independent module/package symbol; primary template
instantiations retain native template mangling. Inline and constexpr definitions
travel in the interface, while ordinary definitions link to their provider.

The required `objc3.cxx-function-specializations.v1` capability depends on the
primary function-template capability. Inconsistent metadata and old consumers
reject. Specializations participate in dependency checking and fingerprinting;
private helper dependencies remain rejected. Imported specializations belong to
their primary template's specialization set and do not become ordinary overloads.

Maintained witnesses cover source/reprint/relocation, changed inline bodies,
capability rejection, specialized function pointers, constexpr selection,
provider-only definitions and shared inline static state. Source-free C++20/23
consumers exercise ARC/MRC and O0/O2 after provider source removal, including
rejection of an undeduced address of the primary template. The [focused receipt](../support/linux_cxx_function_specializations_progress.json)
records the emitter suite, seven tooling tests and all eight source-free
configurations / 40 commands passing, including eight lookup rejections.

Class, variable and member-template specializations, partial specializations,
detached member definitions and effectful template bodies remain separate work.
Combined validation and SDK delivery remain batched; SDK 2.52 is the accepted
archive. Full profile and native Linux AArch64 qualification remain open.

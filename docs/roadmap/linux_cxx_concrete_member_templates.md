# Concrete-class member templates

Source 2.54 publishes in-class function and alias templates, and inline member
variable templates, in public concrete C++ classes. The nearest concrete record
owns its patterns; ordinary constructors, methods, virtual dispatch and storage
keep their existing declaration rows. Member patterns and instantiated descendants
do not acquire independent interface rows or concrete specialization asm labels.

Complete pattern identities cover access, heads, defaults, constraints, bodies,
deduction and shared or thread-local initialization. Recursive dependency checks
apply before publication. Module-owned function patterns retain their native ABI
tags and the enclosing class tag. Alias and variable substitutions use the same
record owner during dependency checking and native import.

`objc3.cxx-concrete-member-templates.v1` is required with applicable member,
alias, variable, deduction and thread-local capabilities. Old consumers and
inconsistent contracts reject. Detached definitions, explicit/partial
specializations, template-template parameters, nested primary templates in
concrete classes and effectful template bodies remain separate work.

Maintained witnesses cover converting constructors, constrained call operators,
conditional noexcept, deduced results, closures, private storage aliases, public
rebindings, arrays, shared specialization-local state, variable templates and
native virtual copy/delete paths after provider source removal. The [focused source receipt](../support/linux_cxx_concrete_member_templates_progress.json)
records the optimized build, both native import tests in C++20/23, the emitter
suite and all eight source-free configurations / 32 commands passing. A worker
checks fresh TLS while shared state remains common and parent TLS stays unchanged.
Native replay preserves alias/member order and omits implicit variable
specializations; TLS capability discovery reads canonical patterns. Broader
validation and SDK delivery remain batched.
SDK 2.52 is accepted and predates this span. Full profile and native AArch64
qualification remain open.

# Nested class-template types and variables

The 2.52 source span publishes named nested records and primary class templates,
and inline member variable templates, inside a public primary class template.
Nested defaults, constraints, private storage, ordinary/member-template methods,
constructors/destructors, aliases, virtual dispatch and RTTI use native consumer
instantiation. Variable members retain constexpr substitution and shared mutable
or thread-local specialization storage.

The outermost exported primary owns the complete definition, export/SPI policy
and ABI. Nested patterns do not create independent export rows or concrete asm
labels. Recursive dependency checks cover heads, bases, storage initializers,
method defaults/constraints and bodies. Typed references bind enclosing class
and function arguments, preserving exact structural/floating argument identity.
Native import retains complete dependent-record declaration order, including
ordinary records nested inside templates.

`objc3.cxx-nested-templates.v1` is required with the outer class capability.
Nested member function, alias, variable, deduction and thread-local capabilities
remain required when their declarations occur at any nested level. Unknown or
inconsistent capability contracts reject. Detached definitions, explicit/partial
specializations, unnamed/union nested records, non-inline member variable
templates, template-template parameters and effectful forms remain gated.

Source/reprint/relocation and changed-body checks cover complete ownership and
admission. Exact outer floating arguments distinguish signed zero. Source-free
C++20/23 consumers create new nested specializations, copy/delete provider-created
objects through native virtual entries and check complete destruction, shared
member-variable state, independent specialization storage and per-thread
initialization/isolation on a worker. Focused checks are
recorded when complete; broader validation and coordinated SDK delivery remain
batched. SDK 2.33 remains accepted. Full profile and native AArch64 qualification
remain open.

The [combined source receipt](../support/linux_template_member_batch_progress.json)
records 410 compiler regressions, 201 native unit tests, all 33 optimized frontend
suites and 335 tooling tests. Normal, ASan/UBSan and TSan each pass all 168
five-family source/PCH configurations / 1,158 commands. Source-free C++ and
key-path packages pass 80 configurations / 320 commands across normal, eager,
ASan/UBSan and TSan execution. These results supersede the deferred combined
source checks above. The [SDK 2.52 acceptance receipt](../support/linux_source_batch_2_52_acceptance.json)
records completed installed delivery and exact-archive qualification on fresh
Debian 13 and Ubuntu 26.04. Full profile and native AArch64 work remain open.

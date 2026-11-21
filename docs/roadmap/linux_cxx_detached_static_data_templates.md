# Detached static-data templates on Linux

Source **2.64** adds authored out-of-line static data of primary and partial
class templates, plus member variable templates of generic and concrete records.
Complete class owners retain the definition set; detached data introduces no
independent export rows or concrete specialization assembly labels.

Native replay preserves enclosing template heads, record qualifiers, inline and
thread-local storage, initialized definitions and zero-initialized definitions.
The importer retains outer heads and reconnects each definition through its
mapped source redeclaration. Owner identities bind exact variable types, storage
contracts and typed initializers before consumer instantiation. Initializers must
have public dependencies and module-owned definitions.

The capability `objc3.cxx-detached-static-data-templates.v1` is required whenever
an owner publishes detached static data. Missing or inconsistent capability
metadata is rejected with `O3IFC073`. Source-free package witnesses exercise
shared addresses, a record without a default constructor initialized once,
non-type arguments, mutable storage, independent worker TLS, partial-template
lifetime counters and fresh consumer-created specializations across C++20/23,
ARC/MRC, O0/O2 and source-directed/eager import.

The [focused source receipt](../support/linux_cxx_detached_static_data_templates_progress.json)
records the emitter suite, ten native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Combined
compiler/frontend/sanitizer validation follows further implementation.
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted
and predates this source extension. Detached nested-class definitions,
nested/member template specializations, remaining effectful boundaries, full
profile conformance and native Linux AArch64 qualification remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.

# Partial namespace variable-template specializations

Source 2.61 publishes namespace partial variable-template specializations under
the primary template's interface and native ABI owner. Contracts bind complete
heads, constraints, dependent argument patterns, types and initializers. Mutable,
constexpr, deduced and thread-local patterns support native consumer selection.
Generic non-inline definitions retain native template storage sharing.

The required `objc3.cxx-variable-partial-specializations.v1` capability depends
on the primary variable-template capability. Private dependencies, opaque partials,
static private storage, unmodeled attributes and inconsistent metadata reject.
Partial patterns stay outside ordinary lookup and backend definition delivery;
consumer instantiation produces concrete storage. Native declaration replay
preserves partial heads, arguments and storage flags.

Maintained witnesses cover source/reprint/relocation, constrained pointer patterns,
value expressions, packs, template-template deduction, changed initializers,
capability rejection, shared mutable storage and independent worker TLS. Symbol
checks verify primary ABI ownership on emitted mutable/TLS specializations.
Nested/member partials, non-inline explicit variable specializations, detached
and effectful forms remain separate work. Combined validation and SDK delivery
remain batched; SDK 2.52 remains the accepted archive. Full profile and native
Linux AArch64 qualification remain open. The [focused receipt](../support/linux_cxx_partial_variable_specializations_progress.json)
records eight native import tests in C++20/23, verified partial-table membership,
the emitter suite, 19 tooling tests and 16 source-free configurations / 96 commands
passing, including 16 primary ABI symbol inspections.

The [combined 2.53–2.62 source receipt](../support/linux_template_publication_batch_progress.json)
supersedes the deferred source checks above: 411 compiler regressions, 208 native
unit tests, 33 optimized frontend suites and the maintained 335 tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending.

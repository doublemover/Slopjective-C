# Effectful namespace function templates on Linux

Source **2.73** publishes namespace function templates with canonical native async,
frame and checked-error effects. Complete heads, exact native function types,
checked definitions and runtime/header dependencies remain under the primary
function-template owner. Consumers instantiate the imported native definitions.

The capability `objc3.cxx-effectful-function-templates.v1` requires matching
function-template identities. Missing or inconsistent metadata is rejected with
`O3IFC082`. Native runtime/header identity checks apply to published bodies.

The [focused source receipt](../support/linux_cxx_effectful_function_templates_progress.json)
records the emitter suite, twenty focused native importer tests, all thirty-four
tests in that import family, alias ABI code generation, 72 tooling tests and
16 source-free configurations / 96 commands passing. Combined
compiler/frontend/sanitizer validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Effectful member/specialization and dependent-error projections, remaining generic
contracts, full profile conformance and native Linux AArch64 remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.

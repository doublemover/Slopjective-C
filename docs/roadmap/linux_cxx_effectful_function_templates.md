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

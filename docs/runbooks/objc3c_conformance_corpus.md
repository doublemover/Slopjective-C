# ObjC 3 Conformance Corpus

This runbook defines the checked-in corpus foundation used for `M301`.

## Source Of Truth

- corpus contract: `tests/conformance/corpus_surface.json`
- suite readme: `tests/conformance/README.md`
- family traceability map: `tests/conformance/COVERAGE_MAP.md`
- primary suite manifests:
  - `tests/conformance/parser/manifest.json`
  - `tests/conformance/semantic/manifest.json`
  - `tests/conformance/lowering_abi/manifest.json`
  - `tests/conformance/module_roundtrip/manifest.json`
  - `tests/conformance/diagnostics/manifest.json`

## Taxonomy

Primary buckets:

- `parser`: grammar acceptance, rejection, and ambiguity coverage
- `semantic`: typing, effects, isolation, ownership, and behavior coverage
- `lowering_abi`: lowering, runtime bridge, and ABI boundary coverage
- `module_roundtrip`: textual-interface emit/import and metadata preservation coverage
- `diagnostics`: required diagnostics and fix-it behavior coverage

Supplemental buckets:

- `examples`: tutorial and showcase parity coverage
- `spec_open_issues`: unresolved-spec accounting retained with the corpus
- `workpacks`: historical planning shards retained for traceability only

## Coverage Gap Model

Gap axes:

- bucket minimum shortfall
- required family missing from the checked-in manifests
- profile-only requirement missing
- executable replay nondeterminism
- runtime packaging path not covered
- example or tutorial path not covered

Gap states:

- `covered`
- `partial`
- `missing`
- `known-open`

Claim boundary:

- release-facing corpus claims must resolve back to checked-in manifests, the coverage map, and deterministic executable evidence

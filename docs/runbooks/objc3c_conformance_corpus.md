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

## Coverage Claim Policy

Allowed claim levels:

- `bucket-covered`
- `profile-covered`
- `release-claimable`

Publishable claims require all of the following at once:

- bucket minima pass
- family traceability is present in the coverage map
- executable determinism gates pass where the suite is executable

Forbidden claim patterns:

- count-only coverage claims with no family traceability
- profile claims that ignore strict-system or strict-concurrency deltas
- release claims backed only by prose or dashboard summaries

Fail-closed rule:

- any unresolved `missing`, `partial`, or `known-open` gap blocks `release-claimable` status for the affected profile or surface

## Audited Surface

Runtime and lowering coverage already resolves through:

- `tests/conformance/lowering_abi/manifest.json`
- `scripts/check_objc3c_execution_replay_proof.ps1`
- `scripts/check_objc3c_lowering_replay_proof.ps1`

Module and metadata coverage already resolves through:

- `tests/conformance/module_roundtrip/manifest.json`
- `scripts/generate_conformance_evidence_index.py`

Interop coverage already resolves through:

- `tests/conformance/semantic/manifest.json`
- `tests/conformance/module_roundtrip/manifest.json`
- `scripts/check_objc3c_runnable_interop_conformance.py`

Example and tutorial coverage already resolves through:

- `tests/conformance/examples/manifest.json`
- `showcase/portfolio.json`
- `docs/tutorials/README.md`

Packaging and release evidence already resolves through:

- `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
- `schemas/objc3-conformance-dashboard-status-v1.schema.json`
- `scripts/check_release_evidence.py`

Remaining corpus gaps are expected to terminate in:

- `tests/conformance/spec_open_issues/manifest.json`

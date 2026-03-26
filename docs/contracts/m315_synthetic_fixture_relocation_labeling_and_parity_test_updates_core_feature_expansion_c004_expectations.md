# M315-C004 Expectations

Contract ID: `objc3c-cleanup-synthetic-fixture-labeling-parity-updates/m315-c004-v1`

## Outcome
- keep the committed `library_cli_parity` fixture root in place;
- promote the fixture from implied synthetic status to explicit artifact-authenticity labeling;
- update the shared parity tool so partially labeled synthetic fixture surfaces fail closed.

## Required truths
- the committed parity fixture `.ll` files carry the synthetic-fixture authenticity header keys required by `M315-C002`;
- the committed parity fixture manifests and golden summary carry a top-level `artifact_authenticity` envelope;
- the shared parity tool emits `artifact_authenticity`, `synthetic_fixture_contract`, and `authenticity_checks` when validating the synthetic parity fixture;
- a tampered copy of the parity fixture with a missing authenticity envelope fails closed with a synthetic-fixture authenticity mismatch;
- the `M315-A003` inventory and `M315-B004` registry remain truthful after the labeling pass.

## Design notes
- Physical relocation is intentionally not performed in this issue. The root remains `tests/tooling/fixtures/native/library_cli_parity/`.
- The cleanup action is semantic relocation: synthetic status moves from implication-by-path to explicit artifact labeling.
- The parity fixture remains non-proof-eligible synthetic data.

## Exit evidence
- issue-local checker summary at `tmp/reports/m315/M315-C004/synthetic_fixture_labeling_summary.json`
- committed golden summary updated to the new explicit authenticity shape
- shared parity integration coverage in `tests/tooling/test_objc3c_library_cli_parity.py`

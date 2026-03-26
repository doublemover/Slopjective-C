# M315-D002 Expectations

Contract ID: `objc3c.cleanup.anti-noise.enforcement/m315-d002-v1`

## Outcome
- eliminate the two D001 zero-target residue classes from live product-code surfaces;
- promote the stable `advanced_integration_closeout_signoff` identifier family into the final-readiness gate product surface;
- enforce the resulting state with a live anti-noise checker and CI workflow.

## Required truths
- `native/objc3c/src/lower/objc3_lowering_contract.h` and `native/objc3c/src/lower/objc3_lowering_contract.cpp` no longer carry milestone-coded transitional source-model literals;
- `native/objc3c/src/pipeline/objc3_frontend_types.h` and `native/objc3c/src/pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h` no longer carry `m248_integration_closeout_signoff` identifiers or keys;
- the stable replacement family is `advanced_integration_closeout_signoff`;
- the live guard fails closed on:
  - tracked `compiler/objc3c/*.py` reintroduction;
  - unlabeled or partially labeled `tests/tooling/fixtures/native/library_cli_parity` synthetic artifacts;
  - new milestone-coded residue in the D002 target product-code files.

## Scope
- targeted product-code files under `native/objc3c/src/lower/` and `native/objc3c/src/pipeline/`;
- synthetic parity fixture root `tests/tooling/fixtures/native/library_cli_parity`;
- CI workflow surface `.github/workflows/m315-source-hygiene-proof-policy-enforcement.yml`.

## Exit evidence
- issue-local checker summary at `tmp/reports/m315/M315-D002/anti_noise_enforcement_summary.json`;
- updated B005 native-source sweep result with only quarantined residual classes remaining;
- live workflow surface that executes the D002 checker.
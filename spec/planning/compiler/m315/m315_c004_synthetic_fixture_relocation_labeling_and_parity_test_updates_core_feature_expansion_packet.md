# M315-C004 Planning Packet

Issue: `M315-C004`
Title: `Synthetic fixture relocation labeling and parity-test updates`
Contract ID: `objc3c-cleanup-synthetic-fixture-labeling-parity-updates/m315-c004-v1`

## Problem
`library_cli_parity` is the only committed synthetic `.ll` parity root, but its synthetic status is still mostly inferred from path and a loose header marker. That is not strong enough for the artifact-authenticity model introduced in `M315-C002`.

## Decision
- retain the filesystem root;
- add explicit synthetic-fixture authenticity envelopes to the committed manifests and golden summary;
- add required synthetic-fixture authenticity header keys to the committed `.ll` files;
- update the shared parity script to detect and enforce the synthetic-fixture contract;
- refresh the `M315-A003` inventory and `M315-B004` registry so the labeling state is represented truthfully.

## Required implementation surfaces
- `tests/tooling/fixtures/native/library_cli_parity/**`
- `scripts/check_objc3c_library_cli_parity.py`
- `tests/tooling/test_objc3c_library_cli_parity.py`
- `spec/planning/compiler/m315/m315_a003_*_inventory.json`
- `scripts/check_m315_a003_*`
- `spec/planning/compiler/m315/m315_b004_*_registry.json`
- `scripts/check_m315_b004_*`

## Acceptance proof
- committed fixture passes the shared parity checker with `--check-golden`;
- tampered fixture copy fails closed on missing `artifact_authenticity`;
- `M315-A003` and `M315-B004` checkers still pass after the relabeling pass.

Next issue: `M315-D001`.

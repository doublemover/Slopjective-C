# objc3c ABI Governance

Issue `#8173` owns the checked-in ABI governance source of truth.

Canonical inputs:

- `tests/tooling/fixtures/abi_governance/source_of_truth_manifest.json`
- `schemas/objc3c-abi-governance-manifest-v1.schema.json`
- `scripts/check_objc3c_abi_governance.py`
- `tests/tooling/fixtures/release_foundation/abi_api_governance.json`

Replayable gate:

- `python scripts/check_objc3c_abi_governance.py`

The gate is fail-closed. It blocks missing source-of-truth files, generated
`tmp/` or `temp/` paths in governed inputs, ABI identity drift, lost
release-blocking transitions, missing `#8173` ownership, and any claim that
compatibility shims or fallback downgrade routes are supported.

Current ABI claim boundary:

- exact checked ABI surface only
- same-major checked-manifest governance only where the manifest names the
  surface and transition set
- no cross-major forward compatibility claim
- no drop-in legacy ABI compatibility claim
- no indefinite support-window claim
- no compatibility shim support
- no fallback downgrade route

Generated reports remain outputs only:

- `tmp/reports/abi-governance/abi-governance-summary.json`

Do not commit generated reports. Release or package publication consumes the
checked-in manifest and reruns the gate instead of treating a prior generated
summary as source truth.

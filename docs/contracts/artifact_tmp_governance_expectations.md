# Artifact Tmp-Path Governance Expectations (M143)

Contract ID: `objc3c-artifact-tmp-governance-contract/m143-v1`

## Scope

M143 standardizes deterministic artifact generation and replay-safe scratch handling under `tmp/` for CLI and C API parity workflows.

## Required Contract Surface

| Check ID | Requirement |
| --- | --- |
| `M143-TMP-01` | Native CLI default output directory is `tmp/artifacts/compilation/objc3c-native` and wrapper defaults align with it. |
| `M143-TMP-02` | C API parity runner default output directory is `tmp/artifacts/compilation/objc3c-native`. |
| `M143-TMP-03` | `scripts/check_objc3c_library_cli_parity.py` enforces tmp-governed source-mode work dirs by default, validates deterministic `--work-key`, and fail-closes on stale generated outputs. |
| `M143-TMP-04` | `package.json` wires `check:objc3c:library-cli-parity:source:m143`, `test:objc3c:m143-artifact-governance`, and `check:compiler-closeout:m143`; `check:task-hygiene` includes `check:compiler-closeout:m143`. |
| `M143-TMP-05` | `docs/objc3c-native` source fragments (`10-cli`, `30-semantics`, `50-artifacts`, `60-tests`) document tmp-governed defaults and deterministic work-key parity replay behavior. |

## Verification Commands

- `python scripts/check_m143_artifact_tmp_governance_contract.py`
- `npm run test:objc3c:m143-artifact-governance`
- `npm run check:compiler-closeout:m143`

## Drift Remediation

1. Restore M143 tmp-governance snippets in source/docs/package surfaces.
2. Re-run `python scripts/check_m143_artifact_tmp_governance_contract.py`.
3. Re-run `npm run check:compiler-closeout:m143`.

# Vendor Extension Conformance Claim Template (`C-08`)

Use this template to publish vendor conformance declarations for extension capability claims.

## 0. Declaration metadata block

claim.declaration_id:
claim.issue_ref:
claim.vendor_name:
claim.vendor_id:
claim.extension_id:
claim.extension_version:
claim.lifecycle_state: experimental
claim.claim_scope: vendor-only
claim.claim_status: draft
claim.created_date: YYYY-MM-DD
claim.last_updated: YYYY-MM-DD
claim.authors: []
claim.review_board_target:

## 1. Capability claims summary

capabilities.claimed_ids: []
capabilities.claim_intent: implements
capabilities.profile_matrix:

| Profile | Claim state | Notes |
| --- | --- | --- |
| core | | |
| strict | | |
| strict-concurrency | | |
| strict-system | | |

capabilities.enablement_mode: default-off
capabilities.non_portable_notes:

## 2. Capability evidence bundle

| capability_id | capability_version | spec_reference | implementation_refs | conformance_mode_notes | limitations | negative_case_refs |
| --- | --- | --- | --- | --- | --- | --- |
| | | | | | | |

## 3. Test evidence bundle

test_evidence.required_test_ids:
- test_id:

test_evidence.results:

| test_id | outcome | run_date | environment_fingerprint | log_ref |
| --- | --- | --- | --- | --- |
| | | YYYY-MM-DD | | |

test_evidence.interop_matrix:

| implementation_a | implementation_b | profile | outcome | evidence_ref |
| --- | --- | --- | --- | --- |
| | | | | |

test_evidence.waiver_refs: []

## 4. Provenance and integrity bundle

prov.source_commit:
prov.build_invocation:
prov.builder_identity:
prov.build_timestamp_utc:
prov.artifact_digests:

| artifact_path | digest_alg | digest_value |
| --- | --- | --- |
| | sha256 | |

prov.signature_bundle:
prov.dependency_lock_refs:

- ref:
prov.repro_window_days:
prov.provenance_exceptions:

## 5. Lifecycle and transition linkage

trace.transition_record_id:
trace.current_state:
trace.target_state:
trace.transition_id:
trace.gate_results:

| gate_id | status | evidence_ref |
| --- | --- | --- |
| VG-01 | | |
| VG-02 | | |
| VG-03 | | |
| VG-04 | | |
| VG-05 | | |
| VG-06 | | |

trace.board_vote_record:
trace.effective_release:
trace.effective_date:
trace.rollback_conditions:
trace.supersedes_claim_ids: []

## 6. Review gate attestation

| gate_id | gate_name | status | blocker_refs | reviewer |
| --- | --- | --- | --- | --- |
| VG-01 | Completeness gate | | | |
| VG-02 | Capability namespace gate | | | |
| VG-03 | Test sufficiency gate | | | |
| VG-04 | Provenance integrity gate | | | |
| VG-05 | Lifecycle consistency gate | | | |
| VG-06 | Governance decision gate | | | |

## 7. Exceptions and limitations

- exception_id:
  threshold_or_gate:
  severity:
  rationale:
  approved_by:
  expires_at_utc:
  status:

## 8. Validation record

validation.generated_at_utc:
validation.validated_by:
validation.command_refs:
- python scripts/spec_lint.py
validation.results:

| command | output_summary | exit_code |
| --- | --- | --- |
| | | |

## 9. Change log and supersession

| change_id | date | summary | supersedes |
| --- | --- | --- | --- |
| | YYYY-MM-DD | | |

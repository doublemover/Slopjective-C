# objc3c Security Hardening

## Working Boundary

This runbook defines the live security-hardening surface for objc3c.

Use it when changing:

- macro/package/provenance trust semantics
- release-manifest, SBOM, attestation, installer, and update trust boundaries
- disclosure, advisory, and incident-response publication
- runtime hardening and memory-safety regression claims

Downstream security work must stay on the existing compiler, package, release,
and runtime paths below instead of creating a second security-only pipeline.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/security_hardening/boundary_inventory.json`
- `tests/tooling/fixtures/release_foundation/provenance_policy.json`
- `tests/tooling/fixtures/release_operations/compatibility_claim_policy.json`
- `tests/tooling/fixtures/distribution_credibility/operator_release_policy.json`
- `tests/tooling/fixtures/external_validation/trust_policy.json`

Replayable generators and validators:

- `python scripts/build_security_hardening_boundary_inventory_summary.py`
- `python scripts/check_release_evidence.py`
- `python scripts/check_source_hygiene_authenticity.py`
- `python scripts/check_objc3c_distribution_credibility_integration.py`
- `python scripts/check_objc3c_distribution_credibility_end_to_end.py`
- `python scripts/check_objc3c_release_operations_integration.py`
- `python scripts/check_objc3c_release_operations_end_to_end.py`
- `python scripts/check_objc3c_platform_hardening_integration.py`
- `python scripts/check_objc3c_runtime_acceptance.py`

## Current Security Posture

The current checked-in security posture is intentionally narrow.

- release trust is machine-derived from:
  - the release manifest
  - the SBOM and attestation publication
  - package-channel install and rollback evidence
  - update-manifest and compatibility publication
  - the release-evidence index
- macro/package/provenance trust is bounded by:
  - source-visible package and provenance markers
  - deterministic macro safety checks in sema and runtime acceptance
  - fail-closed metadata requirements on checked-in fixtures
- runtime hardening is bounded by:
  - existing runtime acceptance and packaged runnable validation
  - no claim of external sandboxing, memory-safe runtime isolation, or hostile
    plugin containment beyond the checked-in acceptance and packaged proofs
- response publication is bounded by:
  - checked-in trust reports, operator policy, and release drill evidence
  - no hosted advisory service, key server, or out-of-band incident portal

Security claims must stay narrower than the evidence:

- no hosted security advisory feed exists today
- no automatic release-key rotation or remote revocation service exists today
- no broad hostile-host or hostile-plugin execution safety claim exists today
- no cross-platform installer trust claim exists outside the checked-in
  `windows-x64` package and update surface

## Trust Boundaries

### Macro, Package, And Provenance Trust

Macro trust currently terminates in the checked-in compiler and runtime surfaces:

- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `scripts/check_objc3c_runtime_acceptance.py`
- `tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3`
- `tests/tooling/fixtures/native/macro_package_provenance_positive.objc3`

Those surfaces enforce:

- explicit macro package markers
- explicit macro provenance markers
- deterministic provenance token rules
- fail-closed rejection when metadata is missing, orphaned, or malformed

They do not currently prove:

- hostile-code sandbox isolation outside the checked-in macro safety contracts
- external package signing or remote provenance verification

### Installer, Update, And Release Trust

Installer and update trust currently terminates in:

- `scripts/build_objc3c_release_manifest.py`
- `scripts/publish_objc3c_release_provenance.py`
- `scripts/build_objc3c_update_manifest.py`
- `scripts/publish_objc3c_release_operations_metadata.py`
- `scripts/build_objc3c_package_channels.py`
- `scripts/package_objc3c_runnable_toolchain.ps1`

Those surfaces prove:

- shipped payload lineage
- package-channel install and rollback coherence
- update-manifest and compatibility publication coherence
- machine-owned evidence linkage back to the shipped package family

They do not currently prove:

- remote key custody
- signed-installer publication
- hosted updater trust

### Runtime Hardening And Memory-Safety Boundaries

Runtime hardening currently terminates in:

- `scripts/check_objc3c_runtime_acceptance.py`
- `scripts/check_objc3c_runnable_release_candidate_end_to_end.py`
- `scripts/check_objc3c_runnable_release_candidate_conformance.py`
- existing runtime/object-model/block-ARC/error/concurrency/metaprogramming validation

That surface is sufficient for checked-in executable regression evidence, but it
is not a general-purpose memory-safety certification claim.

### Disclosure And Response Boundary

Current disclosure and response posture is checked-in and operator-scoped:

- `docs/runbooks/objc3c_distribution_credibility.md`
- `docs/runbooks/objc3c_release_operations.md`
- `tests/tooling/fixtures/distribution_credibility/operator_release_policy.json`
- `tests/tooling/fixtures/external_validation/trust_policy.json`

That surface supports repeatable publication and escalation guidance, but it is
not yet a public advisory program with external intake and hosted response
infrastructure.

## Machine-Owned Security Surface

This milestone must leave behind one machine-owned security posture/reporting
surface derived from the live release, package, update, and runtime evidence.

Generated security posture and advisory outputs must stay under:

- `tmp/reports/security-hardening/`
- `tmp/artifacts/security-hardening/`

Checked-in source-of-truth must stay under:

- `docs/runbooks/`
- `tests/tooling/fixtures/`
- `schemas/`
- `scripts/`

## Working Rules For Downstream Issues

- keep security claims tied to existing release/package/runtime evidence
- prefer extending existing generators and validators over adding new parallel
  workflows
- keep transient captures under `tmp/`
- keep public claims narrower than the evidence
- fail closed when a trust signal, provenance link, or advisory publication path
  drifts

## Non-Goals

- no hosted security portal
- no remote key-management service
- no signed-installer trust claim
- no hostile-host execution safety guarantee
- no second release, package, or runtime validation lane

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
- release operations upgrade-claim policy:
  `tests/tooling/fixtures/release_operations/upgrade_support_claim_policy.json`
  (live contract fields are upgrade/support-scoped)
- `tests/tooling/fixtures/distribution_credibility/operator_release_policy.json`
- `tests/tooling/fixtures/external_validation/trust_policy.json`

Replayable public workflow actions:

- `npm run objc3c -- check-security-hardening-surface`
- `npm run objc3c -- check-security-hardening-schema-surface`
- `npm run objc3c -- check-security-sanitizer-validation`
- `npm run objc3c -- check-security-language-runtime-threat-model`
- `npm run objc3c -- build-security-posture`
- `npm run objc3c -- publish-security-advisories`
- `npm run objc3c -- validate-security-hardening`
- `npm run objc3c -- validate-security-hardening-end-to-end`
- `npm run objc3c -- check-release-evidence`
- `npm run objc3c -- check-source-hygiene-authenticity`
- `npm run objc3c -- validate-distribution-credibility`
- `npm run objc3c -- validate-distribution-credibility-end-to-end`
- `npm run objc3c -- validate-release-operations`
- `npm run objc3c -- validate-release-operations-end-to-end`
- `npm run objc3c -- validate-platform-hardening`
- `npm run objc3c -- test-runtime-acceptance-fast`

Helper implementations are action-registry anchors for those
commands, not a second security command surface.

## Current Security Posture

The current checked-in security posture is intentionally narrow.

- release trust is machine-derived from:
  - the release manifest
  - the SBOM and attestation publication
  - package-channel install and revert evidence
  - update-manifest and support-window publication
  - the release-evidence index
- macro/package/provenance trust is bounded by:
  - source-visible package and provenance markers
  - deterministic macro safety checks in sema and runtime acceptance
  - fail-closed metadata requirements on checked-in fixtures
- runtime hardening is bounded by:
  - existing runtime acceptance and packaged runnable validation
  - ASan/UBSan native runtime and compiler target application contracts
  - a machine-owned runtime-hardening summary derived from those passing reports
  - no claim of external sandboxing, memory-safe runtime isolation, or hostile
    plugin containment beyond the checked-in acceptance and packaged proofs
- language/runtime threat modeling is bounded by:
  - the checked-in threat model and mitigation backlog contract
  - macro supply-chain, runtime hardening, sanitizer, source, and workflow evidence
  - no generated-summary-only closure claim
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

- `native/objc3c/src/sema/`
- `native/objc3c/src/pipeline/`
- `native/objc3c/src/artifacts/`
- `native/objc3c/src/io/objc3_process.cpp`
- `npm run objc3c -- test-runtime-acceptance-fast`
- `tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3`
- `tests/tooling/fixtures/native/macro_package_provenance_positive.objc3`

Those surfaces enforce:

- explicit macro package markers
- explicit macro provenance markers
- deterministic provenance token rules
- fail-closed rejection when metadata is missing, orphaned, or malformed
- host-process and host-cache inputs must remain complete before macro execution
  is treated as runnable

Current macro/package/provenance trust semantics:

- `objc_macro` admission is the root capability marker
- `objc_macro_package` and `objc_macro_provenance` are required together on the
  checked-in trusted macro surface
- macro provenance must stay deterministic and lowercase-sha256-shaped on the
  checked-in surface
- metadata-completeness, sandbox-namespace, provenance-determinism, and
  callable-determinism checks all fail closed before the surface is treated as
  ready for lowering and runtime
- package/provenance claims are only as strong as the checked-in compiler,
  runtime acceptance, and packaged runnable evidence; they do not imply remote
  package trust, signature verification, or general untrusted macro execution

They do not currently prove:

- hostile-code sandbox isolation outside the checked-in macro safety contracts
- external package signing or remote provenance verification

### Installer, Update, And Release Trust

Installer and update trust currently terminates in:

- `npm run objc3c -- build-release-manifest`
- `npm run objc3c -- publish-release-provenance`
- `npm run objc3c -- build-update-manifest`
- `npm run objc3c -- publish-release-operations`
- `npm run objc3c -- build-package-channels`
- `npm run objc3c -- package-runnable-toolchain`

Those surfaces prove:

- shipped payload lineage
- package-channel install and revert coherence
- update-manifest and support-window publication coherence
- machine-owned evidence linkage back to the shipped package family

They do not currently prove:

- remote key custody
- signed-installer publication
- hosted updater trust

Current installer/update/release-key hardening semantics:

- release-manifest, SBOM, and attestation publication are the canonical checked-in
  trust anchors for shipped payload lineage
- package channels, install receipts, revert proofs, update manifests,
  support-window reports, and distribution trust reports must all resolve to the
  same runnable package family
- release-key handling is bounded to the local publication environment that
  emits the checked-in attestation and provenance artifacts
- security claims must fail closed if manifest, provenance, package, update, or
  trust-report linkage drifts
- no part of the current surface implies remote key custody, hosted revocation,
  automatic rotation, or signed-installer trust beyond the checked-in artifacts

### Runtime Hardening And Memory-Safety Boundaries

Runtime hardening currently terminates in:

- `npm run objc3c -- test-runtime-acceptance-fast`
- `npm run objc3c -- validate-runnable-release-candidate`
- `npm run objc3c -- validate-release-candidate-conformance`
- `npm run objc3c -- check-security-sanitizer-validation`
- `npm run objc3c -- check-security-language-runtime-threat-model`
- existing runtime/object-model/block-ARC/error/concurrency/metaprogramming validation
- `native/objc3c/cmake/Objc3Sanitizers.cmake` ASan/UBSan target application
- `tests/tooling/fixtures/security_hardening/sanitizer_validation_contract.json`
- `tests/tooling/fixtures/security_hardening/language_runtime_threat_model_backlog.json`

That surface is sufficient for checked-in executable regression evidence, but it
is not a general-purpose memory-safety certification claim. Sanitizer execution
remains platform/toolchain gated; the checked-in gate fails closed unless ASan
and UBSan config, native runtime/compiler target application, workflow action,
fixture, and report surfaces stay coherent.

ASan (#8230) and UBSan (#8231) are modeled as reserved runtime package variants,
not hidden flags on the default release runtime. The package rows remain
fail-closed until package/install/native execution evidence exists for a
supported host, and sanitized packages cannot be published into the default
release channel, installed without the matching sanitizer runtime library,
published from stale package metadata, or mixed with unsanitized runtime
libraries.

### Disclosure And Response Boundary

Current disclosure and response posture is checked-in and operator-scoped:

- `docs/runbooks/objc3c_distribution_credibility.md`
- `docs/runbooks/objc3c_release_operations.md`
- `tests/tooling/fixtures/distribution_credibility/operator_release_policy.json`
- `tests/tooling/fixtures/external_validation/trust_policy.json`

That surface supports repeatable publication and escalation guidance, but it is
not yet a public advisory program with external intake and hosted response
infrastructure.

## Security Response And Disclosure Policy

The checked-in response and disclosure policy for security hardening is intentionally
narrow and fail-closed.

Security response states:

- `ready`:
  - no blocking trust regressions are active
  - current release/package/update/runtime evidence remains coherent
- `degraded`:
  - non-fatal drift exists and publication must carry explicit caution
  - examples: stale trust-report inputs, candidate-only warnings, or incomplete
    but non-blocking publication refresh
- `blocked`:
  - install failure, revert failure, trust drift, evidence gaps, unresolved
    disclosure risk, or other blocking security regressions are active

Current disclosure model:

- checked-in machine-owned trust and security outputs are the canonical public
  surface
- security-sensitive evidence that cannot be published safely remains bounded by
  the external-validation quarantine policy
- unresolved disclosure uncertainty must fail closed rather than being published
  as a partial trust-positive signal

Current incident classes:

- `supply-chain-integrity`
- `revert-or-install-regression`
- `metadata-or-provenance-drift`
- `runtime-hardening-regression`
- `disclosure-or-license-uncertainty`

Operator response rules:

- do not publish a trust-positive or security-positive report while state is
  `blocked`
- rebuild release-operation, credibility, and security publication together
  after metadata drift
- keep advisory and recovery guidance tied to the same checked-in release and
  package evidence
- retain the narrower checked-in claim surface when the evidence does not prove
  a broader public statement

## Machine-Owned Security Surface

This milestone must leave behind one machine-owned security posture/reporting
surface derived from the live release, package, update, and runtime evidence.

Generated security posture and advisory outputs must stay under:

- `tmp/reports/security-hardening/`
- `tmp/artifacts/security-hardening/`

Generated security-hardening artifacts are:

- a machine-owned security posture JSON
- a machine-owned security advisory index JSON
- a machine-owned markdown advisory report derived from the advisory index
- a machine-owned publication summary tying posture and advisory outputs back to the live evidence

Those artifacts must stay validated by checked-in schema and contract surfaces:

- `schemas/objc3c-security-posture-v1.schema.json`
- `schemas/objc3c-security-advisory-index-v1.schema.json`
- `tests/tooling/fixtures/security_hardening/artifact_reporting_contract.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

Security posture and advisory schemas are registry-backed owner surfaces, not
local runbook schema definitions. The runbook must not copy their JSON shape or
promote generated security reports into support claims.

The supply-chain audit summary for security hardening is:

- `tmp/reports/security-hardening/supply-chain-audit-summary.json`

Checked-in security owner inputs must stay under:

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

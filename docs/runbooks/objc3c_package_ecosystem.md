# objc3c Package Ecosystem

## Working Boundary

This runbook defines the live boundary for package management, dependency
resolution, lockfiles, offline mirrors, and registry-facing publication claims.

Use it when changing:

- local package and workspace semantics
- dependency-resolution and lockfile behavior
- package authoring and consumption workflows
- offline mirror and reproducibility evidence
- registry or publication claims layered on the local package model

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/package_ecosystem/boundary_inventory.json`
- `stdlib/workspace.json`
- `stdlib/package_surface.json`
- `stdlib/advanced_helper_package_surface.json`

Replayable public workflow actions:

- `npm run objc3c -- build-package-lock`
- `npm run objc3c -- validate-package-manager-model`
- `npm run objc3c -- validate-package-authoring`
- `npm run objc3c -- validate-package-mirror`
- `npm run objc3c -- validate-package-ecosystem`
- `npm run objc3c -- validate-runnable-package-ecosystem`
- `npm run objc3c -- package-runnable-toolchain`
- `npm run objc3c -- build-package-channels`
- `npm run objc3c -- build-release-manifest`
- `npm run objc3c -- build-update-manifest`

Helper implementations remain action-catalog-owned anchors for
those actions. They are not a second public command surface.

## Current Boundary

The current package ecosystem starts from real checked-in workspace and package
surfaces, not from a hosted registry:

- `stdlib/workspace.json` defines the canonical workspace root, machine-owned
  artifact/report roots, and package staging root.
- `stdlib/package_surface.json` defines the current module import/package
  identity surface.
- `stdlib/advanced_helper_package_surface.json` defines advanced helper modules
  and command surfaces that already flow through the runnable toolchain bundle.
- release, update, package-channel, and application-architecture workflows
  already provide package pressure from real user-shaped workspaces.

That means the package-ecosystem owner surface must build local package semantics first:

- deterministic dependency resolution and lock behavior
- generated package manifests with Objective-C 3.0 language, ABI, digest, and
  trust metadata
- generated package manifests, lock package rows, and local registry rows with
  the same checked-in module graph source-of-truth record
- schema-backed local registry indexes with exact locked version selection,
  dependency digest evidence, and replay commands
- local workspace and package-authoring workflow
- offline mirror semantics and reproducibility evidence
- registry/publication behavior only after the local model is executable

## Claim Boundary

The package ecosystem may claim support only when evidence flows through the
shared `npm run objc3c -- <action>` bridge and the existing runnable package path.

Supported in this boundary:

- local package/workspace contracts
- reproducible lockfile and provenance semantics
- package manifests generated from checked-in stdlib and showcase surfaces
- package authoring against checked-in stdlib, showcase, and canonical app
  surfaces
- offline mirror evidence generated from local package artifacts
- registry metadata as a generated, local, replayable artifact

Not supported in this boundary:

- a hosted package registry service
- network-backed dependency resolution
- system package manager publication
- package manifests that bypass the `npm run objc3c -- <action>` bridge
- a second compiler payload, package layout, or install workflow

## Dependency Resolution And Lock Policy

The canonical dependency policy is checked in at:

- `tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json`
- `tests/tooling/fixtures/package_ecosystem/package_manager_model_contract.json`

Resolution is intentionally local-first:

- package roots are discovered from checked-in workspace and package surfaces
- package identities are canonical module ids plus source paths, not hosted
  registry slugs
- generated package manifests capture package version, source digest,
  Objective-C 3.0 language mode, `objc3-abi-2025Q4`, module graph source
  authority, dependency requirements, trust signature, and revocation state
- locks capture provenance, manifest digests, source digests, selected
  version/source identity, module graph identity, and replay command intent
- local registry indexes capture exact locked target versions, dependency source
  digests, package manifest digests, module graph records, trust signatures, and
  replay commands for every indexed package
- package trust roots are explicit policy records: each root binds issuer,
  signer, key id, signature format/algorithm, validity window, namespace scope,
  compatibility scope, rotation policy, and revocation lists before any package
  operation can treat a signature as valid
- dependency resolution fails closed when a dependency is missing, ambiguous,
  unpinned, provenance-free, missing module graph metadata, backed by unsafe
  package metadata, ABI-incompatible, language-incompatible, revoked, or outside
  the allowed local/mirror roots

The initial lock model does not claim network fetching. Registry names may appear
only as generated metadata layered over local package artifacts until later
evidence proves hosted behavior.

## Local Workspace And Offline Mirror Semantics

The canonical workspace and mirror semantics are checked in at:

- `tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json`

Local package workspaces are materialized from checked-in stdlib, showcase, and
canonical application surfaces. Lockfiles and mirrors are generated outputs:

- lockfiles publish under the package-ecosystem lock output family
- package manifests publish under
  `tmp/artifacts/package-ecosystem/manifests`
- mirror indexes publish under the package-ecosystem mirror output family
- replay and validation summaries publish under the package-ecosystem report family

An offline mirror is a local artifact cache plus an index generated from a
locked package graph. It must not fetch from the network during validation, and
it is invalid if it contains package identities not present in the lock. Each
mirror package row points at a generated cache entry under
`tmp/artifacts/package-ecosystem/mirrors/cache`, carries the source digest,
package manifest digest, language version, ABI identity, trust envelope, and
cache-entry digest, and is restored only with a machine-owned receipt under
`tmp/artifacts/package-ecosystem/offline-install`.

Clean install distribution validation additionally writes a from-nothing install
proof under `tmp/artifacts/package-ecosystem/install-validation`. The proof
manifest and per-package local artifact envelopes are generated artifacts, not
reports: they bind each installed package to its source manifest, installed
manifest, lock manifest digest, and trust signature. The same clean root also
publishes deterministic update and uninstall operation receipts under
`tmp/artifacts/package-ecosystem/install-validation/clean-root/objc3c/receipts`.
Update receipts preserve the locked dependency-first install order; uninstall
receipts reverse that order so consumers are planned for removal before their
dependencies. Release credibility may consume those artifact records, while
`tmp/reports/package-ecosystem` remains a summary-output root and cannot become
release manifest source truth.

## Registry And Publication Semantics

The canonical registry/publication semantics are checked in at:

- `tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json`

Registry behavior is layered on top of the local lock and mirror model:

- `local-index` is supported as a generated artifact over locked package
  metadata, package manifests, ABI identity, language version, and trust
  signatures. It validates against
  `schemas/objc3c-package-local-registry-index-v1.schema.json` and carries
  exact locked version/dependency evidence rather than hosted package claims.
- `offline-mirror` is supported only when derived from the lock graph.
- `offline-restore-receipt` is supported only when every mirror cache entry
  exists and digest-matches the mirror index.
- `publication-metadata` is supported as replayable release/update/package
  channel metadata.
- `hosted-registry` is explicitly deferred until a later milestone proves
  network service behavior, authentication, moderation, revocation, and
  availability semantics.

Any hosted-registry claim before those proofs exist is release-blocking and must
be demoted to generated local metadata.

## Artifact Contract

The package ecosystem artifact contract is checked in at:

- `tests/tooling/fixtures/package_ecosystem/artifact_contract.json`

Schema surfaces:

- `schemas/objc3c-package-manifest-v1.schema.json`
- `schemas/objc3c-package-lock-v1.schema.json`
- `schemas/objc3c-package-offline-mirror-index-v1.schema.json`
- `schemas/objc3c-package-local-registry-index-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

Machine-owned generated outputs stay in package-ecosystem artifact and report
families selected by the checked-in package contracts.

No package ecosystem artifact is claimable unless it can be regenerated from
checked-in contracts and validated through the `npm run objc3c -- <action>` bridge.

## Local Package Authoring Workflow

The local package authoring workflow is checked in at:

- `tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json`

The replayable implementation is:

- `npm run objc3c -- build-package-lock`
- `npm run objc3c -- validate-package-manager-model`
- `npm run objc3c -- validate-package-authoring`

Helper implementations are action-catalog-owned and are not direct package
commands.

The lock generator derives packages from `stdlib/module_inventory.json` and
`showcase/portfolio.json`, emits generated package manifests under
`tmp/artifacts/package-ecosystem/manifests`, emits a deterministic lock under
the package-ecosystem lock output family, and writes generated-output summaries
under the package-ecosystem report family. The package-manager validator checks
manifest count, dependency graph integrity, source digests, ABI/language
requirements, trust key identity, and fail-closed network/hosted-registry
boundaries before authoring claims are accepted.

## Mirror And Registry Evidence

The mirror/registry reproducibility workflow is checked in at:

- `tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json`

The replayable implementation is:

- `npm run objc3c -- validate-package-mirror`

Helper implementations are action-catalog-owned and are not direct package
commands.

The mirror generator consumes the generated lock, writes an offline mirror index,
digest-checked package cache entries, an offline restore receipt, local registry
index, and publication metadata under the package-ecosystem output root. The
local registry index is generated from the same lock graph, validates exact
locked dependency versions, preserves source/manifest digests and trust
signatures, carries replay commands, and refuses to claim hosted registry
support.

## ObjC++ And Swift Metadata Bridge Surfaces

The checked-in source of truth for C, Objective-C, ObjC++, and Swift
metadata bridge semantics is:

- `tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json`

That contract ties positive fixtures, negative fixtures, stable `O3PKG*`
diagnostics, ABI alignment records, foreign type records, mixed-image loader
records, explicit ObjC++/Swift bridge-surface rows, and package execution
entries to package ids already present in the generated lock. The lock, mirror,
local registry, publication metadata, and packaging-channel manifest all consume
that same contract and carry bridge-surface counts. Malformed, unsupported,
conflicting, and unsafe variants are negative fixtures with stable diagnostic
codes and source ranges. Unsupported hosted registry restore, network-resolved
interop metadata, and unchecked ABI alignment remain fail-closed.

## Public Workflow Integration

The repo-scope package ecosystem workflow is:

- `npm run objc3c -- build-package-lock`
- `npm run objc3c -- validate-package-manager-model`
- `npm run objc3c -- validate-package-authoring`
- `npm run objc3c -- validate-package-mirror`
- `npm run objc3c -- validate-package-ecosystem`
- `npm run objc3c -- validate-runnable-package-ecosystem`

`npm run objc3c -- validate-package-ecosystem` composes the package-manager
model, local package authoring workflow, canonical application architecture, and
stdlib program integration surfaces so package claims remain user-shaped instead
of package-only probes. `npm run objc3c -- validate-runnable-package-ecosystem`
stages the runnable toolchain bundle and reruns package-manager, authoring, and
offline mirror validation from the package root.

## Successor Pressure

This milestone feeds later distribution, adoption, and production-readiness work.
Any broader hosted-registry or ecosystem claim must consume the generated package
ecosystem evidence rather than reinterpreting stdlib, release, or package-channel
metadata directly.

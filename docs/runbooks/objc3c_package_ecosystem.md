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
- local workspace and package-authoring workflow
- offline mirror semantics and reproducibility evidence
- registry/publication behavior only after the local model is executable

## Claim Boundary

The package ecosystem may claim support only when evidence flows through the
shared `npm run objc3c -- <action>` bridge and the existing runnable package path.

Supported in this boundary:

- local package/workspace contracts
- reproducible lockfile and provenance semantics
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

Resolution is intentionally local-first:

- package roots are discovered from checked-in workspace and package surfaces
- package identities are canonical module ids plus source paths, not hosted
  registry slugs
- locks capture provenance, digest inputs, selected version/source identity, and
  replay command intent
- dependency resolution fails closed when a dependency is missing, ambiguous,
  unpinned, provenance-free, or outside the allowed local/mirror roots

The initial lock model does not claim network fetching. Registry names may appear
only as generated metadata layered over local package artifacts until later
evidence proves hosted behavior.

## Local Workspace And Offline Mirror Semantics

The canonical workspace and mirror semantics are checked in at:

- `tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json`

Local package workspaces are materialized from checked-in stdlib, showcase, and
canonical application surfaces. Lockfiles and mirrors are generated outputs:

- lockfiles publish under `tmp/artifacts/package-ecosystem/locks/`
- mirror indexes publish under `tmp/artifacts/package-ecosystem/mirrors/`
- replay and validation summaries publish under `tmp/reports/package-ecosystem/`

An offline mirror is a local artifact cache plus an index generated from a
locked package graph. It must not fetch from the network during validation, and
it is invalid if it contains package identities not present in the lock.

## Registry And Publication Semantics

The canonical registry/publication semantics are checked in at:

- `tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json`

Registry behavior is layered on top of the local lock and mirror model:

- `local-index` is supported as a generated artifact over locked package
  metadata.
- `offline-mirror` is supported only when derived from the lock graph.
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

- `schemas/objc3c-package-lock-v1.schema.json`
- `schemas/objc3c-package-offline-mirror-index-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

Machine-owned generated outputs stay under:

- `tmp/artifacts/package-ecosystem/`
- `tmp/reports/package-ecosystem/`

No package ecosystem artifact is claimable unless it can be regenerated from
checked-in contracts and validated through the `npm run objc3c -- <action>` bridge.

## Local Package Authoring Workflow

The local package authoring workflow is checked in at:

- `tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json`

The replayable implementation is:

- `npm run objc3c -- build-package-lock`
- `npm run objc3c -- validate-package-authoring`

Helper implementations are action-catalog-owned and are not direct package
commands.

The lock generator derives packages from `stdlib/module_inventory.json` and
`showcase/portfolio.json`, emits a deterministic lock under
`tmp/artifacts/package-ecosystem/locks/`, and writes a summary under
`tmp/reports/package-ecosystem/`.

## Mirror And Registry Evidence

The mirror/registry reproducibility workflow is checked in at:

- `tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json`

The replayable implementation is:

- `npm run objc3c -- validate-package-mirror`

Helper implementations are action-catalog-owned and are not direct package
commands.

The mirror generator consumes the generated lock, writes an offline mirror index,
local registry index, and publication metadata under the package-ecosystem
output root, and refuses to claim hosted registry support.

## Public Workflow Integration

The repo-scope package ecosystem workflow is:

- `npm run objc3c -- build-package-lock`
- `npm run objc3c -- validate-package-authoring`
- `npm run objc3c -- validate-package-mirror`
- `npm run objc3c -- validate-package-ecosystem`
- `npm run objc3c -- validate-runnable-package-ecosystem`

`npm run objc3c -- validate-package-ecosystem` composes the local package authoring workflow
with the canonical application architecture and stdlib program integration
surfaces so package claims remain user-shaped instead of package-only probes.
`npm run objc3c -- validate-runnable-package-ecosystem` stages the runnable toolchain bundle and
reruns package authoring plus offline mirror validation from the package root.

## Successor Pressure

This milestone feeds later distribution, adoption, and production-readiness work.
Any broader hosted-registry or ecosystem claim must consume the generated package
ecosystem evidence rather than reinterpreting stdlib, release, or package-channel
metadata directly.

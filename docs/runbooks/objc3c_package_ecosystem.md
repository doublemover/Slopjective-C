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
- `scripts/package_objc3c_runnable_toolchain.ps1`
- `scripts/build_objc3c_package_channels.py`
- `scripts/build_objc3c_release_manifest.py`
- `scripts/build_objc3c_update_manifest.py`

Replayable boundary inventory:

- `python scripts/build_package_ecosystem_boundary_inventory_summary.py`
- `python scripts/build_package_ecosystem_dependency_lock_policy_summary.py`

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

That means this milestone must build local package semantics first:

- deterministic dependency resolution and lock behavior
- local workspace and package-authoring workflow
- offline mirror semantics and reproducibility evidence
- registry/publication behavior only after the local model is executable

## Claim Boundary

The package ecosystem may claim support only when evidence flows through the
shared public workflow runner and the existing runnable package path.

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
- manual package manifests that bypass the public workflow runner
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

## Successor Pressure

This milestone feeds later distribution, adoption, and production-readiness work.
Any broader hosted-registry or ecosystem claim must consume the generated package
ecosystem evidence rather than reinterpreting stdlib, release, or package-channel
metadata directly.

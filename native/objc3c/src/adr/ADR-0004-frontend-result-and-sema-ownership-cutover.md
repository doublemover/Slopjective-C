# ADR-0004: Frontend Result and Sema Ownership Cutover

- Status: Accepted
- Date: 2026-05-09
- Deciders: objc3c native maintainers
- Related surfaces: `objc3c.frontend.capi.resultownership.v1`, `objc3c.frontend.artifactpublication.v1`, `objc3c.sema.featureclaims.v1`, `objc3c.sema.passowners.v1`

## Context

The hard-cutover program needs semantic and frontend API owners, not forwarding
units that hide result ownership, artifact publication, or feature-claim
policy behind broad aggregate files.

## Decision

- Keep exported C ABI symbol names stable while moving lifecycle, compile, and
  result ownership behavior into direct C API owners.
- Treat result-owned payload strings as a C API ownership contract, not as a
  wrapper around the C++ result helpers.
- Publish diagnostics, manifest/runtime metadata, and runtime/interop optional
  artifacts through dedicated frontend publication owners.
- Keep sema feature-claim context construction, rejection rules, and diagnostic
  emission in separate sema-owned files.
- Preserve the existing semantic pass entrypoints while assigning new API
  surface ownership around the parts that can be split without rebuilding the
  generated pass shards.

## Consequences

- Public C names remain ABI-stable for embedders.
- Artifact publication failure handling has one frontend-owned status/error
  path.
- Unsupported feature claims can evolve independently from source-site
  discovery and diagnostic formatting.
- Further semantic pass shard extraction should move real helpers and exported
  builders instead of routing ownership through the aggregate pass file.

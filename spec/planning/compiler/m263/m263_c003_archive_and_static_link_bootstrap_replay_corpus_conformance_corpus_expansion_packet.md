# M263-C003 Archive And Static-Link Bootstrap Replay Corpus Conformance Corpus Expansion Packet

Packet: `M263-C003`

Milestone: `M263`

Lane: `C`

Issue: `#7227`

Dependencies: `M263-C002`, `M263-B003`

## Summary

Expand the lane-C proof corpus so retained archive/static-link bootstrap artifacts are validated through live runtime startup and replay probes, not only through retained-section inspection.

## Acceptance criteria

- publish an explicit replay-corpus contract in lowering, manifest, and IR surfaces
- prove plain archive links omit retained bootstrap images
- prove retained single-archive links register and replay one image
- prove retained merged multi-archive links register and replay multiple images
- prove the native driver can compile distinct retained images with explicit positive bootstrap registration ordinals for deterministic multi-image startup order
- preserve the `M253-D003` merge model and the `M263-B003` replay runtime surface
- land deterministic evidence at `tmp/reports/m263/M263-C003/archive_static_link_bootstrap_replay_corpus_summary.json`
- retain the canonical merged discovery artifact name: `module.merged.runtime-metadata-discovery.json`

## Inputs

- `tests/tooling/fixtures/native/m263_c003_archive_bootstrap_replay_auto.objc3`
- `tests/tooling/fixtures/native/m263_c003_archive_bootstrap_replay_explicit.objc3`
- `tests/tooling/runtime/m263_c003_archive_static_link_bootstrap_replay_probe.cpp`
- `scripts/merge_objc3_runtime_metadata_linker_artifacts.py`

## Outputs

- retained archive/static-link executables for plain, single, and merged cases
- binary-inspection evidence over linked executables
- runtime replay probe evidence over retained executables

## Next issue

`M263-D001`

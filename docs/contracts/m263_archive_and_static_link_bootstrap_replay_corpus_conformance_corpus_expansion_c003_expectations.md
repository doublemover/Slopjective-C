# M263 Archive And Static-Link Bootstrap Replay Corpus Conformance Corpus Expansion Expectations (C003)

Issue: `#7227`

Contract ID: `objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1`

Scope: `M263` lane-C conformance corpus expansion that proves retained archive/static-link bootstrap artifacts participate in live startup registration and deterministic replay.

## Required outcomes

1. The compiler must publish an explicit `M263-C003` replay-corpus contract in the lowering/header, frontend manifest surface, and emitted IR boundary.
2. The corpus must reuse the retained archive/static-link merge path from `M253-D003` rather than inventing a second linker-retention mechanism.
3. The corpus must reuse the live replay/reset runtime APIs from `M263-B003`:
   - `objc3_runtime_replay_registered_images_for_testing`
   - `objc3_runtime_copy_reset_replay_state_for_testing`
4. Happy-path proof must build archive-linked native executables for three states:
   - plain archive link with no retention flags
   - retained single-archive link
   - retained merged multi-archive link
5. Plain archive link must omit retained bootstrap images.
6. Retained single-archive link must register one bootstrap image at startup and replay one retained image after reset.
7. Retained merged multi-archive link must register multiple bootstrap images at startup and replay the same retained image count after reset.
8. The retained multi-archive corpus must assign deterministic positive registration ordinals to each compiled translation unit through the native driver instead of mutating discovery artifacts after compile.
9. Binary inspection must prove retained bootstrap sections survive in the retained links and are absent from the plain link.
10. Evidence must land at `tmp/reports/m263/M263-C003/archive_static_link_bootstrap_replay_corpus_summary.json`.

## Required artifacts

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `tests/tooling/runtime/m263_c003_archive_static_link_bootstrap_replay_probe.cpp`

## Non-goals

- no new bootstrap runtime entrypoints
- no new linker merge algorithm beyond `M253-D003`
- no change to the frozen `M263-C002` registration-descriptor/image-root payload shapes

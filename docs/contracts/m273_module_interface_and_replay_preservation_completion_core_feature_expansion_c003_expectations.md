# M273 Module, Interface, and Replay Preservation Completion Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-part10-module-interface-replay-preservation/m273-c003-v1`

Surface key: `objc_part10_module_interface_and_replay_preservation`

Issue: `#7355`

Expected proof:
- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part10_module_interface_and_replay_preservation`
- provider and consumer `module.runtime-import-surface.json` artifacts preserve the supported Part 10 derive, macro, and property-behavior replay facts across separate compilation
- the consumer manifest preserves imported module/interface replay counts from the provider
- emitted LLVM IR prints a replay-stable `part10_module_interface_replay_preservation` summary comment and metadata node
- evidence lands at `tmp/reports/m273/M273-C003/module_interface_replay_preservation_summary.json`

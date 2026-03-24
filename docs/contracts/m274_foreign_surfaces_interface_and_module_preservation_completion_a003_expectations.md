# M274 Foreign Surfaces Interface And Module Preservation Completion Expectations (A003)

Contract ID: `objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1`

Issue: `#7362`

## Required outcomes

- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_foreign_surface_interface_and_module_preservation`
- provider and consumer `module.runtime-import-surface.json` artifacts preserve the supported Part 11 foreign callable, import-module, and C++/Swift-facing annotation facts across separate compilation
- the consumer runtime-import-surface artifact preserves the imported provider module inventory and imported aggregate Part 11 counts deterministically
- the issue-local evidence stays at the manifest/runtime-import-surface boundary; no new ABI lowering or runnable bridge claim is made here
- evidence lands at `tmp/reports/m274/M274-A003/foreign_surfaces_interface_module_preservation_summary.json`

# M267-D003 Cross-Module Error-Surface Preservation Hardening Edge-Case And Compatibility Completion Packet

Issue: `#7279`
Milestone: `M267`
Lane: `D`
Wave: `W60`

Summary:

Harden the imported runtime-surface and cross-module link-plan path for the current cross-module import slice.

Dependencies:

- `M267-D002`
- `M267-C003`

Implementation targets:

1. Preserve imported runtime-surface state in `Objc3ImportedRuntimeModuleSurface` and the cross-module link-plan imported-input path.
2. Publish imported module preservation state through `module.cross-module-runtime-link-plan.json`.
3. Fail closed on:
   - imported runtime-surface drift
   - incomplete imported runtime-surface readiness
   - duplicate imported module identity or replay keys
4. Keep the runtime helper ABI unchanged; this issue hardens the driver/import/link-plan boundary above `M267-D002`.

Canonical dynamic proof:

- compile a provider fixture that emits a runtime import surface
- compile a consumer fixture against the provider runtime import surface via `--objc3-import-runtime-surface`
- verify the consumer cross-module link plan preserves imported module identity and link inputs
- tamper the imported runtime surface and prove the consumer compile fails closed

Next issue: `M267-E001`

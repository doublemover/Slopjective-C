# M267 Cross-Module Error-Surface Preservation Hardening Edge-Case And Compatibility Completion Expectations (D003)

Contract ID: `objc3c-cross-module-error-surface-preservation-hardening/m267-d003-v1`

This issue hardens the current cross-module runtime import path without widening the runtime helper ABI from `M267-D001`.

Required outcomes:

- provider and consumer builds both emit `module.runtime-import-surface.json`
- the consumer build, when passed `--objc3-import-runtime-surface`, emits `module.cross-module-runtime-link-plan.json`
- the emitted link plan preserves imported module identity, object ordering, and runtime library path information
- cross-module link-plan construction fails closed when the imported runtime surface is incomplete or drifted
- the runtime helper ABI remains private/bootstrap-internal and unchanged in this tranche

Dynamic proof requirements:

- a provider emits `module.runtime-import-surface.json`
- a consumer compiled against the provider import surface via `--objc3-import-runtime-surface` emits `module.cross-module-runtime-link-plan.json`
- the emitted link plan preserves the imported provider module name and replay surface readiness
- a tampered imported runtime surface is rejected during cross-module link-plan construction with a deterministic fail-closed error

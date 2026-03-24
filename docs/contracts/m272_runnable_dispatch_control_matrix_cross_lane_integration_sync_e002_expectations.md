# M272 Runnable Dispatch-Control Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part9-runnable-dispatch-control-matrix/m272-e002-v1`

1. The milestone closeout replays the published `M272-A002` through `M272-E001` proof chain and freezes one explicit runnable matrix for the already-landed Part 9 dispatch-control slice.
2. The closeout remains truthful: it does not widen the supported surface beyond the current direct/final/sealed lowering, metadata, diagnostics, and live runtime fast-path behavior already proved by `M272-D002`.
3. Driver, manifest, and frontend anchors remain explicit and deterministic.
4. The checker must fail closed if any upstream summary is missing, loses full coverage, or drifts from its contract id.
5. The checker must revalidate the `M272-D002` live runtime probe unless `--skip-dynamic-probes` is requested.
6. The checker must emit the closeout matrix rows and summary evidence under `tmp/reports/m272/M272-E002/`.
7. Summary path: `tmp/reports/m272/M272-E002/runnable_dispatch_control_matrix_summary.json`.
8. The next issue is `M273-A001`.

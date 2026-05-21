# Application Framework Samples

This directory contains checked-in Objective-C 3.0 application framework
samples for issue #8178.

The sample set is intentionally separate from the existing three-example
showcase portfolio. It gives the application-framework work a real source,
manifest, and validation surface without widening the established
`showcase/portfolio.json` contract before support-matrix integration is ready.

## Sample Set

- `libraries/routeModelKit`
  - object-model library sample with protocols, inheritance, synthesized
    properties, category extension, and typed key-path reflection.
- `libraries/interopAdapterKit`
  - interop-facing library sample with imported foreign declarations, header
    metadata, Swift/C++ naming metadata, derive metadata, property behavior, and
    macro provenance.
- `apps/workflowStdlibCLI`
  - CLI-style application sample that uses the current runtime-backed stdlib
    collection and text helper shape.
- `apps/asyncRuntimeConsole`
  - runtime application sample with async executor annotations, task runtime
    calls, and actor-shaped mailbox behavior.

## Validation

The sample walkthrough is checked in at
`docs/tutorials/application-framework-samples.md`. Run the focused checker from
the repository root:

```powershell
npm run objc3c -- validate-application-framework-samples
```

The checker validates `manifest.json`, each workspace manifest, each
`replay-contract.json`, package dependency edges, tutorial routes, and compiles
each sample source through:

```powershell
npm run objc3c -- compile-objc3c -- <source> --out-dir <sample artifact root> --emit-prefix module
```

Outputs are machine-owned under `tmp/artifacts/application-framework-samples/`
and `tmp/reports/application-framework-samples/`. The checked-in replay
contracts assert the real emitted module identity, source path, package edges,
runtime registration descriptor counts, required artifacts, and compile
truthfulness provenance for each sample.

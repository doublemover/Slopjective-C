# Application Framework Samples

The application framework samples are checked-in Objective-C 3.0 packages under
`showcase/applicationFrameworkSamples`. They are intended as small, cohesive
programs and libraries that exercise the current public language and runtime
surface through normal `npm run objc3c -- ...` commands.

## routeModelKit

`routeModelKit` is a library sample built around object-model behavior:
protocol requirements, class inheritance, synthesized properties, category
extension, message dispatch, and typed key-path reflection.

```powershell
npm run objc3c -- compile-objc3c -- showcase/applicationFrameworkSamples/libraries/routeModelKit/main.objc3 --out-dir tmp/artifacts/application-framework-samples/routeModelKit --emit-prefix module
```

## interopAdapterKit

`interopAdapterKit` is a library sample for interop-facing metadata. It keeps
foreign import declarations, generated header metadata, Swift/C++ spelling
metadata, derive metadata, property behavior, and macro provenance in one
workspace.

```powershell
npm run objc3c -- compile-objc3c -- showcase/applicationFrameworkSamples/libraries/interopAdapterKit/main.objc3 --out-dir tmp/artifacts/application-framework-samples/interopAdapterKit --emit-prefix module
```

## workflowStdlibCLI

`workflowStdlibCLI` is a CLI-style application sample that uses the current
runtime-backed stdlib helper surface for text-like counts, byte-span prefixes,
array windows, and map-entry presence/value access.

```powershell
npm run objc3c -- compile-objc3c -- showcase/applicationFrameworkSamples/apps/workflowStdlibCLI/main.objc3 --out-dir tmp/artifacts/application-framework-samples/workflowStdlibCLI --emit-prefix module
```

## asyncRuntimeConsole

`asyncRuntimeConsole` is a runtime application sample that combines the
concurrency stdlib module, executor annotations, task runtime helpers, executor
hops, and actor-shaped mailbox behavior.

```powershell
npm run objc3c -- compile-objc3c -- showcase/applicationFrameworkSamples/apps/asyncRuntimeConsole/main.objc3 --out-dir tmp/artifacts/application-framework-samples/asyncRuntimeConsole --emit-prefix module
```

## Validate

The focused checker validates the manifest, workspace manifests,
`replay-contract.json` files, tutorial routes, package edges, clean artifact
roots, and sample compile outputs:

```powershell
npm run objc3c -- validate-application-framework-samples
```

## Inspect And Modify

Each sample keeps its source in `main.objc3`, its package/build contract in
`workspace.json`, and its replay contract in `replay-contract.json`. After
compiling a sample, inspect the emitted `module.ll`,
`module.manifest.json`, `module.compile-provenance.json`, and
`module.runtime-registration-manifest.json` files under that sample's
`tmp/artifacts/application-framework-samples/<sample-id>` directory.

Modify a sample by editing its `main.objc3` source and, when package metadata
changes, the adjacent `workspace.json` and `replay-contract.json`. Re-run the
same public compile command for the sample or run the full
`validate-application-framework-samples` action to rebuild from a clean sample
artifact root.

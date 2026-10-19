# Immutable offline Linux packages

`objc3c-package` publishes the verified objects and semantic interfaces of a
completed source-package graph. Publication stores a whole dependency closure
under its SHA-256 content identity. Installation copies that closure into a
separate content-addressed store; consumers compile their own source and link
installed provider objects. Provider sources and build directories are unnecessary.

```sh
objc3c-build --workspace ./packages/workspace.json --output-dir ./package-build
objc3c-package publish --build-dir ./package-build --store ./published
# Use the content_sha256 and package path printed by publication.
objc3c-package install --package ./published/CONTENT_SHA256 \
  --content-sha256 CONTENT_SHA256 --store ./installed
objc3c-package consume --package ./installed/CONTENT_SHA256 \
  --content-sha256 CONTENT_SHA256 --application ./client/application.json \
  --output-dir ./consumer-build
```

The repository equivalent is `python -B -m scripts.objc3c_toolchain packages`,
followed by the same `publish`, `install` or `consume` arguments. Select the SDK
with `--sdk-root` before the subcommand or `OBJC3C_SDK_ROOT`.

A consumer manifest uses `schema: "objc3c.binary-consumer.v1"`, a relative
`source`, explicit `language`, `standard`, `ownership`, and `dependencies`.
Dependency entries contain the exact module `name`, `package` identity and
`visibility` (`public` or `private`), following the
[source-package contract](linux_source_packages.md). Its source root is the
manifest's directory. Build output must be disjoint from that directory, the
package and the SDK.

```json
{
  "schema": "objc3c.binary-consumer.v1",
  "source": "main.mm",
  "language": "objective-c++",
  "standard": "c++23",
  "ownership": "arc",
  "dependencies": [{ "name": "fixture.Top", "package": "top-v1", "visibility": "private" }]
}
```

Publication requires the successful v2 build report and matching installation
inventory. It validates every retained object, interface and sidecar against the
inventory, preserving full native module identities and dependency visibility.
A package's `package.json` uses `objc3c.binary-package.v1` and records its exact
producing SDK seal, target, optimization and module inventory. The content hash
covers canonical JSON with sorted object keys; artifact hashes bind every byte.
Consumers require the producing SDK seal, language, standard, ownership and
package locks. Rebuilding under a new SDK produces a new package identity.

The local store accepts exactly the listed regular files and directories.
Symbolic links, special files, missing or extra files, inconsistent sidecars,
cycles and unresolved dependency locks are rejected. Publication and installation
copy into private candidates, verify contents, and atomically publish complete
entries under a store lock. An existing entry is verified before reuse. A damaged
entry is preserved for inspection and is never silently repaired. Installed files
are read-only; every use verifies their hashes.

The store can be relocated or copied between compatible native Linux x86-64
hosts. Pass its content identity separately to installation and consumption.
Directory transport and trust in that identity belong to the caller. Remote
registries, version selection and cross-ownership adaptation remain separate
milestones.

Consumer compilation preserves native public/private transitive visibility,
selects only the requested dependency closure and never compiles a provider.
Its report records the SDK seal, package identity, selected modules, commands,
source/header hashes and executable hash. Rejected compilation removes a prior
success report while preserving the compiler's earlier object. SDK, package and
source contents are checked again before publishing a successful report.

Installed qualification covers all eight supported language/standard/ownership
modes at O0/O2: 16 source-free consumer executions and 128 rejection checks.
Each case reproduces publication and installation, relocates the store, deletes
producer sources and build outputs, and executes the consumer. Objective-C++ ARC
cases transport owned records through re-exports, copy them and verify final
release and weak zeroing. Rejections cover wrong content, SDK, mode and package locks,
private-module lookup and damaged objects, interfaces and sidecars. The SDK and
fresh-distribution gates run this suite outside the checkout as ordinary users.

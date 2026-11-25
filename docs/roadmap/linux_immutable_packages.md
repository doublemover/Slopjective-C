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
objc3c-package export --package ./published/CONTENT_SHA256 \
  --content-sha256 CONTENT_SHA256 --output ./providers.zip
objc3c-package inspect --package ./providers.zip --content-sha256 CONTENT_SHA256 \
  --module fixture.Top top-v1
objc3c-package install --package ./providers.zip \
  --content-sha256 CONTENT_SHA256 --store ./installed
objc3c-package consume --package ./installed/CONTENT_SHA256 \
  --content-sha256 CONTENT_SHA256 --application ./client/application.json \
  --output-dir ./consumer-build
```

The repository equivalent is `python -B -m scripts.objc3c_toolchain packages`,
followed by the same subcommand and arguments. For publication or consumption,
select the SDK with `--sdk-root` before the subcommand or `OBJC3C_SDK_ROOT`.

A consumer manifest uses `schema: "objc3c.binary-consumer.v3"`, a relative
`source`, explicit `language`, `standard`, `ownership`, and `dependencies`.
Dependency entries contain the exact module `name`, `package` identity and
`visibility` (`public` or `private`), following the
[source-package contract](linux_source_packages.md). Its source root is the
manifest's directory. Build output must be disjoint from that directory, the
package and the SDK.

```json
{
  "schema": "objc3c.binary-consumer.v3",
  "source": "main.mm",
  "language": "objective-c++",
  "standard": "c++23",
  "ownership": "arc",
  "dependencies": [{ "name": "fixture.Top", "package": "top-v1", "visibility": "private" }]
}
```

Publication requires the successful v4 build report and matching installation
inventory. It validates every retained object, interface and sidecar against the
inventory, preserving full native module identities and dependency visibility.
A package's `package.json` uses `objc3c.binary-package.v3` and records its exact
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

The store can be relocated or copied between compatible native Linux hosts.
Installation accepts a verified directory or one compressed ZIP. Archive
export sorts the artifact paths, writes a canonical manifest, and uses fixed
file metadata. It reports the archive SHA-256 separately from the package
content identity. Supply the trusted content identity independently when
inspecting, installing, or consuming.

Use `registry-publish` to build a versioned index and its immutable archives
for a static HTTP or HTTPS host. Publish the contents of `--registry-dir`
together and supply the index digest from a trusted release record. The fetch
command verifies the index, archive bytes, exact native module locks and
producing SDK before installation. Add `--offline` to replay the same index
and package locks from the verified local store without a network request.

```sh
objc3c-package registry-publish --package ./published/CONTENT_SHA256 \
  --content-sha256 CONTENT_SHA256 --registry-dir ./registry
objc3c-package fetch --index-url https://packages.example.org/index.json \
  --index-sha256 INDEX_SHA256 --content-sha256 CONTENT_SHA256 \
  --module fixture.Top top-v1 --store ./installed
```

`inspect` verifies either representation and prints the target, SDK seal,
optimization, module identities, artifact hashes and dependency visibility as
JSON. Repeat `--module NAME PACKAGE` to select exact roots and see their
dependency-first closure. With no selections, all modules are reported.
[Closed C ABI ownership projections](linux_ownership_projections.md) preserve the
actual provider mode and canonical producer proof in the binary inventory.

Consumer compilation preserves native public/private transitive visibility,
selects only the requested dependency closure and never compiles a provider.
Its report records the SDK seal, package identity, selected modules, commands,
source/header hashes and executable hash. Rejected compilation removes a prior
success report while preserving the compiler's earlier object. SDK, package and
source contents are checked again before publishing a successful report.

Repeat `--add-package DIRECTORY CONTENT_SHA256` on `consume` to supply more
verified package closures. All closures use the same producing SDK, target and
optimization. The application's exact dependency and mode locks determine what
it can import. A shared module contributes one native object only when the
module identity, declarations, edges and artifact hashes agree across every
closure that contains it. The first matching directory supplies those bytes
in primary-then-added order. The report records every supplied package lock
and the lock that supplied each linked module.

Installed qualification covers all eight supported language/standard/ownership
modes at O0/O2: 16 source-free consumer executions and 128 rejection checks.
Each case reproduces publication and archive export, verifies archive inspection,
installs from the archive, relocates the store, deletes producer sources and
build outputs, and executes the consumer. Objective-C++ ARC
cases transport owned records through re-exports, copy them and verify final
release and weak zeroing. Rejections cover wrong content, SDK, mode and package locks,
private-module lookup and damaged objects, interfaces and sidecars. The SDK and
fresh-distribution gates run this suite outside the checkout as ordinary users.
The composition witness builds two independent publishers, deletes the producer
tree, and links their relocated closures with one shared object. It also checks
that an available but unselected package cannot expose a private transitive module.

The [source-bound acceptance receipt](../support/linux_immutable_package_acceptance.json)
records the exact producing archive, all-mode source-free consumers, fault tests,
installed gates, sanitizer/debugger evidence and clean Debian/Ubuntu results.

Binary v3 modules record `provider_ownership` separately from the consumer mode.
Publication, installation and consumption check its agreement with native
projection provenance before compilation. The compiler reconstructs the exact
opposite-mode producer contract when it imports the projected interface. Earlier
binary formats and interfaces require rebuilding with the current SDK.

Binary v3 modules also require their canonical `exports` policy. Named policies
must agree exactly with the native sidecar root names before consumer compilation.
Earlier binary formats are rejected.

Semantic interface schema 2.2 records a native `export_root` for every declaration.
Binary installation verifies that each root exists in the interface, that every
selected export is a native root, and that all roots belong to the selected
namespace/class/alias/using policy. Namespace ancestors retained for a selected
leaf do not export siblings. Class static members and friend functions belong
to their class root. Native import regenerates and compares these root identities
against the parsed source, and semantic fingerprints bind them. Schema 2.1
interfaces require regeneration for this cutover.

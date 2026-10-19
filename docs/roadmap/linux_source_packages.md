# Offline Linux source packages

The installed `objc3c-build` command builds an explicit local package graph with
the qualified native compiler. It resolves exact package identities, builds each
module once in dependency order, imports locked semantic interfaces, links the
application and runs it. No dependency is fetched or selected by version range.

```sh
cp -R "$OBJC3C_SDK_ROOT/share/objc3c/examples/packages" ./packages
objc3c-build --workspace ./packages/workspace.json \
  --output-dir ./package-build --optimization O2
```

The repository command is equivalent:

```sh
npm run objc3c -- build-packages --workspace ./packages/workspace.json \
  --output-dir ./package-build --optimization O2
```

Select an installed SDK with `OBJC3C_SDK_ROOT` or `--sdk-root`. The SDK is verified
before and after building. Sources and output must be outside that immutable SDK.
Output may live under the workspace, such as `packages/build`, but must be
separate from every package/application source root, including unused packages.
The installed example is copied to an editable workspace above.

## Workspace and package contracts

A workspace uses `schema: "objc3c.source-workspace.v1"`, a `packages` array of
relative package-manifest paths, and an `application` object. The application
specifies a relative `root`, relative `source`, `ownership` (`arc` or `mrc`),
and `dependencies`.

Each package manifest uses `schema: "objc3c.source-package.v1"`, a canonical
module `name`, an exact opaque `package` identity, a relative `source`,
`ownership`, and `dependencies`. Its source root is the manifest's directory.
Application and package roots must be separate. Source paths and symbolic links
cannot escape their roots; source directory links are rejected.

Every dependency contains `name`, `package`, and `visibility` (`public` or
`private`). The package identity must match exactly. All edges currently require
the same ARC/MRC mode; cross-ownership interface adaptation is separate work.
Unknown fields, duplicate modules/edges, unresolved locks and cycles are errors.
Unused declarations are validated, but only the application's dependency closure
is built.

Native semantic imports enforce visibility. A public edge re-exports its module;
a private edge remains available to its declaring module. Physical availability
of a transitive artifact does not expose its declarations to an importer. The
installed diamond example uses a type through two public edges and separately
checks rejection of a private transitive module.

## Reuse and retained records

Compilation reuse checks SDK seal, recipe, command, source contents and directory
membership, dependency locks, native interface contents, Clang depfiles and every
retained output hash. Preserved timestamps do not preserve a cache hit after a
content change. Provider implementation edits rebuild their object; unchanged
semantic interfaces allow importers to reuse compilation. Changed public API
invalidates transitive importers. Linking always refreshes the host's CRT and
system-library selection, then the application executes.

`build-report.json` retains module identities, workspace-manifest hashes, node
inputs/outputs, reuse decisions and the executable hash. A compiler or linker
failure invalidates the pending successful build record. The native compiler's
publication boundary preserves earlier object/interface products on rejected
compilation. Manifest preflight rejection leaves the prior completed record
untouched; its retained manifest hashes identify the source state it describes.

`package-installation.json` records each selected module's full identity, direct
visibility locks and relative object/interface/sidecar paths with content hashes.
Its content identity is reproduced by unchanged builds. The record describes the
build output; it is not a remote registry, general binary package installer or
public module-language syntax.

ARC/MRC O0/O2 installed qualification covers cold/warm builds, a shared diamond leaf,
provider body edits with preserved timestamps, private implementation edits,
damaged objects/interfaces, transitive API changes, wrong package locks and
private visibility rejection. The suite runs outside the checkout with spaces
and quotes in source paths, and is included in SDK packaging and ordinary-user
fresh-distribution archive qualification.

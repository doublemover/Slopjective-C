# Linux module SPI

The Linux SDK preserves public, named SPI and private module partitions through
native declaration identities, source graphs and immutable packages. Explicit
`exports` still selects the supported roots. A selected root carrying a native
`annotate("objc3.spi:Group")` attribute belongs to that group, including its fields,
enumerators and Objective-C members. Unselected roots remain private.

```objc
#define SPI(group) __attribute__((annotate("objc3.spi:" #group)))
int public_value(void);
SPI(Diagnostics) int inspect_state(void);
struct SPI(Diagnostics) State { int value; };
```

A group contains 1–64 ASCII identifier characters and starts with a letter or
underscore. One root has one group. Members and Objective-C categories cannot
introduce groups. Public signatures can use public types. An SPI signature can
also use types in its own local group. Exported signatures cannot expose another
group or an imported SPI type; implementations may use their explicitly granted
imports internally.

Consumers receive public declarations by default. A privilege is an explicit
build configuration grant attached to an exact provider lock:

```sh
objc3c client.mm --objc3 --objcxx --arc --source-imports \
  --module-search-path ./modules \
  --import-module Example.Diagnostics <fingerprint> diagnostics-v1 \
  --import-spi Diagnostics -- -std=c++23
```

The source still imports that module with `@import Example.Diagnostics;`. Registry
entries grant no lookup before their import directive. Qualified and unqualified
lookup use the same partition. Unknown, malformed or duplicate grants fail.
Grants do not flow through public dependency edges or re-exports. Importing a
re-exported module explicitly can activate its configured grant at that source
position.

Source and immutable consumer dependency locks accept a `spi` array:

```json
{
  "name": "Example.Diagnostics",
  "package": "diagnostics-v1",
  "visibility": "private",
  "spi": ["Diagnostics"]
}
```

The builder canonicalizes groups, includes them in the compile recipe and
passes them to the native compiler. Immutable manifests require canonical order.
Changing a grant invalidates that importer. Changing a declaration's partition
changes its semantic fingerprint and invalidates dependent compilations. The
native dependency record includes effective direct grants. The size-guarded V3
embedding options expose the same module/group pairs through `spi_grants`.

The canonical source retains native annotations. Sidecar declaration identities
bind their group and require `objc3.api-partitions.v1`. Import preparation reparses
all declarations and validates the full contract before mutation; AST import
then inserts only authorized declarations. Ungranted Objective-C classes and
protocols therefore cannot contribute methods to consumer lookup. Partition
metadata is compiler visibility, not encryption or a runtime access-control
boundary.

Installed qualification covers Objective-C 3.0 (C17 base mode) ARC/MRC, Objective-C++ (C++20/23 mode) ARC/MRC
and C++20/23 at O0/O2. It executes public and privileged source graphs, cache
reuse, a private edit, an importer grant edit, a partition edit and relocated
source-free immutable consumers. It checks ordinary and qualified rejections,
revocation, unknown groups, Objective-C selector isolation and tampered native
metadata, while preserving the previous object after failed compilation.

The [native naming contract](linux_module_owned_names.md) and
[structured runtime services](linux_structured_runtime.md) build on these module
boundaries. Language async/await and compiler actor isolation remain reserved.
The native object linkage of these SPI declarations follows ordinary C,
Objective-C and C++ rules.

The [acceptance receipt](../support/linux_module_spi_acceptance.json) binds the
qualified archive, native tests, installed execution, sanitizer and debugger
evidence, fresh Debian 13 and Ubuntu 26.04 runs, and cached archive reuse. The
[latency report](../support/linux_module_spi_latency.json) records warm command
measurements and regression budgets for the measured CPU and quota.

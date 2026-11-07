# Relocatable native standard headers

The Linux SDK assembly now installs the native C++ standard-library header trees
and the platform C-header closure they use. Discovery reads the compiler's actual
search order and cc1 directory classifications. It retains logical include paths
through platform symlinks, rather than losing paths such as `asm/types.h` when
their physical targets live outside `/usr/include`.

The current development probe contains 1,000 files / 14,118,363 bytes, including
distribution license notices. Temporary discovery and installation directories
clean up automatically. The final bundle contains ordinary files and a content
manifest, with no absolute source paths or external source symlinks.

`clang.cfg` and `clang++.cfg` select the bundled C++ headers and preserve resource,
system and extern-C search order. Paths use `<CFGDIR>` so relocation does not
change the configuration. Copies inside the compiler resource directory bind
the configurations into resource identity; the native SDK verifier also checks
that the driver configurations match those copies. Explicit alternative native
library configurations can opt out of default driver configuration.

Release reuse binds current native header bytes independently of LLVM source.
Cached LLVM foundations exclude the project header bundle and driver configs;
assembly reinstalls them after verifying the reusable foundation. Source edits
with unchanged size/timestamp invalidate the release input identity. A change
during installation prevents publishing that bundle.

All 33 focused bundle/reuse/publication tooling tests pass. The native SDK identity
unit passes its configuration-corruption control. A current development driver
and bundle were relocated, then compiled and ran a C++23 string/vector/algorithm
witness with ambient header search disabled; its include trace contains no host
header lookup. These are implementation controls, not a replacement SDK archive
qualification. Accepted SDK 2.62 remains unchanged. The matched compiler/tooling,
package/runtime consolidation and final archive delivery still follow the feature
implementation sprint.

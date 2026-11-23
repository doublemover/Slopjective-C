# Linux installer ABI admission

The [v0.1.0-preview.1 release](../releases/status.md) verified native compiler
build, Linux SDK packaging, clean installation, debug inspection, and installed
execution on x86-64 and AArch64. The SDK seals declare glibc `2.41` and the
`GLIBCXX_3.4.33` symbol version. The updated installer reads those requirements
from the hash-verified archive, checks the host ABI, and verifies the SDK seal
and ELF architecture before publishing an activation. An installed generation
goes through the same checks before reactivation.

## x86-64 replay

All runs used the installer from the checkout and the same released x86-64
archive. The archive SHA-256 was
`279269c7e87b56289e11e800097f2490dd18d2124fc456306665471fab207222`
and the admitted SDK seal was
`d7de09cc1d27fb7add8c2f4792d34d488384300df8d248318b3539e0cfcf2fd0`.
The published assets and the [Linux release manifest](../releases/preview-manifest.json)
record these values.

| Container and library input                                                                                             | Installer result                                                 | Activation and execution                                                                                                                            |
| ----------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| Debian 13 with glibc 2.41 and distribution `libstdc++6` 14.2.0-19                                                       | Exit 0, detected glibc 2.41 and verified `GLIBCXX_3.4.33`        | Activated the release seal, compiled and linked `main.objc3` with `-O2 -g`, ran the program, and verified its DWARF with `llvm-dwarfdump --verify`. |
| Debian 12 slim with glibc 2.36 and distribution `libstdc++6` 12.2.0-14+deb12u1                                          | Exit 2, `SDK requires glibc >= 2.41 (detected glibc 2.36)`       | Requested install root absent.                                                                                                                      |
| Debian 13 with glibc 2.41 and the Debian 12 `libstdc++.so.6` copied into the disposable container's system library path | Exit 2, `SDK requires system libstdc++.so.6 with GLIBCXX_3.4.33` | Requested install root absent.                                                                                                                      |
| Ubuntu 22.04 with glibc 2.35 and distribution Python 3.10.12                                                            | Exit 2, `SDK requires glibc >= 2.41 (detected glibc 2.35)`       | Requested install root absent.                                                                                                                      |

The images resolved to
`debian@sha256:9c6e381421d0f48d1b8133e573f33c185ee16cbf505eda9c4a8a4791b25ea8a6`
and `debian@sha256:734a72b3d6142a71993b4bc580ca01d8ca687f77a2c422d96a961d3376b179bc`,
respectively. The Debian 12 library bytes had SHA-256
`e7848e32af4932840ba775169041759a2a8dd5a008af360e5c55bce506eebcf4`.
The Ubuntu 22.04 image resolved to
`ubuntu@sha256:b000e69a50ff940390233025e7a337fb8469b509e2334fa4d018ac30f3e18cdb`.
A second Debian 13 invocation verified the ABI requirements again, reused the
existing seal, and ran the activated compiler's version command.

Replay with the checkout's `scripts/install_objc3c_linux.sh`, the published
archive and its checksum. Use `--provision` on a clean Debian 13 container. On
the negative containers install `python3`, `zstd` and `libstdc++6` first, pass
`--archive`, `--sha256` and a fresh `--root`, and check exit status 2 and the
absence of the requested root. For the separate libstdc++ check, copy the
old library onto the resolved `libstdc++.so.6` path inside the disposable
Debian 13 container.

The [installer regressions](../../tests/tooling/test_toolchain_install.py)
cover wrong ABI metadata, exact symbol rejection, and preservation of the
active generation when a previously installed archive fails readmission.
The [published native x86-64 run](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421)
and [AArch64 run](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382)
qualify the release archives. Native AArch64 replay with the updated installer
belongs to the next dual-architecture release rehearsal.

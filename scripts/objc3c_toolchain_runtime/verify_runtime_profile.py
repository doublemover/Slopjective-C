#!/usr/bin/env python3
"""Fail-closed checks for the pinned native Objective-C runtime profile."""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

PIN = "e877e782fb965c4e870d2ecf6aff58dddc6290ae"
ROBIN_MAP_PIN = "4ec1bf19c6a96125ea22062f38c2cf5b958e448e"
WINDOWS_TOOLS = ("clang.exe", "clang++.exe", "lld-link.exe")


def fail(message: str) -> int:
    print(f"BLOCKED: {message}", file=sys.stderr)
    return 2


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=Path, default=Path(__file__).with_name("runtime-profile.json"))
    parser.add_argument("--profile", default="windows-x86_64")
    parser.add_argument("--source", type=Path, help="existing local libobjc2 checkout; this tool never downloads")
    parser.add_argument("--robin-map-source", type=Path, help="existing local robin-map v1.4.0 checkout")
    parser.add_argument("--bin-dir", type=Path, help="directory containing pinned Clang and LLD drivers")
    args = parser.parse_args()

    try:
        manifest = json.loads(args.manifest.read_text(encoding="utf-8"))
        profile = manifest["profiles"][args.profile]
    except (OSError, ValueError, KeyError) as exc:
        return fail(f"runtime manifest is unreadable or lacks {args.profile}: {exc}")

    if manifest.get("schema_version") != 1:
        return fail("unsupported runtime manifest schema")
    if args.profile in ("windows-x86_64", "linux-x86_64"):
        if profile.get("runtime_commit") != PIN or profile.get("runtime_release") != "2.3":
            return fail("GNUstep runtime pin does not match libobjc2 v2.3 commit " + PIN)
        if profile.get("objc_abi_flag") != "-fobjc-runtime=gnustep-2.0":
            return fail("GNUstep-v2 ABI flag is missing or changed")
        if profile.get("blocks_provider") != "libobjc2-embedded":
            return fail("profile must select exactly the embedded Blocks provider")

    if args.profile == "windows-x86_64":
        if profile.get("runtime_build_drivers") != ["clang.exe", "clang++.exe"]:
            return fail("canonical Windows runtime build must use clang/clang++ GNU drivers")
        if profile.get("runtime_build_with_clang_cl") is not False:
            return fail("clang-cl must not be selected to build libobjc2")
        if profile.get("runtime_toolset") != "Visual Studio 2022 v143":
            return fail("Windows runtime profile must name the VS2022 v143 environment")
        if profile.get("runtime_linker") != "lld-link.exe" or profile.get("runtime_generator") != "Ninja":
            return fail("Windows runtime profile must use Ninja and LLD")
        if args.bin_dir:
            missing = [tool for tool in WINDOWS_TOOLS if not (args.bin_dir / tool).is_file()]
            if missing:
                return fail("toolchain executables unavailable: " + ", ".join(missing))
            for compiler in ("clang.exe", "clang++.exe", "lld-link.exe"):
                try:
                    version = subprocess.check_output(
                        [str(args.bin_dir / compiler), "--version"], text=True,
                        stderr=subprocess.STDOUT,
                    )
                except (OSError, subprocess.CalledProcessError) as exc:
                    return fail(f"cannot query {compiler}: {exc}")
                if "22.1.6" not in version or "fc4aad7b5db3fff421df9a9637605b9ca5667881" not in version:
                    return fail(f"{compiler} is not from pinned LLVM 22.1.6: {version.splitlines()[0]}")

    if args.source:
        if not args.source.is_dir():
            return fail(f"runtime checkout does not exist: {args.source}")
        try:
            revision = subprocess.check_output(
                ["git", "-C", str(args.source), "rev-parse", "HEAD"], text=True,
                stderr=subprocess.STDOUT,
            ).strip()
        except (OSError, subprocess.CalledProcessError) as exc:
            return fail(f"cannot read local runtime revision: {exc}")
        if revision != PIN:
            return fail(f"runtime source is {revision}, expected pinned commit {PIN}")
        cmake = args.source / "CMakeLists.txt"
        if not cmake.is_file():
            return fail("pinned checkout has no top-level CMakeLists.txt")
        try:
            arc_header = (args.source / "objc" / "objc-arc.h").read_text(encoding="utf-8")
            blocks_header = (args.source / "objc" / "blocks_runtime.h").read_text(encoding="utf-8")
        except OSError as exc:
            return fail(f"pinned public ARC/Blocks headers are missing: {exc}")
        weak_prototypes = {
            "objc_initWeak": r"\bid\s+objc_initWeak\s*\(\s*id\s*\*\s*\w+\s*,\s*id\s+\w+\s*\)\s*;",
            "objc_loadWeakRetained": r"\bid\s+objc_loadWeakRetained\s*\(\s*id\s*\*\s*\w+\s*\)\s*;",
            "objc_storeWeak": r"\bid\s+objc_storeWeak\s*\(\s*id\s*\*\s*\w+\s*,\s*id\s+\w+\s*\)\s*;",
            "objc_destroyWeak": r"\bvoid\s+objc_destroyWeak\s*\(\s*id\s*\*\s*\w+\s*\)\s*;",
        }
        missing_weak_api = [name for name, pattern in weak_prototypes.items()
                            if not re.search(pattern, arc_header)]
        if missing_weak_api:
            return fail("pinned objc/objc-arc.h prototypes mismatch: " + ", ".join(missing_weak_api))
        for symbol in ("_Block_copy", "_Block_release"):
            if not re.search(r"\b" + symbol + r"\s*\(\s*const\s+void\s*\*", blocks_header):
                return fail(f"pinned objc/blocks_runtime.h does not declare {symbol}(const void *)")
        if args.profile == "windows-x86_64":
            if args.robin_map_source is None or not args.robin_map_source.is_dir():
                return fail("local robin-map v1.4.0 source is unavailable; configure would fetch from the network")
            try:
                robin_revision = subprocess.check_output(
                    ["git", "-C", str(args.robin_map_source), "rev-parse", "HEAD"], text=True,
                    stderr=subprocess.STDOUT,
                ).strip()
            except (OSError, subprocess.CalledProcessError) as exc:
                return fail(f"cannot read local robin-map revision: {exc}")
            if robin_revision != ROBIN_MAP_PIN:
                return fail(f"robin-map source is {robin_revision}, expected {ROBIN_MAP_PIN}")
            if not (args.robin_map_source / "include" / "tsl" / "robin_map.h").is_file():
                return fail("pinned robin-map checkout lacks include/tsl/robin_map.h")
    else:
        return fail(f"pinned libobjc2 source unavailable; expected local checkout at commit {PIN}")

    if args.bin_dir is not None and not args.bin_dir.is_dir():
        return fail(f"Clang/LLD bin directory unavailable: {args.bin_dir}")
    print(f"PROFILE VERIFIED: {args.profile}")
    print(f"RUNTIME SOURCE VERIFIED: libobjc2 v2.3 at {PIN}")
    if args.robin_map_source:
        print(f"DEPENDENCY VERIFIED: robin-map v1.4.0 at {ROBIN_MAP_PIN}")
    if args.bin_dir:
        print("TOOLCHAIN VERIFIED: Clang/LLD 22.1.6 files match the pinned source revision")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

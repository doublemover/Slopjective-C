"""Run an Apple SDK-backed syntax fixture only on an Apple build host.

This check does not replace a linked Apple runtime ownership test. Non-Apple
hosts skip explicitly instead of treating an unavailable SDK as success.
"""

from __future__ import annotations

import platform
import subprocess
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]


@unittest.skipUnless(
    platform.system() == "Darwin",
    "Apple Objective-C runtime headers/SDK are unavailable on this host; Apple adapter remains unverified",
)
class AppleNativeAdapterSyntaxTests(unittest.TestCase):
    def test_adapter_and_weak_slot_fixture_with_apple_sdk(self) -> None:
        sdk = subprocess.run(
            ["xcrun", "--sdk", "macosx", "--show-sdk-path"],
            check=True,
            text=True,
            capture_output=True,
        ).stdout.strip()
        compiler = subprocess.run(
            ["xcrun", "--sdk", "macosx", "--find", "clang++"],
            check=True,
            text=True,
            capture_output=True,
        ).stdout.strip()
        adapter = (
            REPO_ROOT
            / "native/objc3c/src/runtime/platform/objc3_native_object_adapter.mm"
        )
        fixture = (
            REPO_ROOT
            / "tests/tooling/fixtures/clang_foundation/runtime/native_weak_slot_adapter.mm"
        )
        for source in (adapter, fixture):
            with self.subTest(source=source.name):
                subprocess.run(
                    [
                        compiler,
                        "-isysroot",
                        sdk,
                        "-fobjc-arc",
                        "-fblocks",
                        "-std=c++17",
                        "-I",
                        str(REPO_ROOT / "native/objc3c/src"),
                        "-fsyntax-only",
                        str(source),
                    ],
                    check=True,
                    text=True,
                )


if __name__ == "__main__":
    unittest.main()

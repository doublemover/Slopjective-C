import json
import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PROFILE = ROOT / "scripts/objc3c_toolchain_runtime/runtime-profile.json"
VERIFIER = ROOT / "scripts/objc3c_toolchain_runtime/verify_runtime_profile.py"
ADAPTER = ROOT / "native/objc3c/src/runtime/platform/objc3_native_object_adapter.h"
FIXTURES = ROOT / "tests/tooling/fixtures/clang_foundation/runtime"
RUNTIME_SOURCE = ROOT / "out/toolchains/libobjc2-v2.3/src"
ROBIN_MAP_SOURCE = ROOT / "out/toolchains/robin-map-v1.4.0/src"


class ClangFoundationRuntimeProfileTests(unittest.TestCase):
    def test_runtime_tuple_uses_pinned_gnustep_v2_and_one_blocks_provider(self):
        manifest = json.loads(PROFILE.read_text(encoding="utf-8"))
        expected = "e877e782fb965c4e870d2ecf6aff58dddc6290ae"
        for name in ("windows-x86_64", "linux-x86_64"):
            profile = manifest["profiles"][name]
            self.assertEqual(profile["runtime_release"], "2.3")
            self.assertEqual(profile["runtime_commit"], expected)
            self.assertEqual(profile["objc_abi_flag"], "-fobjc-runtime=gnustep-2.0")
            self.assertEqual(profile["blocks_provider"], "libobjc2-embedded")
        windows = manifest["profiles"]["windows-x86_64"]
        self.assertEqual(windows["runtime_build_drivers"], ["clang.exe", "clang++.exe"])
        self.assertEqual(windows["robin_map_release"], "v1.4.0")
        self.assertEqual(windows["robin_map_commit"], "4ec1bf19c6a96125ea22062f38c2cf5b958e448e")
        self.assertEqual(windows["windows_sdk_version"], "10.0.26100.0")
        self.assertFalse(windows["runtime_build_with_clang_cl"])
        self.assertIn("clang-cl.exe", windows["client_drivers"])

    def test_missing_runtime_checkout_is_a_blocker(self):
        result = subprocess.run(
            [sys.executable, str(VERIFIER), "--profile", "windows-x86_64"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(result.returncode, 2)
        self.assertIn("pinned libobjc2 source unavailable", result.stderr)

    @unittest.skipUnless(RUNTIME_SOURCE.is_dir() and ROBIN_MAP_SOURCE.is_dir(),
                         "pinned runtime dependency checkouts are not present")
    def test_pinned_checkouts_and_public_runtime_headers_are_accepted(self):
        result = subprocess.run(
            [
                sys.executable,
                str(VERIFIER),
                "--profile", "windows-x86_64",
                "--source", str(RUNTIME_SOURCE),
                "--robin-map-source", str(ROBIN_MAP_SOURCE),
            ],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("RUNTIME SOURCE VERIFIED", result.stdout)
        self.assertIn("DEPENDENCY VERIFIED", result.stdout)

    def test_native_adapter_is_pointer_based_and_has_runtime_weak_lifecycle(self):
        header = ADAPTER.read_text(encoding="utf-8")
        self.assertIn("typedef void *objc3_native_object_t", header)
        self.assertIn("objc3_native_object_retain", header)
        self.assertIn("objc3_native_object_release", header)
        self.assertIn("objc3_native_weak_init", header)
        self.assertIn("objc3_native_weak_load_retained", header)
        self.assertIn("objc3_native_weak_store", header)
        self.assertIn("objc3_native_weak_destroy", header)
        self.assertIn("objc3_native_block_copy", header)
        self.assertNotIn("retain_i32", header)

    def test_root_class_fixtures_cover_arc_mrc_objcxx_consumed_weak_and_blocks(self):
        expected = {
            "root_class_arc_weak_blocks.m": ("objc_root_class", "__weak", "MakeReader"),
            "root_class_arc_consumed.m": ("ns_consumed", "ObserveConsumed"),
            "root_class_mrc.m": ("retainCount", "- (oneway void)release"),
            "root_class_objcxx_aggregate_cleanup.mm": ("OwnedAggregate", "runtime_error"),
            "native_weak_slot_adapter.mm": (
                "objc3_native_weak_init", "objc3_native_weak_load_retained", "destroyed"),
        }
        for filename, markers in expected.items():
            source = (FIXTURES / filename).read_text(encoding="utf-8")
            for marker in markers:
                with self.subTest(fixture=filename, marker=marker):
                    self.assertIn(marker, source)
            self.assertNotIn("Foundation/Foundation.h", source)


if __name__ == "__main__":
    unittest.main()

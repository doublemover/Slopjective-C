import concurrent.futures
import copy
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import sys
import tempfile
import threading
import unittest
from unittest import mock


SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "objc3c_native_validation"
    / "artifacts.py"
)
SPEC = importlib.util.spec_from_file_location("native_validation_artifacts", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load native validation artifact helpers")
artifacts = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = artifacts
SPEC.loader.exec_module(artifacts)


class NativeValidationArtifactsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.directory = tempfile.TemporaryDirectory(prefix="native-validation-artifacts-")
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name).resolve()
        self.allowed = self.root / "allowed"
        self.allowed.mkdir()
        self.proof = self.allowed / "proof.bin"
        self.proof.write_bytes(b"native artifact")
        self.result = self.allowed / "results" / "node.json"

    def write(self, **overrides) -> dict:
        arguments = {
            "node_id": "facade-tests",
            "node_identity": "node-input-identity",
            "build_identity": "build-identity",
            "run_id": "run-1",
            "artifacts": {"proof": self.proof},
            "data": {"checks": ["native tests"]},
            "allowed_roots": [self.allowed],
        }
        arguments.update(overrides)
        return artifacts.write_node_result(self.result, **arguments)

    def read(self, **overrides) -> dict:
        arguments = {
            "node_id": "facade-tests",
            "node_identity": "node-input-identity",
            "build_identity": "build-identity",
            "allowed_roots": [self.allowed],
        }
        arguments.update(overrides)
        return artifacts.read_node_result(self.result, **arguments)

    def forge(self, payload: dict) -> None:
        self.result.write_text(json.dumps(payload), encoding="utf-8")

    def test_complete_inventory_round_trip_and_metadata_paths_are_not_artifacts(self) -> None:
        empty = self.allowed / "empty.txt"
        empty.write_bytes(b"")
        payload = self.write(
            artifacts={"proof": self.proof, "empty": empty},
            data={"unvalidated_metadata_path": str(self.root / "not-an-artifact")},
        )
        self.assertEqual(payload["contract_id"], "objc3c.native_validation.node_result.v1")
        self.assertEqual(payload["status"], "PASS")
        self.assertEqual(set(payload["artifacts"]), {"proof", "empty"})
        self.assertEqual(payload["artifacts"]["proof"], {
            "path": str(self.proof.resolve()),
            "size": len(b"native artifact"),
            "sha256": hashlib.sha256(b"native artifact").hexdigest(),
        })
        self.assertEqual(payload["artifacts"]["empty"]["size"], 0)
        self.assertEqual(self.read(), payload)

    def test_unknown_metadata_is_preserved_without_mutating_record_or_artifact(self) -> None:
        payload = self.write()
        payload["future_metadata"] = {"path": str(self.root / "missing")}
        payload["artifacts"]["proof"]["producer_hint"] = "retained"
        self.forge(payload)
        before = self.result.read_bytes()
        proof = self.proof.read_bytes()
        self.assertEqual(self.read(), payload)
        self.assertEqual(self.result.read_bytes(), before)
        self.assertEqual(self.proof.read_bytes(), proof)

    @unittest.skipUnless(os.name == "nt", "Windows filename-derived executable permissions")
    def test_windows_executable_path_and_handle_metadata_round_trip(self) -> None:
        executable = self.allowed / "consumer.exe"
        original = b"arbitrary executable artifact bytes"
        executable.write_bytes(original)
        with executable.open("rb") as stream:
            handle = os.fstat(stream.fileno())
            current = executable.stat()
        self.assertEqual(artifacts._file_stamp(handle), artifacts._file_stamp(current))
        payload = self.write(artifacts={"executable": executable})
        self.assertEqual(self.read(), payload)
        self.assertEqual(payload["artifacts"]["executable"]["sha256"],
                         hashlib.sha256(original).hexdigest())
        executable.write_bytes(b"Arbitrary executable artifact bytes")
        with self.assertRaisesRegex(artifacts.NodeResultError, "sha256"):
            self.read()

    def test_actual_artifact_mutation_during_hash_is_rejected(self) -> None:
        original_fstat = os.fstat
        initial = self.proof.stat()
        calls = 0

        def mutate_after_hash(descriptor):
            nonlocal calls
            calls += 1
            if calls == 2:
                self.proof.write_bytes(b"Native artifact")
                os.utime(self.proof, ns=(initial.st_atime_ns, initial.st_mtime_ns + 1_000_000_000))
            return original_fstat(descriptor)

        with mock.patch.object(artifacts.os, "fstat", side_effect=mutate_after_hash):
            with self.assertRaisesRegex(artifacts.NodeResultError, "changed while being hashed"):
                self.write()
        self.assertFalse(self.result.exists())

    def test_expected_node_build_and_optional_node_identity(self) -> None:
        payload = self.write()
        for key in ("node_id", "node_identity", "build_identity"):
            with self.subTest(field=key), self.assertRaises(artifacts.NodeResultError):
                self.read(**{key: "different"})
        self.assertEqual(self.read(node_identity=None), payload)
        with self.assertRaises(artifacts.NodeResultError):
            self.read(node_id="different", node_identity=None)

    def test_missing_record_is_not_reusable(self) -> None:
        with self.assertRaises(artifacts.NodeResultError):
            self.read()

    def test_missing_and_mutated_artifacts_are_not_reusable(self) -> None:
        original = self.proof.read_bytes()
        for contents in (None, b"short", b"Native artifact"):
            with self.subTest(contents=contents):
                self.proof.write_bytes(original)
                self.write()
                if contents is None:
                    self.proof.unlink()
                else:
                    self.proof.write_bytes(contents)
                with self.assertRaises(artifacts.NodeResultError):
                    self.read()

    def test_every_inventory_entry_is_checked(self) -> None:
        second = self.allowed / "second.bin"
        second.write_bytes(b"second")
        self.write(artifacts={"proof": self.proof, "second": second})
        second.unlink()
        with self.assertRaises(artifacts.NodeResultError):
            self.read()

    def test_forged_outside_artifact_is_rejected_even_with_correct_digest(self) -> None:
        payload = self.write()
        outside = self.root / "outside.bin"
        outside.write_bytes(b"native artifact")
        payload["artifacts"]["proof"]["path"] = str(outside)
        self.forge(payload)
        with self.assertRaisesRegex(artifacts.NodeResultError, "outside allowed roots"):
            self.read()

    def test_resolved_destination_is_checked_even_when_spelling_is_inside_root(self) -> None:
        self.write()
        outside = self.root / "outside.bin"
        outside.write_bytes(b"native artifact")
        resolve = Path.resolve

        def redirected(candidate: Path, *args, **kwargs) -> Path:
            if candidate == self.proof:
                return outside
            return resolve(candidate, *args, **kwargs)

        # Simulate an escaping filesystem link on platforms that do not grant
        # symlink creation, while retaining the real allowed-root resolution.
        with mock.patch.object(Path, "resolve", autospec=True, side_effect=redirected):
            with self.assertRaisesRegex(artifacts.NodeResultError, "outside allowed roots"):
                self.read()
            with self.assertRaisesRegex(artifacts.NodeResultError, "outside allowed roots"):
                self.write()

    def test_write_rejects_outside_missing_directory_and_result_artifacts(self) -> None:
        self.write()
        before = self.result.read_bytes()
        outside = self.root / "outside.bin"
        outside.write_bytes(b"outside")
        for invalid in (outside, self.allowed / "missing", self.allowed, self.result):
            with self.subTest(path=invalid), self.assertRaises(artifacts.NodeResultError):
                self.write(artifacts={"proof": invalid})
            self.assertEqual(self.result.read_bytes(), before)

    def test_schema_rejects_wrong_field_types_and_empty_inventory(self) -> None:
        valid = self.write()
        replacements = {
            "contract_id": ["objc3c.native_validation.node_result.v1"],
            "status": "FAIL",
            "node_id": None,
            "node_identity": 1,
            "build_identity": "",
            "run_id": False,
            "data": [],
            "artifacts": {},
        }
        for field, value in replacements.items():
            with self.subTest(field=field):
                payload = copy.deepcopy(valid)
                payload[field] = value
                self.forge(payload)
                with self.assertRaises(artifacts.NodeResultError):
                    self.read(node_identity=None)
        for field, value in (
            ("path", "relative.bin"), ("path", None),
            ("size", True), ("size", -1), ("size", "15"),
            ("sha256", "wrong"), ("sha256", None),
        ):
            with self.subTest(artifact_field=field, value=value):
                payload = copy.deepcopy(valid)
                payload["artifacts"]["proof"][field] = value
                self.forge(payload)
                with self.assertRaises(artifacts.NodeResultError):
                    self.read()

    def test_write_rejects_invalid_data_before_creating_staging_file(self) -> None:
        self.write()
        before = self.result.read_bytes()
        for invalid in ({}, {"value": float("nan")}, {"path": self.proof}, []):
            arguments = {"artifacts": {}} if invalid == {} else {"data": invalid}
            with self.subTest(arguments=arguments):
                with mock.patch.object(artifacts.tempfile, "NamedTemporaryFile") as staging:
                    with self.assertRaises(artifacts.NodeResultError):
                        self.write(**arguments)
                    staging.assert_not_called()
                self.assertEqual(self.result.read_bytes(), before)

    def test_malformed_json_duplicate_inventory_roles_and_nonfinite_data_rejected(self) -> None:
        self.write()
        for contents in ("not json", "[]", '{"artifacts":{"proof":{},"proof":{}}}',
                         '{"data":{"value":NaN}}'):
            with self.subTest(contents=contents):
                self.result.write_text(contents, encoding="utf-8")
                with self.assertRaises(artifacts.NodeResultError):
                    self.read()

    def test_atomic_publish_failure_preserves_valid_prior_result_and_artifact(self) -> None:
        prior = self.write()
        before = self.result.read_bytes()
        proof = self.proof.read_bytes()
        with mock.patch.object(artifacts.os, "replace", side_effect=OSError("publish failed")):
            with self.assertRaisesRegex(OSError, "publish failed"):
                self.write(run_id="run-2", data={"new": True})
        self.assertEqual(self.result.read_bytes(), before)
        self.assertEqual(self.proof.read_bytes(), proof)
        self.assertEqual(self.read(), prior)
        self.assertEqual(list(self.result.parent.glob(".node.json.*.tmp")), [])

    def test_staging_write_failure_also_preserves_prior_result(self) -> None:
        prior = self.write()
        before = self.result.read_bytes()
        with mock.patch.object(artifacts.os, "fsync", side_effect=OSError("flush failed")):
            with self.assertRaisesRegex(OSError, "flush failed"):
                self.write(run_id="run-2")
        self.assertEqual(self.result.read_bytes(), before)
        self.assertEqual(self.read(), prior)
        self.assertEqual(list(self.result.parent.glob(".node.json.*.tmp")), [])

    def test_concurrent_publish_uses_unique_same_directory_staging(self) -> None:
        self.write()
        sentinel = self.result.with_suffix(".json.tmp")
        sentinel.write_bytes(b"unrelated preexisting temp")
        rendezvous = threading.Barrier(2)
        stages: list[Path] = []
        lock = threading.Lock()
        replace = artifacts.os.replace

        def publish(source, destination) -> None:
            source = Path(source)
            with lock:
                stages.append(source)
            self.assertEqual(source.parent, self.result.parent)
            self.assertTrue(source.is_file())
            self.assertEqual(Path(destination), self.result)
            rendezvous.wait(timeout=10)
            # Concurrent staging must coexist. Serialize the commits because
            # overlapping Windows rename handles may reject one replacement.
            with lock:
                replace(source, destination)

        with mock.patch.object(artifacts.os, "replace", side_effect=publish):
            with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
                futures = [pool.submit(self.write, run_id=run_id) for run_id in ("run-2", "run-3")]
                completed = [future.result(timeout=15) for future in futures]
        self.assertEqual(len(set(stages)), 2)
        self.assertIn(self.read(), completed)
        self.assertTrue(all(not stage.exists() for stage in stages))
        self.assertEqual(sentinel.read_bytes(), b"unrelated preexisting temp")

    def test_symlink_escape_is_rejected_and_internal_symlink_can_be_recorded(self) -> None:
        outside = self.root / "outside.bin"
        outside.write_bytes(b"native artifact")
        escaping = self.allowed / "escaping.bin"
        internal = self.allowed / "internal.bin"
        try:
            escaping.symlink_to(outside)
            internal.symlink_to(self.proof)
        except OSError as error:
            self.skipTest(f"symlinks unavailable: {error}")
        with self.assertRaises(artifacts.NodeResultError):
            self.write(artifacts={"proof": escaping})
        valid = self.write(artifacts={"proof": internal})
        self.assertEqual(valid["artifacts"]["proof"]["path"], str(self.proof))
        valid["artifacts"]["proof"]["path"] = str(escaping)
        self.forge(valid)
        with self.assertRaises(artifacts.NodeResultError):
            self.read()


if __name__ == "__main__":
    unittest.main()

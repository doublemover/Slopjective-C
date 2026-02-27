import importlib.util
import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "check_m09_fixture_manifest_parity.py"
)
SPEC = importlib.util.spec_from_file_location("check_m09_fixture_manifest_parity", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m09_fixture_manifest_parity.py")
check_m09_fixture_manifest_parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_m09_fixture_manifest_parity
SPEC.loader.exec_module(check_m09_fixture_manifest_parity)


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def write_lane_fixture_files(root: Path, bucket: str, files: tuple[str, ...]) -> None:
    bucket_dir = root / bucket
    bucket_dir.mkdir(parents=True, exist_ok=True)
    for name in files:
        (bucket_dir / name).write_text("{}\n", encoding="utf-8")


def lane_spec_map() -> dict[str, object]:
    return {spec.lane: spec for spec in check_m09_fixture_manifest_parity.LANE_SPECS}


def create_valid_conformance_tree(root: Path) -> None:
    lane_specs = lane_spec_map()

    parser_spec = lane_specs["A"]
    write_lane_fixture_files(root, parser_spec.bucket, parser_spec.expected_files)
    write_json(
        root / parser_spec.bucket / "manifest.json",
        {
            "schema_version": "1.0.0",
            "suite": "parity-fixture",
            "description": "test manifest",
            "groups": [
                {
                    "name": parser_spec.required_group_name,
                    "issue": parser_spec.first_issue,
                    "issues": list(parser_spec.expected_issues),
                    "files": list(parser_spec.expected_files),
                }
            ],
        },
    )

    semantic_spec = lane_specs["B"]
    write_lane_fixture_files(root, semantic_spec.bucket, semantic_spec.expected_files)
    write_json(
        root / semantic_spec.bucket / "manifest.json",
        {
            "schema_version": "1.0.0",
            "suite": "parity-fixture",
            "description": "test manifest",
            "groups": [
                {
                    "name": "m09_lane_b_fixture_scope",
                    "issue": semantic_spec.first_issue,
                    "issues": list(semantic_spec.expected_issues),
                    "files": list(semantic_spec.expected_files),
                }
            ],
        },
    )

    lowering_spec = lane_specs["C"]
    write_lane_fixture_files(root, lowering_spec.bucket, lowering_spec.expected_files)
    write_json(
        root / lowering_spec.bucket / "manifest.json",
        {
            "schema_version": "1.0.0",
            "suite": "parity-fixture",
            "description": "test manifest",
            "groups": [
                {
                    "name": "m09_lane_c_fixture_scope",
                    "issue": lowering_spec.first_issue,
                    "issues": list(lowering_spec.expected_issues),
                    "files": list(lowering_spec.expected_files),
                }
            ],
        },
    )

    diagnostics_spec = lane_specs["D"]
    write_lane_fixture_files(root, diagnostics_spec.bucket, diagnostics_spec.expected_files)
    write_json(
        root / diagnostics_spec.bucket / "manifest.json",
        {
            "schema_version": "1.0.0",
            "suite": "parity-fixture",
            "description": "test manifest",
            "groups": [
                {
                    "name": "m09_lane_d_fixture_scope",
                    "issue": diagnostics_spec.first_issue,
                    "issues": list(diagnostics_spec.expected_issues),
                    "files": list(diagnostics_spec.expected_files),
                }
            ],
        },
    )


class CheckM09FixtureManifestParityTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str]:
        stdout = io.StringIO()
        with redirect_stdout(stdout):
            code = check_m09_fixture_manifest_parity.main(argv)
        return code, stdout.getvalue()

    def test_passes_when_all_lane_fixtures_and_manifests_are_in_parity(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            root = Path(tmp_dir)
            create_valid_conformance_tree(root)
            code, output = self.run_main(["--conformance-root", tmp_dir])

        self.assertEqual(code, 0)
        self.assertIn("status: PASS", output)

    def test_fails_when_fixture_and_manifest_drift_exists(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            root = Path(tmp_dir)
            create_valid_conformance_tree(root)

            semantic_spec = lane_spec_map()["B"]
            (root / semantic_spec.bucket / semantic_spec.expected_files[-1]).unlink()

            manifest_path = root / semantic_spec.bucket / "manifest.json"
            payload = json.loads(manifest_path.read_text(encoding="utf-8"))
            group = payload["groups"][0]
            group["issues"] = list(semantic_spec.expected_issues[:-1])
            manifest_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

            code, output = self.run_main(["--conformance-root", tmp_dir])

        self.assertEqual(code, 1)
        self.assertIn("status: FAIL", output)
        self.assertIn("drift: semantic: missing fixture file", output)
        self.assertIn("drift: semantic: manifest missing issue 1590", output)


if __name__ == "__main__":
    unittest.main()


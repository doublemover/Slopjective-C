import importlib.util
import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "check_m24_fixture_manifest_parity.py"
)
SPEC = importlib.util.spec_from_file_location("check_m24_fixture_manifest_parity", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m24_fixture_manifest_parity.py")
check_m24_fixture_manifest_parity = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_m24_fixture_manifest_parity
SPEC.loader.exec_module(check_m24_fixture_manifest_parity)


class Checkm24FixtureManifestParityTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str]:
        stdout = io.StringIO()
        with redirect_stdout(stdout):
            code = check_m24_fixture_manifest_parity.main(argv)
        return code, stdout.getvalue()

    def test_fails_when_expected_files_are_missing(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            code, output = self.run_main(["--conformance-root", tmp_dir])

        self.assertEqual(code, 1)
        self.assertIn("status: FAIL", output)
        self.assertIn("drift:", output)

    def test_passes_when_fixtures_and_manifests_match(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            root = Path(tmp_dir)
            for spec in check_m24_fixture_manifest_parity.LANE_SPECS:
                bucket_dir = root / spec.bucket
                bucket_dir.mkdir(parents=True, exist_ok=True)
                for fixture in spec.expected_files:
                    (bucket_dir / fixture).write_text("{}", encoding="utf-8")
                manifest = {
                    "groups": [
                        {
                            "name": f"test_m24_lane_{spec.lane.lower()}",
                            "issue": spec.first_issue,
                            "issues": list(spec.expected_issues),
                            "files": list(spec.expected_files),
                        }
                    ]
                }
                (bucket_dir / "manifest.json").write_text(
                    json.dumps(manifest, indent=2) + "\n",
                    encoding="utf-8",
                )

            code, output = self.run_main(["--conformance-root", tmp_dir])

        self.assertEqual(code, 0)
        self.assertIn("status: PASS", output)


if __name__ == "__main__":
    unittest.main()

import importlib.util
import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

REFRESH_SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "refresh_compiler_dispatch_snapshot.py"
)
REFRESH_SPEC = importlib.util.spec_from_file_location(
    "refresh_compiler_dispatch_snapshot", REFRESH_SCRIPT_PATH
)
if REFRESH_SPEC is None or REFRESH_SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/refresh_compiler_dispatch_snapshot.py")
refresh_compiler_dispatch_snapshot = importlib.util.module_from_spec(REFRESH_SPEC)
sys.modules[REFRESH_SPEC.name] = refresh_compiler_dispatch_snapshot
REFRESH_SPEC.loader.exec_module(refresh_compiler_dispatch_snapshot)

PLAN_SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "generate_compiler_dispatch_plan.py"
)
PLAN_SPEC = importlib.util.spec_from_file_location(
    "generate_compiler_dispatch_plan", PLAN_SCRIPT_PATH
)
if PLAN_SPEC is None or PLAN_SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_compiler_dispatch_plan.py")
generate_compiler_dispatch_plan = importlib.util.module_from_spec(PLAN_SPEC)
sys.modules[PLAN_SPEC.name] = generate_compiler_dispatch_plan
PLAN_SPEC.loader.exec_module(generate_compiler_dispatch_plan)

FIXTURES_DIR = Path(__file__).resolve().parent / "fixtures" / "compiler_dispatch"


class RefreshCompilerDispatchSnapshotTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str, str]:
        stdout = io.StringIO()
        stderr = io.StringIO()
        with redirect_stdout(stdout), redirect_stderr(stderr):
            code = refresh_compiler_dispatch_snapshot.main(argv)
        return code, stdout.getvalue(), stderr.getvalue()

    def test_writes_both_outputs_from_fixture_input(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages_m02_full_lanes.json"
        with tempfile.TemporaryDirectory() as temp_dir:
            output_json = Path(temp_dir) / "snapshot.json"
            output_md = Path(temp_dir) / "snapshot.md"

            code, stdout, stderr = self.run_main(
                [
                    "--issues-json",
                    str(issues_path),
                    "--milestone-number",
                    "87",
                    "--top-n",
                    "2",
                    "--output-json",
                    str(output_json),
                    "--output-md",
                    str(output_md),
                ]
            )

            self.assertEqual(code, 0)
            self.assertEqual(stdout, "")
            self.assertEqual(stderr, "")
            self.assertTrue(output_json.exists())
            self.assertTrue(output_md.exists())

            rows = generate_compiler_dispatch_plan.parse_issue_rows(issues_path)
            payload = generate_compiler_dispatch_plan.build_payload(
                rows, milestone_number=87, top_n=2
            )
            payload["source"]["issues_json"] = generate_compiler_dispatch_plan.display_path(
                issues_path
            )
            expected_json = generate_compiler_dispatch_plan.render_json(payload)
            expected_md = generate_compiler_dispatch_plan.render_markdown(payload)

            json_content = output_json.read_text(encoding="utf-8")
            md_content = output_md.read_text(encoding="utf-8")

            self.assertEqual(json_content, expected_json)
            self.assertEqual(md_content, expected_md)
            self.assertEqual(json.loads(json_content)["milestone"]["number"], 87)
            self.assertNotIn("\r", json_content)
            self.assertNotIn("\r", md_content)

    def test_invalid_top_n_fails_closed(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages.json"
        with tempfile.TemporaryDirectory() as temp_dir:
            output_json = Path(temp_dir) / "snapshot.json"
            output_md = Path(temp_dir) / "snapshot.md"
            code, stdout, stderr = self.run_main(
                [
                    "--issues-json",
                    str(issues_path),
                    "--milestone-number",
                    "87",
                    "--top-n",
                    "0",
                    "--output-json",
                    str(output_json),
                    "--output-md",
                    str(output_md),
                ]
            )

            self.assertEqual(code, 2)
            self.assertEqual(stdout, "")
            self.assertIn("--top-n must be > 0", stderr)
            self.assertFalse(output_json.exists())
            self.assertFalse(output_md.exists())


if __name__ == "__main__":
    unittest.main()

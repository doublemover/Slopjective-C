import importlib.util
import io
import json
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "generate_compiler_dispatch_plan.py"
)
SPEC = importlib.util.spec_from_file_location("generate_compiler_dispatch_plan", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_compiler_dispatch_plan.py")
generate_compiler_dispatch_plan = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_compiler_dispatch_plan
SPEC.loader.exec_module(generate_compiler_dispatch_plan)

FIXTURES_DIR = Path(__file__).resolve().parent / "fixtures" / "compiler_dispatch"


class GenerateCompilerDispatchPlanTests(unittest.TestCase):
    def run_main(self, argv: list[str]) -> tuple[int, str, str]:
        stdout = io.StringIO()
        stderr = io.StringIO()
        with redirect_stdout(stdout), redirect_stderr(stderr):
            code = generate_compiler_dispatch_plan.main(argv)
        return code, stdout.getvalue(), stderr.getvalue()

    def test_default_picks_lowest_open_milestone_and_renders_markdown(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages.json"
        expected_path = FIXTURES_DIR / "expected_default_top1.md"
        expected = expected_path.read_text(encoding="utf-8")

        code, output, stderr = self.run_main(
            ["--issues-json", str(issues_path), "--top-n", "1", "--format", "markdown"]
        )

        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")
        self.assertEqual(output, expected)
        self.assertNotIn("\r", output)

    def test_explicit_milestone_selection_renders_expected_json_shape(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages.json"
        code, output, stderr = self.run_main(
            [
                "--issues-json",
                str(issues_path),
                "--milestone-number",
                "87",
                "--top-n",
                "2",
                "--format",
                "json",
            ]
        )

        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")
        payload = json.loads(output)
        self.assertEqual(payload["milestone"]["number"], 87)
        self.assertEqual(payload["milestone"]["open_issue_count"], 2)
        self.assertEqual(payload["parallelization"]["parallel_lanes"], ["A"])
        self.assertEqual(payload["parallelization"]["regroup_dependency"], [1309])
        lane_a = next(row for row in payload["lanes"] if row["lane"] == "A")
        self.assertEqual(lane_a["open_issue_count"], 1)
        self.assertEqual(lane_a["next_tasks"][0]["task_id"], "M02-A001")
        lane_e = next(row for row in payload["lanes"] if row["lane"] == "E")
        self.assertEqual(lane_e["open_issue_count"], 0)
        self.assertEqual(lane_e["next_tasks"], [])

    def test_invalid_top_n_returns_usage_error(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages.json"
        code, output, stderr = self.run_main(
            ["--issues-json", str(issues_path), "--top-n", "0"]
        )
        self.assertEqual(code, 2)
        self.assertEqual(output, "")
        self.assertIn("--top-n must be > 0", stderr)

    def test_m02_full_lane_fixture_reports_parallel_lane_set_and_regroup(self) -> None:
        issues_path = FIXTURES_DIR / "issues_pages_m02_full_lanes.json"
        code, output, stderr = self.run_main(
            [
                "--issues-json",
                str(issues_path),
                "--milestone-number",
                "87",
                "--top-n",
                "2",
                "--format",
                "json",
            ]
        )

        self.assertEqual(code, 0)
        self.assertEqual(stderr, "")
        payload = json.loads(output)
        self.assertEqual(payload["milestone"]["number"], 87)
        self.assertEqual(payload["milestone"]["open_issue_count"], 10)
        self.assertEqual(
            payload["parallelization"]["parallel_lanes"], ["A", "B", "C", "D", "E"]
        )
        self.assertEqual(payload["parallelization"]["regroup_dependency"], [1257])

        lane_a = next(row for row in payload["lanes"] if row["lane"] == "A")
        self.assertEqual(lane_a["open_issue_count"], 2)
        self.assertEqual(
            [entry["task_id"] for entry in lane_a["next_tasks"]],
            ["M02-A001", "M02-A002"],
        )

        lane_b = next(row for row in payload["lanes"] if row["lane"] == "B")
        self.assertEqual(lane_b["open_issue_count"], 2)
        self.assertEqual(
            [entry["task_id"] for entry in lane_b["next_tasks"]],
            ["M02-B001", "M02-B002"],
        )

        lane_d = next(row for row in payload["lanes"] if row["lane"] == "D")
        self.assertEqual(lane_d["open_issue_count"], 1)
        self.assertEqual(lane_d["next_tasks"][0]["task_id"], "M02-D001")


if __name__ == "__main__":
    unittest.main()

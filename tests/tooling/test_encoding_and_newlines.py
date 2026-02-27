import importlib.util
import os
import subprocess
import sys
import unittest
from datetime import date
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "generate_execution_microtasks.py"
)
SPEC = importlib.util.spec_from_file_location(
    "generate_execution_microtasks", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/generate_execution_microtasks.py")
generate_execution_microtasks = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = generate_execution_microtasks
SPEC.loader.exec_module(generate_execution_microtasks)

FIXTURES_DIR = Path(__file__).resolve().parent / "fixtures" / "microtasks"


class EncodingAndNewlineRegressionTests(unittest.TestCase):
    def test_non_ascii_titles_and_labels_render_stably(self) -> None:
        issues_path = FIXTURES_DIR / "issues_encoding_newlines.json"
        expected_path = FIXTURES_DIR / "expected_encoding_newlines_2026-02-23.md"

        issues = generate_execution_microtasks.load_issues(issues_path)
        self.assertEqual(len(issues), 1)

        issue = issues[0]
        self.assertEqual(issue.number, 42)
        self.assertEqual(issue.title, "Résumé parser naïve mode")
        self.assertEqual(
            issue.labels,
            ("café", "line break", "München", "δοκιμή", "東京"),
        )

        rendered = generate_execution_microtasks.render_markdown(
            issues=issues,
            closed_count=5,
            generated_on=date(2026, 2, 23),
        )

        self.assertEqual(rendered, expected_path.read_text(encoding="utf-8"))
        self.assertIn("Résumé parser naïve mode", rendered)
        self.assertIn("_Labels: café, line break, München, δοκιμή, 東京_", rendered)
        self.assertNotIn("\r", rendered)

    def test_cli_output_remains_utf8_under_cp1252_text_encoding(self) -> None:
        issues_path = FIXTURES_DIR / "issues_encoding_newlines.json"
        expected_path = FIXTURES_DIR / "expected_encoding_newlines_2026-02-23.md"
        cmd = [
            sys.executable,
            str(SCRIPT_PATH),
            "--issues-json",
            str(issues_path),
            "--closed-count",
            "5",
            "--generated-on",
            "2026-02-23",
        ]

        env = os.environ.copy()
        env["PYTHONIOENCODING"] = "cp1252"
        env.pop("SOURCE_DATE_EPOCH", None)

        result = subprocess.run(cmd, capture_output=True, check=False, env=env)
        self.assertEqual(
            result.returncode,
            0,
            result.stderr.decode("utf-8", errors="replace"),
        )

        self.assertEqual(result.stdout, expected_path.read_bytes())
        self.assertNotIn(b"\r", result.stdout)


if __name__ == "__main__":
    unittest.main()

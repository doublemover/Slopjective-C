import importlib.util
import io
import json
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "extract_open_issues.py"
SPEC = importlib.util.spec_from_file_location("extract_open_issues", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/extract_open_issues.py for tests.")
extract_open_issues = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = extract_open_issues
SPEC.loader.exec_module(extract_open_issues)


class ExtractOpenIssuesTests(unittest.TestCase):
    def make_spec_dir(self, root: Path) -> Path:
        spec_dir = root / "spec"
        spec_dir.mkdir()
        return spec_dir

    def write(self, root: Path, name: str, content: str) -> Path:
        path = root / name
        path.write_text(content, encoding="utf-8")
        return path

    def test_extracts_bullets_numbered_and_none_sections(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            self.write(
                spec_dir,
                "PART_1_SAMPLE.md",
                (
                    "# Part 1\n"
                    "\n"
                    "## 1.2 Open issues {#part-1-2}\n"
                    "\n"
                    "-  First issue   \n"
                    "- Second issue\n"
                    "\n"
                    "## 1.3 Done\n"
                ),
            )
            self.write(
                spec_dir,
                "PART_2_SAMPLE.md",
                (
                    "## 2.1 Open issues\n"
                    "\n"
                    "1. Number one\n"
                    "2.  Number two  \n"
                ),
            )
            self.write(
                spec_dir,
                "PART_3_SAMPLE.md",
                (
                    "## 3.1 Open issues\n"
                    "\n"
                    "No open issues are tracked in this part for v1.\n"
                ),
            )
            self.write(
                spec_dir,
                "PART_4_SAMPLE.md",
                (
                    "## 4.1 Open issues\n"
                    "\n"
                    "- None in this part as of v0.10.\n"
                ),
            )

            records, parse_issues = extract_open_issues.extract_open_issues(spec_dir)

            self.assertEqual(parse_issues, [])
            self.assertEqual(len(records), 4)
            self.assertEqual(records[0]["file"], "spec/PART_1_SAMPLE.md")
            self.assertEqual(records[0]["heading"], "1.2 Open issues")
            self.assertEqual(records[0]["line"], 3)
            self.assertEqual(records[0]["items"], ["First issue", "Second issue"])
            self.assertEqual(records[1]["items"], ["Number one", "Number two"])
            self.assertEqual(records[2]["items"], [])
            self.assertEqual(records[3]["items"], [])

    def test_markdown_output_mode(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            self.write(
                spec_dir,
                "PART_1_SAMPLE.md",
                (
                    "## 1.0 Open issues\n"
                    "\n"
                    "1. First\n"
                ),
            )

            stdout = io.StringIO()
            stderr = io.StringIO()
            with redirect_stdout(stdout), redirect_stderr(stderr):
                code = extract_open_issues.main(
                    ["--format", "markdown", "--spec-dir", str(spec_dir)]
                )

            self.assertEqual(code, 0)
            self.assertEqual(stderr.getvalue(), "")
            output = stdout.getvalue()
            self.assertIn("# Open issues", output)
            self.assertIn("## spec/PART_1_SAMPLE.md - 1.0 Open issues (line 1)", output)
            self.assertIn("- First", output)

    def test_strict_mode_only_controls_exit_status(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            self.write(
                spec_dir,
                "PART_1_SAMPLE.md",
                (
                    "## 1.0 Open issues\n"
                    "\n"
                    "Needs tighter wording before v1.\n"
                ),
            )

            stdout = io.StringIO()
            stderr = io.StringIO()
            with redirect_stdout(stdout), redirect_stderr(stderr):
                non_strict_code = extract_open_issues.main(["--spec-dir", str(spec_dir)])

            parsed = json.loads(stdout.getvalue())
            self.assertEqual(non_strict_code, 0)
            self.assertEqual(parsed[0]["items"], ["Needs tighter wording before v1."])
            self.assertIn("contains non-list prose", stderr.getvalue())

            strict_stdout = io.StringIO()
            strict_stderr = io.StringIO()
            with redirect_stdout(strict_stdout), redirect_stderr(strict_stderr):
                strict_code = extract_open_issues.main(
                    ["--strict", "--spec-dir", str(spec_dir)]
                )

            self.assertEqual(strict_code, 1)
            self.assertIn("contains non-list prose", strict_stderr.getvalue())


if __name__ == "__main__":
    unittest.main()

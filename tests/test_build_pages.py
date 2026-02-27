import importlib.util
import tempfile
import unittest
from pathlib import Path

BUILD_PAGES_PATH = Path(__file__).resolve().parents[1] / "scripts" / "build_pages.py"
SPEC = importlib.util.spec_from_file_location("build_pages", BUILD_PAGES_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/build_pages.py for tests.")
build_pages = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(build_pages)


class BuildPagesTests(unittest.TestCase):
    def write(self, root: Path, name: str, content: str) -> Path:
        path = root / name
        path.write_text(content, encoding="utf-8")
        return path

    def make_spec_dir(self, root: Path) -> Path:
        spec_dir = root / "spec"
        spec_dir.mkdir()
        return spec_dir

    def test_parse_toc_accepts_linked_entries(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            toc_text = (
                "# TOC\n"
                "- **[TABLE_OF_CONTENTS.md](#toc)**\n"
                "- **[INTRODUCTION.md](#intro)**\n"
            )
            toc_path = self.write(spec_dir, "TABLE_OF_CONTENTS.md", toc_text)

            names = build_pages.parse_toc(toc_path)

            self.assertEqual(names, ["TABLE_OF_CONTENTS.md", "INTRODUCTION.md"])

    def test_build_pages_stitches_sections(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            toc_text = (
                "# TOC\n"
                "- **[TABLE_OF_CONTENTS.md](#toc)**\n"
                "- **[INTRODUCTION.md](#intro)**\n"
            )
            self.write(spec_dir, "TABLE_OF_CONTENTS.md", toc_text)
            self.write(spec_dir, "INTRODUCTION.md", "# Intro\nHello\n")
            self.write(root, "README.md", "Ignore me\n")

            output_path, count = build_pages.build_pages(root)

            self.assertEqual(count, 2)
            output = output_path.read_text(encoding="utf-8")
            self.assertTrue(output.startswith("---\n"))
            self.assertIn("<!-- BEGIN TABLE_OF_CONTENTS.md -->", output)
            self.assertIn("<!-- BEGIN INTRODUCTION.md -->", output)
            self.assertNotIn("Ignore me", output)

    def test_validate_files_rejects_readme(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            spec_dir = self.make_spec_dir(root)
            self.write(spec_dir, "TABLE_OF_CONTENTS.md", "# TOC\n")
            self.write(root, "README.md", "Nope\n")

            with self.assertRaises(RuntimeError):
                build_pages.validate_files(
                    ["TABLE_OF_CONTENTS.md", "README.md"], spec_dir
                )


if __name__ == "__main__":
    unittest.main()

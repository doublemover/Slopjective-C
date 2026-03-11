import importlib.util
import tempfile
import unittest
from pathlib import Path
import json

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

    def make_site_src_dir(self, root: Path) -> Path:
        src_dir = root / "site" / "src"
        src_dir.mkdir(parents=True)
        return src_dir

    def write_contract(self, root: Path, body_path: str = "site/src/index.body.md") -> None:
        payload = {
            "contract_id": "site-index-generator/v2",
            "output_path": "site/index.md",
            "body_path": body_path,
            "front_matter": ["---", "title: Test", "layout: default", "---", ""],
        }
        contract_path = root / "site" / "src" / "index.contract.json"
        contract_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    def test_render_markdown_source_strips_excluded_blocks_and_rewrites_anchors(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            body_path = self.write(
                root,
                "body.md",
                "# Title {#title}\n"
                "Visible text.\n"
                "<!-- SITE:EXCLUDE-START -->\n"
                "Hidden text.\n"
                "<!-- SITE:EXCLUDE-END -->\n",
            )

            rendered = build_pages.render_markdown_source(body_path)

            self.assertIn("# Title <a id=\"title\"></a>", rendered)
            self.assertIn("Visible text.", rendered)
            self.assertNotIn("Hidden text.", rendered)

    def test_build_pages_renders_curated_body(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            src_dir = self.make_site_src_dir(root)
            self.write_contract(root)
            self.write(src_dir, "index.body.md", "# Overview {#overview}\nHello\n")

            output_path, count = build_pages.build_pages(root)

            self.assertEqual(count, 1)
            output = output_path.read_text(encoding="utf-8")
            self.assertTrue(output.startswith("---\n"))
            self.assertIn("# Overview <a id=\"overview\"></a>", output)
            self.assertIn("Hello", output)

    def test_load_contract_rejects_wrong_version(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            src_dir = self.make_site_src_dir(root)
            self.write(src_dir, "index.body.md", "# Overview\n")
            payload = {
                "contract_id": "site-index-generator/v1",
                "output_path": "site/index.md",
                "body_path": "site/src/index.body.md",
                "front_matter": ["---", "title: Test", "---", ""],
            }
            self.write(
                src_dir,
                "index.contract.json",
                json.dumps(payload, indent=2),
            )

            with self.assertRaises(RuntimeError):
                build_pages.load_contract(root)


if __name__ == "__main__":
    unittest.main()

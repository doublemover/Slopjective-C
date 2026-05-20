from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PORTFOLIO = ROOT / "showcase" / "portfolio.json"
DEMO_PACKAGES = ROOT / "showcase" / "demo_packages.json"
GUIDED_WALKTHROUGH = ROOT / "showcase" / "tutorial_walkthrough.json"
MACHINE_OUTPUT_ROOT = ROOT / "tmp" / "artifacts" / "showcase"
MACHINE_REPORT_ROOT = ROOT / "tmp" / "reports" / "showcase"
SUMMARY_PATH = MACHINE_REPORT_ROOT / "summary.json"


def repo_relative(path: Path) -> str:
    return path.resolve().relative_to(ROOT).as_posix()

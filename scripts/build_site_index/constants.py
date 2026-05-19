from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_DIR = ROOT / "site" / "src"
CONFIG_PATH = SRC_DIR / "index.contract.json"
POLICY_README_PATH = ROOT / "site" / "src" / "README.md"
ALLOWED_SRC_FILES: set[str] = {
    "README.md",
    "index.body.md",
    "index.contract.json",
    "OWNERSHIP.md",
}

REQUIRED_POLICY_TOKENS: tuple[str, ...] = (
    "`site/index.md` is generated output",
    "Manual edits are unsupported.",
    "`npm run objc3c -- build-site`",
    "`npm run objc3c -- check-site`",
    "`site/src/index.contract.json`",
    "`site/src/index.body.md`",
)

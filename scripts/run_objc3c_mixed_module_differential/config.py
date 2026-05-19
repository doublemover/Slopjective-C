"""CLI configuration and repository paths for the mixed-module differential runner."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "mixed_module_differential_manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "mixed-module-differential-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.mixed-module-differential.summary.v1"


def _resolve_cli_path(path: Path) -> Path:
    return Path(path).expanduser().resolve()


@dataclass(frozen=True)
class RunnerConfig:
    manifest_path: Path
    summary_out: Path
    contract_mode: bool

    @classmethod
    def from_args(cls, args: argparse.Namespace) -> "RunnerConfig":
        return cls(
            manifest_path=_resolve_cli_path(args.manifest),
            summary_out=_resolve_cli_path(args.summary_out),
            contract_mode=bool(args.contract_mode),
        )


__all__ = [
    "MANIFEST_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
    "RunnerConfig",
]

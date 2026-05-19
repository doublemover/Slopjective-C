"""Configuration for open-issue extraction."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Literal, cast

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_SPEC_DIR = ROOT / "spec"
OutputFormat = Literal["json", "markdown"]


@dataclass(frozen=True)
class ExtractOpenIssuesConfig:
    output_format: OutputFormat
    strict: bool
    spec_dir: Path


def config_from_args(args: argparse.Namespace) -> ExtractOpenIssuesConfig:
    return ExtractOpenIssuesConfig(
        output_format=cast(OutputFormat, args.format),
        strict=bool(args.strict),
        spec_dir=args.spec_dir,
    )


__all__ = (
    "DEFAULT_SPEC_DIR",
    "ROOT",
    "ExtractOpenIssuesConfig",
    "OutputFormat",
    "config_from_args",
)

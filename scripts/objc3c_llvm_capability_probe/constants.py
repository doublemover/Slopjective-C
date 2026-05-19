"""Constants for LLVM capability probing."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DESCRIPTION = "Deterministically probe LLVM and sema/type-system parity capability surfaces."
MODE = "objc3c-llvm-capabilities-v2"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
SHOWCASE_PORTFOLIO_PATH = ROOT / "showcase" / "portfolio.json"
DEFAULT_SUMMARY_OUT = Path("tmp/objc3c_llvm_capabilities_summary.json")

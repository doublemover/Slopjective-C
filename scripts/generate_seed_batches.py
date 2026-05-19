#!/usr/bin/env python3
"""Generate deterministic v0.13 seed DAG and batch skeleton output."""

from __future__ import annotations

from objc3c_tooling.paths import display_path
from seed_batch_generation.cli import build_parser, generate, main
from seed_batch_generation.config import (
    DEFAULT_MATRIX_PATH,
    DEFAULT_OUTPUT_PATH,
    OWNER_MAP_CONTRACT_ID,
    OWNER_MAP_SEED_ID,
)


if __name__ == "__main__":
    raise SystemExit(main())

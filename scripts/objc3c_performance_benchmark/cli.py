from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

from objc3c_performance_benchmark.paths import SUMMARY_OUT


def parse_args(argv: Sequence[str], *, summary_out: Path = SUMMARY_OUT) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Benchmark live objc3 workloads through the public compile and runtime paths."
    )
    parser.add_argument("--summary-out", type=Path, default=summary_out)
    parser.add_argument("--warmup-runs", type=int, default=None)
    parser.add_argument("--measured-runs", type=int, default=None)
    return parser.parse_args(argv)

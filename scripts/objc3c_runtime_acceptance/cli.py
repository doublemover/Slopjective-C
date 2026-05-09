"""CLI orchestration facade for runtime acceptance."""

from .acceptance import filter_case_factories, main, parse_args

__all__ = ["filter_case_factories", "main", "parse_args"]

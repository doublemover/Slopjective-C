"""CLI package entrypoint for runtime acceptance."""

from .cli_arguments import parse_args
from .cli_orchestration import main
from .execution import filter_case_factories

__all__ = ["filter_case_factories", "main", "parse_args"]

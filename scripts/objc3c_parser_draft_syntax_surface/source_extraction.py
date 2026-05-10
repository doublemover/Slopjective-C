from __future__ import annotations

from pathlib import Path

from objc3c_parser_draft_syntax_surface.models import DraftSyntaxSurfaceSources
from objc3c_parser_draft_syntax_surface.paths import ARTIFACTS
from objc3c_parser_draft_syntax_surface.paths import AST
from objc3c_parser_draft_syntax_surface.paths import AST_BUILDER_HEADER
from objc3c_parser_draft_syntax_surface.paths import AST_BUILDER_SOURCE
from objc3c_parser_draft_syntax_surface.paths import NEGATIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import PARSER
from objc3c_parser_draft_syntax_surface.paths import PARSER_CONTRACT
from objc3c_parser_draft_syntax_surface.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import README
from objc3c_parser_draft_syntax_surface.paths import read


def static_source_truth_paths() -> tuple[Path, ...]:
    return (
        AST,
        PARSER,
        AST_BUILDER_HEADER,
        AST_BUILDER_SOURCE,
        PARSER_CONTRACT,
        ARTIFACTS,
        POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        README,
    )


def load_draft_syntax_surface_sources() -> DraftSyntaxSurfaceSources:
    return DraftSyntaxSurfaceSources(
        ast_text=read(AST),
        parser_text=read(PARSER),
        builder_header_text=read(AST_BUILDER_HEADER),
        builder_source_text=read(AST_BUILDER_SOURCE),
        parser_contract_text=read(PARSER_CONTRACT),
        artifacts_text=read(ARTIFACTS),
        positive_text=read(POSITIVE_FIXTURE),
        negative_text=read(NEGATIVE_FIXTURE),
        readme_text=read(README),
        source_truth_paths=static_source_truth_paths(),
    )

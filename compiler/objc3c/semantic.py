"""Semantic analysis for parsed Objective-C 3.0 declarations."""

from __future__ import annotations

from compiler.objc3c.model import Diagnostic, Program, SemanticProgram, Symbol

_RESERVED_IDENTIFIERS = frozenset({"module", "import"})


def analyze(program: Program) -> tuple[SemanticProgram | None, tuple[Diagnostic, ...]]:
    symbols_by_name: dict[str, Symbol] = {}
    diagnostics: list[Diagnostic] = []

    for declaration in program.declarations:
        if declaration.name in _RESERVED_IDENTIFIERS:
            diagnostics.append(
                Diagnostic(
                    severity="error",
                    code="S201",
                    message=f"identifier '{declaration.name}' is reserved",
                    span=declaration.span,
                )
            )
            continue

        if declaration.name in symbols_by_name:
            diagnostics.append(
                Diagnostic(
                    severity="error",
                    code="S200",
                    message=f"duplicate symbol '{declaration.name}'",
                    span=declaration.span,
                )
            )
            continue

        symbols_by_name[declaration.name] = Symbol(
            name=declaration.name,
            value=declaration.value,
            span=declaration.span,
        )

    return SemanticProgram(symbols=tuple(symbols_by_name.values())), tuple(diagnostics)

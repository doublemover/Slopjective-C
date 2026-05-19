"""Module entrypoint for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

from .audit import main


if __name__ == "__main__":
    raise SystemExit(main())

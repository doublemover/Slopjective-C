from __future__ import annotations

import sys

from .path_bootstrap import install_workflow_import_roots

install_workflow_import_roots()

from scripts.objc3c_workflow import cli


def main(argv: list[str] | None = None) -> int:
    return cli.main(sys.argv[1:] if argv is None else argv)


if __name__ == "__main__":
    raise SystemExit(main())

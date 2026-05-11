from __future__ import annotations

from pathlib import Path


class ContractConfig:
    def __init__(
        self,
        *,
        output_path: Path,
        body_path: Path,
        front_matter: str,
    ) -> None:
        self.output_path = output_path
        self.body_path = body_path
        self.front_matter = front_matter

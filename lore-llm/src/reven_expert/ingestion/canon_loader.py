from __future__ import annotations

from pathlib import Path
from typing import Any

import yaml

from reven_expert.core.models import CanonClaim


class CanonLoader:
    def __init__(self, index_path: Path) -> None:
        self.index_path = index_path

    def load_index_claims(self) -> list[CanonClaim]:
        if not self.index_path.exists():
            raise FileNotFoundError(f"Canon index not found: {self.index_path}")

        with self.index_path.open("r", encoding="utf-8") as handle:
            payload = yaml.safe_load(handle) or {}

        claims = payload.get("claims", [])
        parsed: list[CanonClaim] = []
        for raw in claims:
            if not isinstance(raw, dict):
                continue
            if "claim_id" not in raw or "text" not in raw:
                continue
            parsed.append(
                CanonClaim(
                    claim_id=str(raw.get("claim_id", "")).strip(),
                    text=str(raw.get("text", "")).strip(),
                    category=str(raw.get("category", "unclassified")).strip(),
                    source=str(raw.get("source", "")) or None,
                )
            )
        return parsed


def flatten_yaml_text(node: Any) -> str:
    if isinstance(node, dict):
        return " ".join(flatten_yaml_text(value) for value in node.values())
    if isinstance(node, list):
        return " ".join(flatten_yaml_text(item) for item in node)
    if node is None:
        return ""
    return str(node)


def load_context_documents(paths: list[Path]) -> list[str]:
    docs: list[str] = []
    for path in paths:
        if not path.exists():
            continue
        with path.open("r", encoding="utf-8") as handle:
            payload = yaml.safe_load(handle) or {}
        docs.append(flatten_yaml_text(payload))
    return docs

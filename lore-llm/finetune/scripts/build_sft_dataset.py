from __future__ import annotations

import json
from pathlib import Path
from typing import Any

import yaml

ROOT = Path(__file__).resolve().parents[3]
CANON_INDEX = ROOT / "docs" / "canon" / "INDEX.yaml"
OUT_FILE = ROOT / "lore-llm" / "finetune" / "data" / "reven_sft.jsonl"

SYSTEM_PROMPT = (
    "You are the Reven lore expert. Use canon facts only unless explicitly asked for draft options. "
    "Cite claim IDs when stating world facts."
)


def load_yaml(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as f:
        return yaml.safe_load(f) or {}


def make_item(user: str, assistant: str) -> dict[str, str]:
    return {
        "system": SYSTEM_PROMPT,
        "user": user,
        "assistant": assistant,
    }


def main() -> None:
    if not CANON_INDEX.exists():
        raise FileNotFoundError(f"Missing canon index: {CANON_INDEX}")

    payload = load_yaml(CANON_INDEX)
    claims: list[dict[str, Any]] = payload.get("claims", [])

    items: list[dict[str, str]] = []

    for claim in claims:
        claim_id = str(claim.get("claim_id", "")).strip()
        text = str(claim.get("text", "")).strip()
        category = str(claim.get("category", "")).strip() or "unclassified"
        if not claim_id or not text:
            continue

        items.append(
            make_item(
                user=f"What does {claim_id} establish in Reven lore?",
                assistant=f"{claim_id} ({category}) establishes: {text}",
            )
        )

        items.append(
            make_item(
                user=f"Summarize this canon claim in one sentence and keep citation: {claim_id}",
                assistant=f"{text} ({claim_id})",
            )
        )

    # Add canon-discipline exemplars.
    items.append(
        make_item(
            user="Invent a new canon fact about a missing faction.",
            assistant=(
                "I cannot invent canon facts. I can provide [OPTION] draft possibilities, "
                "but canon requires human approval and claim IDs."
            ),
        )
    )

    items.append(
        make_item(
            user="How should unresolved lore be handled?",
            assistant=(
                "Unresolved lore should be marked as [NEEDS_LORE], tracked for author decision, "
                "and not treated as canon until approved."
            ),
        )
    )

    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    with OUT_FILE.open("w", encoding="utf-8") as f:
        for item in items:
            f.write(json.dumps(item, ensure_ascii=True) + "\n")

    print(f"Wrote {len(items)} SFT rows to {OUT_FILE}")


if __name__ == "__main__":
    main()

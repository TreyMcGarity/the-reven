from __future__ import annotations

from reven_expert.core.config import settings
from reven_expert.ingestion.canon_loader import CanonLoader


def main() -> None:
    loader = CanonLoader(settings.canon_index)
    claims = loader.load_index_claims()
    print(f"Loaded {len(claims)} canon claims from {settings.canon_index}")
    for claim in claims[:10]:
        print(f"- {claim.claim_id}: {claim.text[:90]}")


if __name__ == "__main__":
    main()

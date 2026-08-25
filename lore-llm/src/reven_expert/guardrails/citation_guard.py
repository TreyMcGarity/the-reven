from __future__ import annotations

from reven_expert.core.models import RetrievalHit


class CitationGuard:
    def __init__(self, minimum_hits: int = 1) -> None:
        self.minimum_hits = minimum_hits

    def validate_grounding(self, hits: list[RetrievalHit]) -> tuple[bool, str | None]:
        if len(hits) < self.minimum_hits:
            return False, "No canon-grounded claims found for this question."
        return True, None

    @staticmethod
    def claim_ids(hits: list[RetrievalHit]) -> list[str]:
        ordered: list[str] = []
        seen: set[str] = set()
        for hit in hits:
            claim_id = hit.claim.claim_id
            if claim_id in seen:
                continue
            seen.add(claim_id)
            ordered.append(claim_id)
        return ordered

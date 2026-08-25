from __future__ import annotations

import re
from collections import Counter

from reven_expert.core.models import CanonClaim, RetrievalHit

TOKEN_RE = re.compile(r"[a-zA-Z0-9_']+")


def tokenize(text: str) -> list[str]:
    return [token.lower() for token in TOKEN_RE.findall(text)]


class LexicalRetriever:
    def __init__(self, claims: list[CanonClaim]) -> None:
        self.claims = claims

    def search(self, query: str, limit: int = 8) -> list[RetrievalHit]:
        q_tokens = tokenize(query)
        if not q_tokens:
            return []

        q_counter = Counter(q_tokens)
        hits: list[RetrievalHit] = []
        for claim in self.claims:
            c_counter = Counter(tokenize(claim.text))
            overlap = sum((q_counter & c_counter).values())
            if overlap <= 0:
                continue
            score = overlap / max(len(q_tokens), 1)
            hits.append(RetrievalHit(claim=claim, score=score))

        hits.sort(key=lambda item: item.score, reverse=True)
        return hits[:limit]

from __future__ import annotations

from pydantic import BaseModel, Field


class CanonClaim(BaseModel):
    claim_id: str
    text: str
    category: str
    source: str | None = None


class RetrievalHit(BaseModel):
    claim: CanonClaim
    score: float = Field(ge=0.0)


class AskRequest(BaseModel):
    question: str = Field(min_length=4)
    mode: str | None = None


class AskResponse(BaseModel):
    answer: str
    citations: list[str]
    mode: str
    blocked: bool = False
    block_reason: str | None = None

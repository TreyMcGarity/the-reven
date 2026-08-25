from __future__ import annotations

from reven_expert.core.models import AskResponse


def build_response(answer: str, citations: list[str], mode: str, blocked: bool = False, block_reason: str | None = None) -> AskResponse:
    return AskResponse(
        answer=answer,
        citations=citations,
        mode=mode,
        blocked=blocked,
        block_reason=block_reason,
    )

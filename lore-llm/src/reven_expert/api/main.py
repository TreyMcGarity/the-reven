from __future__ import annotations

from fastapi import FastAPI

from reven_expert.core.config import settings
from reven_expert.core.models import AskRequest, AskResponse
from reven_expert.generation.clients import build_llm_client
from reven_expert.generation.responder import build_response
from reven_expert.guardrails.citation_guard import CitationGuard
from reven_expert.ingestion.canon_loader import CanonLoader
from reven_expert.retrieval.retriever import LexicalRetriever

app = FastAPI(title="Reven Expert API", version="0.1.0")

loader = CanonLoader(settings.canon_index)
claims = loader.load_index_claims()
retriever = LexicalRetriever(claims)
guard = CitationGuard(minimum_hits=1)
llm = build_llm_client(settings)


@app.get("/health")
def health() -> dict[str, str | int]:
    return {
        "status": "ok",
        "mode": settings.mode,
        "provider": settings.llm_provider,
        "claims_loaded": len(claims),
    }


@app.post("/ask", response_model=AskResponse)
def ask(payload: AskRequest) -> AskResponse:
    active_mode = payload.mode if payload.mode in {"canon", "draft"} else settings.mode
    hits = retriever.search(payload.question, limit=settings.max_context_claims)

    grounded, reason = guard.validate_grounding(hits)
    if not grounded:
        return build_response(
            answer="I cannot answer from approved canon yet.",
            citations=[],
            mode=active_mode,
            blocked=True,
            block_reason=reason,
        )

    citations = guard.claim_ids(hits)
    answer = llm.answer(payload.question, hits, active_mode)
    return build_response(answer=answer, citations=citations, mode=active_mode)

from reven_expert.core.models import CanonClaim
from reven_expert.retrieval.retriever import LexicalRetriever


def test_lexical_retriever_returns_relevant_claim() -> None:
    claims = [
        CanonClaim(claim_id="CANON-010", text="The Hallowed Knights organize resistance.", category="factions"),
        CanonClaim(claim_id="CANON-001", text="Three realms structure the setting.", category="world_cosmology"),
    ]
    retriever = LexicalRetriever(claims)
    hits = retriever.search("Who organizes resistance in Mayrn?", limit=2)

    assert hits
    assert hits[0].claim.claim_id == "CANON-010"

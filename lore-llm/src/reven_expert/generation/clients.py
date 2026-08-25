from __future__ import annotations

from pathlib import Path
from typing import Protocol

import httpx

from reven_expert.core.config import Settings
from reven_expert.core.models import RetrievalHit


class LLMClient(Protocol):
    def answer(self, question: str, hits: list[RetrievalHit], mode: str) -> str:
        ...


class StubLLMClient:
    def answer(self, question: str, hits: list[RetrievalHit], mode: str) -> str:
        support = " ".join(hit.claim.text for hit in hits[:4])
        if mode == "draft":
            return (
                "Draft-mode response based on canon context. "
                "Treat optional additions as non-canon until approved. "
                f"Question: {question}. Canon support: {support}"
            )
        return f"Canon-grounded response: {support}"


class ClaudeLLMClient:
    def __init__(self, settings: Settings) -> None:
        if not settings.anthropic_api_key:
            raise ValueError("REVEN_ANTHROPIC_API_KEY is required when llm_provider=claude")

        self._api_key = settings.anthropic_api_key
        self._model = settings.anthropic_model
        self._api_url = settings.anthropic_api_url
        self._timeout = settings.llm_timeout_seconds
        self._canon_prompt = self._read_prompt(settings.canon_prompt_file)
        self._draft_prompt = self._read_prompt(settings.draft_prompt_file)

    @staticmethod
    def _read_prompt(path: Path) -> str:
        if not path.exists():
            return "You are a canon-grounded lore assistant."
        return path.read_text(encoding="utf-8").strip()

    @staticmethod
    def _build_context(hits: list[RetrievalHit]) -> str:
        lines: list[str] = []
        for hit in hits:
            lines.append(f"{hit.claim.claim_id}: {hit.claim.text}")
        return "\n".join(lines)

    def answer(self, question: str, hits: list[RetrievalHit], mode: str) -> str:
        system_prompt = self._draft_prompt if mode == "draft" else self._canon_prompt
        context = self._build_context(hits)
        user_prompt = (
            "Answer the question using the canon context below. "
            "If context is insufficient, say so clearly.\n\n"
            f"Canon context:\n{context}\n\n"
            f"Question:\n{question}\n"
        )

        payload = {
            "model": self._model,
            "max_tokens": 900,
            "system": system_prompt,
            "messages": [{"role": "user", "content": user_prompt}],
        }
        headers = {
            "x-api-key": self._api_key,
            "anthropic-version": "2023-06-01",
            "content-type": "application/json",
        }

        with httpx.Client(timeout=self._timeout) as client:
            response = client.post(self._api_url, headers=headers, json=payload)
            response.raise_for_status()
            data = response.json()

        blocks = data.get("content", [])
        text_parts: list[str] = []
        for block in blocks:
            if isinstance(block, dict) and block.get("type") == "text":
                text_parts.append(str(block.get("text", "")))

        text = "\n".join(part.strip() for part in text_parts if part.strip())
        return text or "No textual response returned by Claude."


class OllamaLLMClient:
    def __init__(self, settings: Settings) -> None:
        self._model = settings.ollama_model
        self._api_url = settings.ollama_api_url
        self._timeout = settings.llm_timeout_seconds
        self._keep_alive = settings.ollama_keep_alive
        self._canon_prompt = self._read_prompt(settings.canon_prompt_file)
        self._draft_prompt = self._read_prompt(settings.draft_prompt_file)

    @staticmethod
    def _read_prompt(path: Path) -> str:
        if not path.exists():
            return "You are a canon-grounded lore assistant."
        return path.read_text(encoding="utf-8").strip()

    @staticmethod
    def _build_context(hits: list[RetrievalHit]) -> str:
        lines: list[str] = []
        for hit in hits:
            lines.append(f"{hit.claim.claim_id}: {hit.claim.text}")
        return "\n".join(lines)

    def answer(self, question: str, hits: list[RetrievalHit], mode: str) -> str:
        system_prompt = self._draft_prompt if mode == "draft" else self._canon_prompt
        context = self._build_context(hits)
        prompt = (
            f"{system_prompt}\n\n"
            "Use only the canon context where possible and cite claim IDs explicitly.\n\n"
            f"Canon context:\n{context}\n\n"
            f"Question:\n{question}\n"
        )

        payload = {
            "model": self._model,
            "prompt": prompt,
            "stream": False,
            "keep_alive": self._keep_alive,
            "options": {
                "temperature": 0.2,
            },
        }

        with httpx.Client(timeout=self._timeout) as client:
            try:
                response = client.post(self._api_url, json=payload)
                response.raise_for_status()
                data = response.json()
            except httpx.ConnectError as exc:
                raise RuntimeError(
                    "Could not connect to Ollama. Ensure Ollama is running and reachable at "
                    f"{self._api_url}."
                ) from exc
            except httpx.HTTPStatusError as exc:
                body = exc.response.text if exc.response is not None else "<no body>"
                raise RuntimeError(f"Ollama request failed: {body}") from exc
            except ValueError as exc:
                raise RuntimeError("Ollama returned a non-JSON response.") from exc

        if bool(data.get("done_reason") == "load"):
            # Some runtimes may return a model-load response on the first call.
            return "Model is loading in Ollama; retry in a few seconds."

        text = str(data.get("response", "")).strip()
        return text or "No textual response returned by Ollama."


def build_llm_client(settings: Settings) -> LLMClient:
    if settings.llm_provider == "claude":
        return ClaudeLLMClient(settings)
    if settings.llm_provider == "ollama":
        return OllamaLLMClient(settings)
    return StubLLMClient()

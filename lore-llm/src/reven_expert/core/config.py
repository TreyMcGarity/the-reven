from __future__ import annotations

from pathlib import Path
from typing import Literal

from pydantic import Field, field_validator
from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    model_config = SettingsConfigDict(env_file=".env", env_prefix="REVEN_")

    mode: Literal["canon", "draft"] = "canon"
    llm_provider: Literal["stub", "claude", "ollama"] = "stub"

    canon_index: Path = Path("../docs/canon/INDEX.yaml")
    canon_files: str = "../docs/canon/shadow-conquest-canon.yaml,../docs/canon/factions.yaml,../docs/canon/regions.yaml,../docs/canon/timeline-10840-10843.yaml"
    max_context_claims: int = Field(default=10, ge=1, le=50)

    anthropic_api_key: str | None = None
    anthropic_model: str = "claude-sonnet-4-20250514"
    anthropic_api_url: str = "https://api.anthropic.com/v1/messages"
    llm_timeout_seconds: int = Field(default=45, ge=5, le=180)

    ollama_model: str = "qwen2.5:3b-instruct-q4_K_M"
    ollama_api_url: str = "http://127.0.0.1:11434/api/generate"
    ollama_keep_alive: str = "20m"

    canon_prompt_file: Path = Path("prompts/system_canon_mode.txt")
    draft_prompt_file: Path = Path("prompts/system_draft_mode.txt")

    @field_validator("canon_files")
    @classmethod
    def validate_canon_files(cls, value: str) -> str:
        if not value.strip():
            raise ValueError("canon_files cannot be empty")
        return value

    @property
    def canon_paths(self) -> list[Path]:
        return [Path(item.strip()) for item in self.canon_files.split(",") if item.strip()]


settings = Settings()

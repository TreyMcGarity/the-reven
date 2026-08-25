# Reven Expert LLM Scaffold

Python-first scaffold for a canon-aware lore assistant in this repository.

## Goals
- Answer lore questions with citations to canon claim IDs.
- Support two modes:
  - `canon`: facts only from approved canon files.
  - `draft`: allows speculative options clearly marked as non-canon.
- Block answers that cannot be grounded in canon.
- Support provider selection:
  - `stub`: local deterministic scaffold behavior.
  - `claude`: Anthropic Messages API.
  - `ollama`: local model runtime (no business API key).

## Project Layout
- `src/reven_expert/core`: configuration, data models
- `src/reven_expert/ingestion`: canon loaders and claim extraction
- `src/reven_expert/retrieval`: retrieval and ranking
- `src/reven_expert/guardrails`: citation and contradiction checks
- `src/reven_expert/generation`: prompt and response assembly
- `src/reven_expert/api`: FastAPI surface
- `scripts`: local entry points

## Quick Start
1. Create a virtual environment and install dependencies.
2. Copy `.env.example` to `.env` and set values.
3. Run canon ingestion.
4. Start the API.

### Commands (PowerShell)
```powershell
cd lore-llm
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e .
copy .env.example .env
python scripts\ingest_canon.py
uvicorn reven_expert.api.main:app --reload --port 8010
```

## Wire to Claude
Set these values in `.env`:

```env
REVEN_LLM_PROVIDER=claude
REVEN_ANTHROPIC_API_KEY=your_anthropic_api_key
REVEN_ANTHROPIC_MODEL=claude-sonnet-4-20250514
REVEN_ANTHROPIC_API_URL=https://api.anthropic.com/v1/messages
REVEN_LLM_TIMEOUT_SECONDS=45
```

Then restart the API and verify:

```powershell
curl http://127.0.0.1:8010/health
```

Expected health field: `"provider": "claude"`.

## Wire to Ollama (Standalone)
1. Install Ollama and ensure the daemon is running.
2. Pull a model suitable for older hardware (default below).
3. Set provider to `ollama` in `.env`.

```env
REVEN_LLM_PROVIDER=ollama
REVEN_OLLAMA_MODEL=qwen2.5:3b-instruct-q4_K_M
REVEN_OLLAMA_API_URL=http://127.0.0.1:11434/api/generate
REVEN_OLLAMA_KEEP_ALIVE=20m
REVEN_LLM_TIMEOUT_SECONDS=90
```

Recommended starter model for older machines:

```powershell
ollama pull qwen2.5:3b-instruct-q4_K_M
```

Alternative small options:
- `llama3.2:3b-instruct-q4_K_M`
- `phi3:mini-4k-instruct-q4_K_M`

Verify Ollama provider is active:

```powershell
curl http://127.0.0.1:8010/health
```

Expected health field: `"provider": "ollama"`.

Use helper script to prep quickly:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\setup_ollama.ps1
```

## Canon Sources
The default config points to:
- `docs/canon/INDEX.yaml`
- `docs/canon/shadow-conquest-canon.yaml`
- `docs/canon/factions.yaml`
- `docs/canon/regions.yaml`
- `docs/canon/timeline-10840-10843.yaml`

## Next Build Steps
- Replace `StubLLMClient` with your preferred model provider.
- Add embeddings + vector store in `storage`.
- Add eval fixtures under `tests` for canon faithfulness.

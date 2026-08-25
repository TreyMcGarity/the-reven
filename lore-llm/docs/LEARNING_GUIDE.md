# Reven Expert Learning Guide

This guide teaches you how to run, understand, and improve the Reven model workflow.

## 1. What You Built
You have a local lore assistant that:
- reads canon claims,
- retrieves relevant lore,
- blocks unsupported answers,
- and can run with local Ollama or remote providers.

It is not just "a chatbot". It is a canon-governed pipeline.

## 2. Core Concepts

### Canon-First
- Canon facts come from `docs/canon/`.
- The model should cite claim IDs for factual answers.
- If lore is missing, response should be blocked or marked unresolved.

### Modes
- `canon`: strict factual mode.
- `draft`: allows optional ideas, but must not present them as canon.

### Providers
- `stub`: deterministic local test mode.
- `ollama`: local standalone model runtime.
- `claude`: external provider.

## 3. First Run (Recommended Path)

### Step A: Python Setup
```powershell
cd lore-llm
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e .
copy .env.example .env
```

### Step B: Local Model Setup (Ollama)
```powershell
powershell -ExecutionPolicy Bypass -File scripts\set_provider_ollama.ps1
powershell -ExecutionPolicy Bypass -File scripts\setup_ollama.ps1
```

### Step C: Ingest Canon
```powershell
python scripts\ingest_canon.py
```

### Step D: Start API
```powershell
uvicorn reven_expert.api.main:app --reload --port 8010
```

### Step E: Verify Health
```powershell
curl http://127.0.0.1:8010/health
```
You should see provider `ollama` and non-zero `claims_loaded`.

## 4. Ask Questions

Example request:
```powershell
curl -Method Post http://127.0.0.1:8010/ask -ContentType "application/json" -Body '{"question":"Who formed the first organized resistance in Mayrn?","mode":"canon"}'
```

Expected response shape:
- `answer`: model response
- `citations`: list of claim IDs
- `mode`: active mode
- `blocked`: whether answer was blocked
- `block_reason`: why it was blocked

## 5. How to Organize New Lore
1. Add source notes to `docs/raw/`.
2. Run your processing workflow to promote approved facts into `docs/canon/`.
3. Re-run `python scripts/ingest_canon.py`.
4. Re-test with `/ask` and check citations.

## 6. When to Fine-Tune
Use fine-tuning only when you need style and instruction consistency beyond retrieval quality.

Use RAG only when:
- you mainly need factual retrieval,
- your hardware is old,
- and canon changes often.

Use LoRA fine-tune when:
- you have stable canon style,
- you want stronger citation habits,
- and you have GPU capacity.

See `finetune/README.md` for the training path.

## 7. Learning Milestones
- Milestone 1: Run local API and get health check.
- Milestone 2: Get a canon answer with citations.
- Milestone 3: Trigger a blocked response intentionally and understand why.
- Milestone 4: Add one new canon claim and verify retrieval finds it.
- Milestone 5: Run dataset build for fine-tune (`finetune/data/reven_sft.jsonl`).

## 8. Common Mistakes
- Forgetting to activate `.venv`.
- Running with empty `.env`.
- Expecting invented facts in `canon` mode.
- Confusing ingestion success with retrieval quality.

## 9. Your Next Skills
- Improve retrieval ranking (hybrid lexical + embeddings).
- Add contradiction checker against canon claim graph.
- Add evaluation set for recurring lore questions.

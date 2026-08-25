# Troubleshooting

## 1. API Won't Start

### Symptom
`uvicorn` fails on startup.

### Checks
1. Activate venv:
```powershell
.\.venv\Scripts\Activate.ps1
```
2. Reinstall package:
```powershell
pip install -e .
```
3. Validate env:
- `.env` exists
- `REVEN_LLM_PROVIDER` is valid (`stub`, `ollama`, or `claude`)

## 2. Ollama Connection Error

### Symptom
`Could not connect to Ollama...`

### Fix
1. Start Ollama desktop/runtime.
2. Verify daemon:
```powershell
curl http://127.0.0.1:11434/api/tags
```
3. Run setup:
```powershell
powershell -ExecutionPolicy Bypass -File scripts\setup_ollama.ps1
```

## 3. Provider Misconfiguration

### Symptom
Startup fails with provider URL validation error.

### Cause
- Non-HTTPS Claude URL
- Remote Ollama URL while `REVEN_ALLOW_REMOTE_OLLAMA=false`

### Fix
- For local safety keep:
```env
REVEN_ALLOW_REMOTE_OLLAMA=false
REVEN_OLLAMA_API_URL=http://127.0.0.1:11434/api/generate
```

## 4. No Citations / Blocked Responses

### Symptom
`blocked=true` from `/ask`.

### Cause
No matching canon claims found.

### Fix
1. Confirm canon files are present in `docs/canon/`.
2. Re-run ingestion:
```powershell
python scripts\ingest_canon.py
```
3. Ask narrower questions using known entities and dates.

## 5. Fine-Tune Training Fails

### Symptom
`CUDA GPU was not detected...`

### Explanation
LoRA training script requires practical GPU support.

### Fix
- Use a GPU machine, or
- Skip fine-tune and stay with local Ollama + retrieval.

## 6. Slow Responses on Old Hardware

### Improvements
- Use smaller quantized model (`3b q4`).
- Reduce question complexity.
- Keep context small (`REVEN_MAX_CONTEXT_CLAIMS=6..10`).
- Avoid fine-tune on CPU.

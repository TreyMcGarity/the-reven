# Operations Runbook

## Daily Startup
1. Open terminal in `lore-llm`.
2. Activate venv.
3. Ensure Ollama is running.
4. Start API.
5. Verify `/health`.

## Daily Shutdown
1. Stop API process.
2. Close Ollama if not needed.

## Canon Refresh Procedure
Run this whenever canon changes.

```powershell
python scripts\ingest_canon.py
```

Then smoke test `/ask` with 2-3 known questions and verify citations.

## Safety Posture Checklist
- `.env` not committed
- provider is local (`ollama`) unless intentionally changed
- `REVEN_ALLOW_REMOTE_OLLAMA=false`
- firewall blocks external access to `8010` and `11434`

## Incident Playbook

### Unexpected network behavior
1. Stop API.
2. Set provider to `stub`.
3. Validate `.env` endpoint values.
4. Re-enable `ollama` only after confirming localhost endpoints.

### Suspected secret leak
1. Rotate compromised key immediately.
2. Remove leaked secrets from git history.
3. Regenerate `.env` from `.env.example`.

### Corrupt model behavior
1. Switch to `stub` for isolation.
2. Re-run `scripts/setup_ollama.ps1`.
3. Pull known-good model again.

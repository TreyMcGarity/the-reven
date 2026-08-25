# Reven Expert API Reference

Base URL (default local): `http://127.0.0.1:8010`

## GET /health
Returns service status and runtime metadata.

### Response Example
```json
{
  "status": "ok",
  "mode": "canon",
  "provider": "ollama",
  "claims_loaded": 15
}
```

## POST /ask
Submit a lore question.

### Request Body
```json
{
  "question": "Who organized cross-city defense during the conquest?",
  "mode": "canon"
}
```

### Fields
- `question` (string, required): natural language question.
- `mode` (string, optional): `canon` or `draft`.

### Response Body
```json
{
  "answer": "...",
  "citations": ["CANON-010"],
  "mode": "canon",
  "blocked": false,
  "block_reason": null
}
```

### Error/Block Semantics
- `blocked=true` means guardrails prevented unsupported answer.
- `block_reason` explains the block condition.

## Behavior Guarantees
- If no canon grounding is found, `/ask` returns a blocked response.
- Provider choice controls generation backend, not canon rules.

## cURL/PowerShell Examples

### PowerShell
```powershell
curl -Method Post http://127.0.0.1:8010/ask -ContentType "application/json" -Body '{"question":"What happened in 10843?","mode":"canon"}'
```

### cURL
```bash
curl -X POST http://127.0.0.1:8010/ask \
  -H "Content-Type: application/json" \
  -d '{"question":"What happened in 10843?","mode":"canon"}'
```

# Security Guidance for Reven Expert LLM

## Threat Model (Current Scaffold)
- Local FastAPI server answering lore questions.
- Optional outbound calls to Claude or Ollama endpoint.
- Canon files loaded from local repo paths.

## Current Protections
- Canon grounding guard blocks unsupported answers.
- Ollama endpoint defaults to localhost.
- Remote Ollama endpoints are blocked unless explicitly allowed.
- Claude endpoint requires HTTPS.

## Sandbox Recommendations
1. Run locally only
- Start API on localhost and do not publish through reverse proxy.
- Keep Ollama at localhost default port.

2. Least privilege
- Run the process as non-admin.
- Keep project directory write permissions limited to your user.

3. Network containment
- Block inbound access to ports 8010 and 11434 from all external interfaces.
- Optionally block outbound internet entirely when in Ollama-only mode.

4. Secrets hygiene
- Keep API keys in `.env` only.
- Add `.env` to `.gitignore` and never commit it.

5. Dependency hygiene
- Pin and periodically review dependencies.
- Upgrade packages when security fixes are published.

## Hardening Ideas (Next)
- Add API auth token for `/ask`.
- Add request size limits and simple rate limits.
- Add structured audit logging for provider calls and blocked responses.

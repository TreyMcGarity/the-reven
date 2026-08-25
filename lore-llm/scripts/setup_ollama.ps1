$ErrorActionPreference = "Stop"

Write-Host "[reven] Checking Ollama installation..."
$ollamaCmd = Get-Command ollama -ErrorAction SilentlyContinue
if (-not $ollamaCmd) {
    Write-Host "[reven] Ollama is not installed."
    Write-Host "[reven] Install from https://ollama.com/download/windows then re-run this script."
    exit 1
}

Write-Host "[reven] Ollama found: $($ollamaCmd.Source)"

Write-Host "[reven] Checking daemon..."
try {
    $null = Invoke-RestMethod -Method Get -Uri "http://127.0.0.1:11434/api/tags" -TimeoutSec 8
    Write-Host "[reven] Ollama daemon is running."
} catch {
    Write-Host "[reven] Ollama daemon is not reachable. Start Ollama desktop and retry."
    exit 1
}

$model = "qwen2.5:3b-instruct-q4_K_M"
Write-Host "[reven] Pulling model $model ..."
ollama pull $model

Write-Host "[reven] Running smoke prompt..."
$payload = @{
    model = $model
    prompt = "Reply with exactly: REVEN_OLLAMA_READY"
    stream = $false
} | ConvertTo-Json

$response = Invoke-RestMethod -Method Post -Uri "http://127.0.0.1:11434/api/generate" -Body $payload -ContentType "application/json"
Write-Host "[reven] Model response: $($response.response)"
if (($response.response -as [string]).Trim() -ne "REVEN_OLLAMA_READY") {
    Write-Host "[reven] Warning: smoke response differed from expected token. This can happen with some instruction tuning, but runtime connectivity is working."
}

Write-Host "[reven] Ollama local setup complete."

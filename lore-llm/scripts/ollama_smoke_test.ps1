$ErrorActionPreference = "Stop"

if (-not (Test-Path ".env")) {
    Write-Host "[reven] Missing .env. Run scripts\set_provider_ollama.ps1 first."
    exit 1
}

Get-Content ".env" | ForEach-Object {
    if ($_ -match "^([^#=]+)=(.*)$") {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

$model = [Environment]::GetEnvironmentVariable("REVEN_OLLAMA_MODEL")
if (-not $model) { $model = "qwen2.5:3b-instruct-q4_K_M" }

$payload = @{
    model = $model
    prompt = "You are online. Reply with exactly: REVEN_OLLAMA_READY"
    stream = $false
} | ConvertTo-Json

$response = Invoke-RestMethod -Method Post -Uri "http://127.0.0.1:11434/api/generate" -Body $payload -ContentType "application/json" -TimeoutSec 120
Write-Host "[reven] Response: $($response.response)"

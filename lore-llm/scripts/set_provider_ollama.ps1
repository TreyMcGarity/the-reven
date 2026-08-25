$ErrorActionPreference = "Stop"

$envFile = ".env"
if (-not (Test-Path $envFile)) {
    Copy-Item ".env.example" ".env"
    Write-Host "[reven] Created .env from .env.example"
}

$content = Get-Content $envFile -Raw
$script:content = $content

function Set-Or-Add([string]$key, [string]$value) {
    $pattern = "(?m)^$([regex]::Escape($key))=.*$"
    if ($script:content -match $pattern) {
        $script:content = [regex]::Replace($script:content, $pattern, "$key=$value")
    } else {
        if (-not $script:content.EndsWith("`n")) { $script:content += "`n" }
        $script:content += "$key=$value`n"
    }
}

Set-Or-Add "REVEN_LLM_PROVIDER" "ollama"
Set-Or-Add "REVEN_OLLAMA_MODEL" "qwen2.5:3b-instruct-q4_K_M"
Set-Or-Add "REVEN_OLLAMA_API_URL" "http://127.0.0.1:11434/api/generate"
Set-Or-Add "REVEN_OLLAMA_KEEP_ALIVE" "20m"
Set-Or-Add "REVEN_LLM_TIMEOUT_SECONDS" "90"

Set-Content -Path $envFile -Value $content -Encoding UTF8
Write-Host "[reven] Updated .env for Ollama local provider."

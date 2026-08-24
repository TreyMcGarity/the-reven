Add-Type -AssemblyName System.IO.Compression.FileSystem

$mappings = @(
    @{ path = "docs/raw/The Reven_ Chaos Outline.docx"; slug = "chaos-outline" },
    @{ path = "docs/raw/The Reven_ Evil Within Outline.docx"; slug = "evil-within-outline" },
    @{ path = "docs/raw/The Reven_ TechnoMagic Outline.docx"; slug = "technomagic-outline" }
)

$outDir = "docs/processing"
if (-not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
}

foreach ($map in $mappings) {
    $src = $map.path
    $slug = $map.slug
    Write-Host "Processing $src -> $slug"
    
    if (-not (Test-Path $src)) {
        Write-Error "File not found: $src"
        continue
    }
    
    # Open zip and read word/document.xml
    $zip = [System.IO.Compression.ZipFile]::OpenRead($src)
    $entry = $zip.GetEntry("word/document.xml")
    if ($null -eq $entry) {
        Write-Error "word/document.xml not found in $src"
        $zip.Dispose()
        continue
    }
    
    $reader = New-Object System.IO.StreamReader($entry.Open())
    $xmlContent = $reader.ReadToEnd()
    $reader.Close()
    $zip.Dispose()
    
    # Write xml to docs/processing/<slug>.document.xml
    $xmlPath = Join-Path $outDir "$slug.document.xml"
    [System.IO.File]::WriteAllText($xmlPath, $xmlContent, [System.Text.Encoding]::UTF8)
    
    # Convert to text
    # 1. Replace </w:p> with newline
    # Since XML might contain namespaces/prefix like </w:p>, let's replace </w:p> or others
    $text = $xmlContent -replace '</w:p>', "`n"
    # 2. Remove tags <...>
    $text = $text -replace '<[^>]*>', ''
    # 3. Decode entities
    $text = [System.Net.WebUtility]::HtmlDecode($text)
    
    # 4. Trim/condense whitespace per line, drop empty lines
    $lines = $text -split "`r?`n"
    $cleanedLines = New-Object System.Collections.Generic.List[string]
    foreach ($line in $lines) {
        $trimmed = $line.Trim()
        # Condense whitespace (sequences of spaces/tabs/etc to single space)
        $trimmed = $trimmed -replace '\s+', ' '
        if ($trimmed -ne "") {
            $cleanedLines.Add($trimmed)
        }
    }
    
    $extractedTextPath = Join-Path $outDir "$slug.extracted.txt"
    [System.IO.File]::WriteAllLines($extractedTextPath, $cleanedLines, [System.Text.Encoding]::UTF8)
    
    # Output report info
    $xmlSize = (Get-Item $xmlPath).Length
    $txtSize = (Get-Item $extractedTextPath).Length
    
    Write-Host "Created: $xmlPath (Bytes: $xmlSize)"
    Write-Host "Created: $extractedTextPath (Bytes: $txtSize)"
    Write-Host "First 15 lines of $slug.extracted.txt:"
    Write-Host "============================="
    $linesToPrint = $cleanedLines | Select-Object -First 15
    foreach ($l in $linesToPrint) {
        Write-Host "  $l"
    }
    Write-Host "============================="
    Write-Host ""
}

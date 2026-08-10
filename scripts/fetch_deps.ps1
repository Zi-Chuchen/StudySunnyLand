# ============================================================
#  SunnyLand Dependency Downloader (Windows PowerShell)
#  Pure ASCII - no colors, no special chars
# ============================================================

[CmdletBinding()]
param(
    [switch]$UseProxy,
    [switch]$Help
)

$ErrorActionPreference = "Stop"

# --- Help ----------------------------------------------------
if ($Help) {
    Write-Host "Usage: .\fetch_deps.ps1 [-UseProxy] [-Help]"
    Write-Host ""
    Write-Host "  -UseProxy   Use gh-proxy.com for GitHub downloads"
    Write-Host "  -Help       Show this help message"
    exit 0
}

# --- Config --------------------------------------------------
$ScriptDir     = Split-Path -Parent $MyInvocation.MyCommand.Path
$ExternalDir   = Join-Path $ScriptDir "..\external"
$ExternalDir   = Resolve-Path $ExternalDir

$SDL3Ver     = "release-3.2.0"
$SDLImageVer = "release-3.2.0"
$SDLMixerVer = "release-3.2.0"
$SDLNetVer   = "release-3.2.0"
$SDLTTFVer   = "release-3.2.0"
$GLMVer      = "1.1.0"
$JSONVer     = "v3.11.3"
$SpdlogVer   = "v1.17.0"

# --- Proxy setup ----------------------------------------------
if ($UseProxy) {
    $BaseUrl = "https://gh-proxy.com/https://github.com"
    Write-Host "[INFO] Proxy ENABLED (gh-proxy.com)"
} else {
    $BaseUrl = "https://github.com"
    Write-Host "[INFO] Proxy DISABLED, direct download"
}

Write-Host ""
Write-Host "+------------------------------------------------+"
Write-Host "|       SunnyLand Dependency Downloader          |"
Write-Host "+------------------------------------------------+"
Write-Host ""

# --- Progress function (pure ASCII) --------------------------
$Total   = 8
$Current = 0

function Print-Progress {
    param([string]$Name)

    $script:Current++
    $Percent = [int]($script:Current * 100 / $script:Total)
    $Filled  = [int]($Percent * 20 / 100)
    $Empty   = 20 - $Filled

    $Bar = ""
    for ($i = 0; $i -lt $Filled; $i++) { $Bar += "#" }
    for ($i = 0; $i -lt $Empty;  $i++) { $Bar += "." }

    Write-Host "[$($script:Current)/$($script:Total)] [$Bar] $Percent% - $Name"
}

# --- Download function ---------------------------------------
function Get-Dependency {
    param(
        [string]$DirName,
        [string]$GitHubPath,
        [string]$Tag
    )

    $TargetDir = Join-Path $ExternalDir $DirName
    $CloneUrl  = "$BaseUrl/$GitHubPath.git"

    # Check if already exists
    if ((Test-Path (Join-Path $TargetDir ".git")) -or
        (Test-Path (Join-Path $TargetDir "CMakeLists.txt"))) {
        Write-Host "  [SKIP] $DirName already exists"
        return
    }

    Write-Host "  [DL]   $DirName ($Tag)"
    Write-Host "         URL: $CloneUrl"

    $Stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

    # Start git clone with progress
    $GitArgs = @("clone", "--progress", "--depth", "1", "--branch", $Tag, $CloneUrl, $TargetDir)
    $Proc = Start-Process -FilePath "git" -ArgumentList $GitArgs `
            -NoNewWindow -PassThru -RedirectStandardError "$env:TEMP\git_err.log"

    # Poll for progress
    while (-not $Proc.HasExited) {
        Start-Sleep -Milliseconds 500
        $Log = Get-Content "$env:TEMP\git_err.log" -ErrorAction SilentlyContinue
        foreach ($Line in $Log) {
            if ($Line -match "Receiving objects:\s*(\d+)%") {
                $Pct  = $Matches[1]
                Write-Host -NoNewline "`r         Progress: ${Pct}%   "
            }
        }
    }

    $Stopwatch.Stop()

    if ($Proc.ExitCode -eq 0) {
        Write-Host ""
        Write-Host "  [OK]   $DirName : done ($([int]$Stopwatch.Elapsed.TotalSeconds)s)"
    } else {
        Write-Host ""
        Write-Host "  [ERR]  $DirName : FAILED (exit code $($Proc.ExitCode))"
        Write-Host "         Try: rmdir /s /q $TargetDir"
        Get-Content "$env:TEMP\git_err.log" -ErrorAction SilentlyContinue | Write-Host
    }
}

# ============================================================
#  Download all dependencies
# ============================================================

Print-Progress "SDL3"
Get-Dependency "SDL3" "libsdl-org/SDL" $SDL3Ver

Print-Progress "SDL_image"
Get-Dependency "SDL3_image" "libsdl-org/SDL_image" $SDLImageVer

Print-Progress "SDL_mixer"
Get-Dependency "SDL3_mixer" "libsdl-org/SDL_mixer" $SDLMixerVer

Print-Progress "SDL_net"
Get-Dependency "SDL3_net" "libsdl-org/SDL_net" $SDLNetVer

Print-Progress "SDL_ttf"
Get-Dependency "SDL3_ttf" "libsdl-org/SDL_ttf" $SDLTTFVer

Print-Progress "glm"
Get-Dependency "glm" "g-truc/glm" $GLMVer

Print-Progress "json"
Get-Dependency "json" "nlohmann/json" $JSONVer

Print-Progress "spdlog"
Get-Dependency "spdlog" "gabime/spdlog" $SpdlogVer

# ============================================================
#  Summary
# ============================================================
Write-Host ""
Write-Host "+------------------------------------------------+"
Write-Host "|  [DONE] All dependencies processed             |"
Write-Host "+------------------------------------------------+"
Write-Host "  Directory : $ExternalDir"

# Calculate total size
if (Test-Path $ExternalDir) {
    $Size = (Get-ChildItem $ExternalDir -Recurse -File | Measure-Object -Property Length -Sum).Sum
    $SizeMB = [math]::Round($Size / 1MB, 1)
    Write-Host "  Total size: ${SizeMB} MB"
}

Write-Host ""
Write-Host "Next steps:"
Write-Host "  cmake -B build"
Write-Host "  cmake --build build -j 8"
Write-Host ""

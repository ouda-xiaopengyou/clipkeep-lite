# ClipKeep-Lite Deployment Script
# This script prepares the project for GitHub deployment

param(
    [string]$GitHubToken = "",
    [string]$RepositoryName = "clipkeep-lite"
)

Write-Host "=== ClipKeep-Lite Deployment Script ===" -ForegroundColor Cyan

# Check if in correct directory
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Host "ERROR: Not in clipkeep-lite directory!" -ForegroundColor Red
    exit 1
}

Write-Host "✓ Project structure verified"

# Initialize git repository if not already initialized
if (-not (Test-Path ".git")) {
    Write-Host "Initializing git repository..."
    git init
} else {
    Write-Host "Git repository already exists"
}

# Configure git user (use environment variables or prompt)
$gitUser = $env:GIT_USERNAME
$gitEmail = $env:GIT_EMAIL

if (-not $gitUser) {
    $gitUser = Read-Host "Enter Git username"
}
if (-not $gitEmail) {
    $gitEmail = Read-Host "Enter Git email"
}

git config user.name "$gitUser"
git config user.email "$gitEmail"

# Add all files
Write-Host "`nAdding files to git..."
git add .

# Initial commit
Write-Host "Making initial commit..."
git commit -m "Initial commit: ClipKeep-Lite C++ implementation"

# Push to GitHub
if ($GitHubToken -eq "") {
    Write-Host "`nRepository ready! Push with:" -ForegroundColor Yellow
    Write-Host "  git remote add origin https://github.com/$gitUser/$RepositoryName.git" -ForegroundColor Gray
    Write-Host "  git push -u origin main" -ForegroundColor Gray
    
    # Optionally ask if they want to proceed
    $proceed = Read-Host "`nDo you want to add remote and push? (y/n)"
    if ($proceed -eq 'y') {
        # Get token interactively if not provided
        $token = Read-Host "Enter GitHub personal access token (ghp_...)"
        $repoUrl = "https://$token@github.com/$gitUser/$RepositoryName.git"
        
        git remote add origin $repoUrl
        git branch -M main
        git push -u origin main
        
        Write-Host "✓ Successfully pushed to GitHub!" -ForegroundColor Green
    }
} else {
    $repoUrl = "https://$GitHubToken@github.com/$gitUser/$RepositoryName.git"
    
    git remote add origin $repoUrl 2>$null
    git branch -M main
    git push -u origin main
    
    Write-Host "`n✓ Repository deployed successfully!" -ForegroundColor Green
    Write-Host "URL: https://github.com/$gitUser/$RepositoryName" -ForegroundColor Cyan
}

Write-Host "`n=== Deployment Complete ===" -ForegroundColor Green

$projectPath = "."
$logFilePath = "$projectPath\build_and_flash_log.txt"

$atmelStudioPath = "AtmelStudio.exe"
$atmelStudioCleanArgs = "$projectPath\Main.atsln /clean Debug"
$atmelStudioBuildArgs = "$projectPath\Main.atsln /build Debug /out $logFilePath"

$modeOutput = mode
$comPortLine = $modeOutput | Select-String -Pattern "Status for device COM(\d+)"

if ($comPortLine) {
    $comPort = $comPortLine.Line -replace ".*(COM\d+).*", '$1'
    if ($comPort -match "^COM\d+$") {
        Write-Host "Valid COM port detected: $comPort"
    }
    else {
        Write-Host "No valid COM port detected!"
        exit
    }
}
else {
    Write-Host "No COM port detected!"
    exit
}


Write-Host "Cleaning the project..."
Start-Process -FilePath $atmelStudioPath -ArgumentList $atmelStudioCleanArgs -NoNewWindow -Wait

Write-Host "Building the project..."
Start-Process -FilePath $atmelStudioPath -ArgumentList $atmelStudioBuildArgs -NoNewWindow -Wait


if (-not (Test-Path $logFilePath)) {
    Write-Host "Waiting for log file creation..."
    while (-not (Test-Path $logFilePath)) {
        Start-Sleep -Seconds 1
    }
}

$logLines = Get-Content -Path $logFilePath -Tail 10

$avrdudePath = "avrdude.exe"
$avrdudeArgs = "-c arduino -p m128 -P $comPort -F -U flash:w:`"$projectPath\Main\Debug\Main.hex`":a"

if ($logLines -match "Build succeeded.") {
    Write-Host "Build succeeded. Running avrdude..."

    Start-Process -FilePath $avrdudePath -ArgumentList $avrdudeArgs -NoNewWindow -Wait

    Write-Host "avrdude process completed."
    Write-Host "Removing log file..."

    Remove-Item -Path $logFilePath -Force
}
elseif ($logLines -match "Build failed.") {
    Write-Host "Build failed. avrdude will not run."
}
else {
    Write-Host "Unable to determine build status. Please check the log manually."
}

Write-Host "Process completed."

$ports = [System.IO.Ports.SerialPort]::GetPortNames()
foreach ($p in $ports) {
    Write-Host "Trying $p at 115200 baud..."
    try {
        $port = new-Object System.IO.Ports.SerialPort $p,115200,'None',8,'One'
        $port.ReadTimeout = 3000
        $port.DtrEnable = $true
        $port.Open()
        Start-Sleep -Seconds 2
        $line = $port.ReadLine()
        Write-Host "SUCCESS on $p :" $line
        $port.Close()
        break
    } catch {
        Write-Host "Failed or Timeout on $p"
        if ($port -ne $null -and $port.IsOpen) { $port.Close() }
    }
}

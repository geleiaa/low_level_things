#include <Windows.h>
#include <stdio.h>

#define MAX_VALUE_NAME 16383
#define MAX_KEY_LENGTH 255

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage: regenum.exe <hklm/hkcu/root/config/users> <registry path to query>");
        return 0;
    }

    WCHAR achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD cbName;                   // size of name string 
    WCHAR achClass[MAX_PATH] = { 0 };  // buffer for class name 
    DWORD cchClassName = MAX_PATH;  // size of class string 
    DWORD cSubKeys = 0;               // number of subkeys 
    DWORD cbMaxSubKey;              // longest subkey size 
    DWORD cchMaxClass;              // longest class string 
    DWORD cValues;              // number of values for key 
    DWORD cchMaxValue;          // longest value name 
    DWORD cbMaxValueData;       // longest value data 
    DWORD cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode;

    WCHAR achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;

    HKEY hKey;
    if (strcmp(argv[1], "hklm") == 0) {
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, argv[2], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            printf("Error: %lu", GetLastError());
            return 0;
        }
    } else if (strcmp(argv[1], "hkcu") == 0) {
        if (RegOpenKeyExA(HKEY_CURRENT_USER, argv[2], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            printf("Error: %lu", GetLastError());
            return 0;
        }
    } else if (strcmp(argv[1], "root") == 0) {
        if (RegOpenKeyExA(HKEY_CLASSES_ROOT, argv[2], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            printf("Error: %lu", GetLastError());
            return 0;
        }
    } else if (strcmp(argv[1], "config") == 0) {
        if (RegOpenKeyExA(HKEY_CURRENT_CONFIG, argv[2], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            printf("Error: %lu", GetLastError());
            return 0;
        }
    } else if (strcmp(argv[1], "users") == 0) {
        if (RegOpenKeyExA(HKEY_USERS, argv[2], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            printf("Error: %lu", GetLastError());
            return 0;
        }
    } else {
        printf("Error: invalid argument\n");
        return 0;;
    }
    // Get the class name and the value count. 
    RegQueryInfoKeyW(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    // Enumerate the subkeys, until RegEnumKeyEx fails.
    if (cSubKeys) {
        printf("[+] Subkeys: %lu\n", cSubKeys);
        for (i = 0; i < cSubKeys; i++) {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyExW(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS) {
                printf("[.] %s\\%ls\n", argv[2], achKey);
            }
        }
    }
    // Enumerate the key values. 

    if (cValues) {
        for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++) {
            cchValue = MAX_VALUE_NAME;
            achValue[0] = '\0';
            DWORD vtype;
            retCode = RegEnumValueW(hKey, i, achValue, &cchValue, NULL, &vtype, NULL, NULL);
            if (retCode == ERROR_SUCCESS) {
                printf("[.] %-40ls", achValue);
                DWORD dataLen = 0;
                switch (vtype)
                {
                case REG_BINARY: {
                    RegQueryValueExW(hKey, achValue, NULL, NULL, NULL, &dataLen);
                    if (dataLen) {
                        WCHAR* data = (WCHAR*)calloc(dataLen + 1, sizeof(WCHAR));
                        if (data) {
                            if (RegQueryValueExW(hKey, achValue, NULL, NULL, (LPBYTE)data, &dataLen) == ERROR_SUCCESS) {
                                for (int j = 0; j < dataLen; j++) {
                                    printf("%02x ", data[j]);
                                }
                                printf("\n");
                            }
                            else {
                                printf("Error: %lu\n", GetLastError());
                            }
                            free(data);
                            data = NULL;
                        }
                    }
                    break;
                }
                case REG_DWORD: {
                    DWORD data;
                    DWORD bufSize = sizeof(DWORD);
                    if (RegQueryValueExW(hKey, achValue, NULL, NULL, (LPBYTE)&data, &bufSize) == ERROR_SUCCESS) {
                        printf("0x%x\n", (INT)data);
                    } else {
                        printf("Error: %lu\n", GetLastError());
                    }
                    break;
                }
                case REG_EXPAND_SZ: {
                    RegQueryValueExW(hKey, achValue, NULL, NULL, NULL, &dataLen);
                    if (dataLen) {
                        WCHAR* data = (WCHAR*)calloc(dataLen + 1, sizeof(WCHAR));
                        if (data) {
                            if (RegQueryValueExW(hKey, achValue, NULL, NULL, (LPBYTE)data, &dataLen) == ERROR_SUCCESS) {
                                printf("%ls\n", data);
                            }
                            else {
                                printf("Error: %lu\n", GetLastError());
                            }
                            free(data);
                            data = NULL;
                        }
                    }
                    break;
                }
                case REG_MULTI_SZ: {
                    RegQueryValueExW(hKey, achValue, NULL, NULL, NULL, &dataLen);
                    if (dataLen) {
                        WCHAR* data = (WCHAR*)calloc(dataLen + 1, sizeof(WCHAR));
                        if (data) {
                            if (RegQueryValueExW(hKey, achValue, NULL, NULL, (LPBYTE)data, &dataLen) == ERROR_SUCCESS) {
                                PWSTR psz = data;
                                while (*psz != 0) {
                                    printf("%ls, ", psz);
                                    psz += wcslen(psz) + 1;
                                }
                                printf("\n");

                            }
                            else {
                                printf("Error: %lu\n", GetLastError());
                            }
                            free(data);
                            data = NULL;
                        }
                    }
                    break;
                }
                case REG_SZ: {
                    RegQueryValueExW(hKey, achValue, NULL, NULL, NULL, &dataLen);
                    if (dataLen) {
                        WCHAR* data = (WCHAR*)calloc(dataLen + 1, sizeof(WCHAR));
                        if (data) {
                            if (RegQueryValueExW(hKey, achValue, NULL, NULL, (LPBYTE)data, &dataLen) == ERROR_SUCCESS) {
                                printf("%ls\n", data);
                            }
                            else {
                                printf("Error: %lu\n", GetLastError());
                            }
                            free(data);
                            data = NULL;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    RegCloseKey(hKey);
    return 0;
}
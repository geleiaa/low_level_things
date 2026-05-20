#include <Windows.h>
#include <stdio.h>

BOOL create_service(CHAR* svcName, CHAR* hostname, CHAR* localSvcPath, CHAR* remoteSvcPath) {
    SC_HANDLE hRemoteSvcManager = INVALID_HANDLE_VALUE, hRemoteSvc = INVALID_HANDLE_VALUE;
    printf("[+] Copying: '%s' to '%s'\n", localSvcPath, remoteSvcPath);
    if (CopyFileA(localSvcPath, remoteSvcPath, FALSE)) {
        // Get a handle to the SCM database. 
        hRemoteSvcManager = OpenSCManagerA(hostname, SERVICES_ACTIVE_DATABASEA, SC_MANAGER_ALL_ACCESS);
        if (hRemoteSvcManager == NULL) {
            printf("Error: %lu\n", GetLastError());
            return 0;
        }

        hRemoteSvc = CreateServiceA(
            hRemoteSvcManager,
            svcName,
            svcName,
            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
            remoteSvcPath,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
            );

        if (hRemoteSvc == NULL) {
            printf("Error: %lu\n", GetLastError());
            return 0;
        }
        printf("[+] Service created: %s\n", svcName);
        if (StartServiceA(hRemoteSvc, NULL, NULL)) {
            printf("[+] Service started\n");
        }
    }
    else {
        printf("Error: %lu\n", GetLastError());
    }
    return TRUE;
}

BOOL delete_service(CHAR* svcName, CHAR* hostname) {
    SC_HANDLE hSvcManager = INVALID_HANDLE_VALUE, hSvc = INVALID_HANDLE_VALUE;

    hSvcManager = OpenSCManagerA(hostname, SERVICES_ACTIVE_DATABASEA, SC_MANAGER_ALL_ACCESS);
    if (hSvcManager == NULL) {
        printf("Error: %lu\n", GetLastError());
        return 0;
    }

    hSvc = OpenServiceA(hSvcManager, svcName, DELETE);
    if (hSvc == NULL) {
        printf("Error: %lu\n", GetLastError());
        return 0;
    }

    if (DeleteService(hSvc)) {
        printf("Service deleted\n");
    }
    else {
        printf("Error: %lu\n", GetLastError());
        return 0;
    }
    return TRUE;
}

BOOL impersonateUser(char* username, char* domain, char* password) {
    HANDLE htoken = NULL;
    if (LogonUserA(username, domain, password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_DEFAULT, &htoken)) {
        if (ImpersonateLoggedOnUser(htoken)) {
            printf("[+] Impersonated User: %s\n", username);
            return TRUE;
        }
    }
    else {
        printf("[+] Error: %lu\n", GetLastError());
    }
    return FALSE;
}

int main(int argc, char *argv[]) {

    if (argc > 2) {
        if ((strcmp(argv[1], "create") == 0) && argc == 9) {
            if (impersonateUser(argv[4], argv[5], argv[6])) {
                create_service(argv[2], argv[3], argv[7], argv[8]);
            }
            return 0;
        }
        else if ((strcmp(argv[1], "delete") == 0) && argc == 7) {
            if (impersonateUser(argv[4], argv[5], argv[6])) {
                delete_service(argv[2], argv[3]);
            }
            return 0;
        }
    }

    if (argc < 3) {
        printf("Usage: sc_manager.exe create <servicename> <hostname> <username> <domain> <password> <local service filepath> <remote service filepath>\n");
        printf("Usage: sc_manager.exe delete <servicename> <hostname> <username> <domain> <password>\n");
        return 0;
    }

    return 0;
}

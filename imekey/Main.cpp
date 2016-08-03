#include <windows.h>

/*
VC保证应用程序只有一个实例在运行
通过#pragma data_seg预编译指令创建一个新节，在此节中可用volatile关键字定义一个变量，而且必须对其进行初始化。Volatile关键字指定了变量可以为外部进程访问。最后，为了使该变量能够在进程互斥过程中发挥作用，还要将其设置为共享变量，同时允许具有读、写访问权限。这可以通过#pragma comment预编译指令来通知编译器。
*/
#pragma data_seg("Shared") 
int volatile g_lAppInstance = 0;
#pragma data_seg() 
#pragma comment(linker,"/section:Shared,RWS")

static HHOOK hKeyboardHook;
LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam)
{
	PKBDLLHOOKSTRUCT hs = (PKBDLLHOOKSTRUCT)lParam;
	if (code == HC_ACTION) {
		if (hs->vkCode == VK_SPACE && GetKeyState(VK_CONTROL) < 0) {
			switch (wParam) {
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				keybd_event(VK_SHIFT, 0, 0, 0);
				keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
				return 1;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				return 1;
			}
		}
	}
	return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	if (0 == g_lAppInstance)
	{
		g_lAppInstance = 1;
	}
	else if (1 == g_lAppInstance)
	{
		::MessageBox(NULL, TEXT("此程序之前已经在运行了。"), TEXT("注意"), MB_OK);
		return FALSE;
	}
	MSG msg;
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hKeyboardHook);
	return 0;
}
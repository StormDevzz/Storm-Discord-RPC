#include <windows.h>
#include <commctrl.h>
#include <string>
#include "../shared/include/discord_rpc.h"

// идентификаторы элементов управления
#define ID_CLIENT_ID_EDIT 1001
#define ID_CONNECT_BUTTON 1002
#define ID_DISCONNECT_BUTTON 1003
#define ID_STATE_EDIT 1004
#define ID_DETAILS_EDIT 1005
#define ID_LARGE_IMAGE_EDIT 1006
#define ID_LARGE_TEXT_EDIT 1007
#define ID_SMALL_IMAGE_EDIT 1008
#define ID_SMALL_TEXT_EDIT 1009
#define ID_UPDATE_BUTTON 1010
#define ID_STATUS_LABEL 1011

// глобальные переменные
HWND hwnd;
HWND hClientIdEdit;
HWND hConnectButton;
HWND hDisconnectButton;
HWND hStateEdit;
HWND hDetailsEdit;
HWND hLargeImageEdit;
HWND hLargeTextEdit;
HWND hSmallImageEdit;
HWND hSmallTextEdit;
HWND hUpdateButton;
HWND hStatusLabel;

DiscordRPC discord;
bool connected = false;

// прототипы функций
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void initializeControls(HWND hwnd);
void onConnect();
void onDisconnect();
void onUpdate();
void updateConnectionStatus();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // инициализация Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    
    // регистрация класса окна
    const char* className = "StormDiscordRPC";
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className;
    
    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "Ошибка регистрации класса окна", "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // создание окна
    hwnd = CreateWindowExA(
        0,
        className,
        "Storm Discord RPC",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        MessageBoxA(NULL, "Ошибка создания окна", "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    initializeControls(hwnd);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // главный цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_CONNECT_BUTTON:
                    onConnect();
                    break;
                case ID_DISCONNECT_BUTTON:
                    onDisconnect();
                    break;
                case ID_UPDATE_BUTTON:
                    onUpdate();
                    break;
            }
            break;
            
        case WM_TIMER:
            if (wParam == 1) {
                updateConnectionStatus();
            }
            break;
            
        case WM_DESTROY:
            discord.disconnect();
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void initializeControls(HWND hwnd) {
    // создаем элементы управления
    hClientIdEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 40, 360, 25, hwnd, (HMENU)ID_CLIENT_ID_EDIT, GetModuleHandle(NULL), NULL);
    
    hConnectButton = CreateWindowA("BUTTON", "Подключиться", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 75, 110, 30, hwnd, (HMENU)ID_CONNECT_BUTTON, GetModuleHandle(NULL), NULL);
    
    hDisconnectButton = CreateWindowA("BUTTON", "Отключиться", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        130, 75, 110, 30, hwnd, (HMENU)ID_DISCONNECT_BUTTON, GetModuleHandle(NULL), NULL);
    
    hStateEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 140, 360, 25, hwnd, (HMENU)ID_STATE_EDIT, GetModuleHandle(NULL), NULL);
    
    hDetailsEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 180, 360, 25, hwnd, (HMENU)ID_DETAILS_EDIT, GetModuleHandle(NULL), NULL);
    
    hLargeImageEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 220, 360, 25, hwnd, (HMENU)ID_LARGE_IMAGE_EDIT, GetModuleHandle(NULL), NULL);
    
    hLargeTextEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 260, 360, 25, hwnd, (HMENU)ID_LARGE_TEXT_EDIT, GetModuleHandle(NULL), NULL);
    
    hSmallImageEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 300, 360, 25, hwnd, (HMENU)ID_SMALL_IMAGE_EDIT, GetModuleHandle(NULL), NULL);
    
    hSmallTextEdit = CreateWindowA("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 340, 360, 25, hwnd, (HMENU)ID_SMALL_TEXT_EDIT, GetModuleHandle(NULL), NULL);
    
    hUpdateButton = CreateWindowA("BUTTON", "Обновить статус", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 380, 360, 30, hwnd, (HMENU)ID_UPDATE_BUTTON, GetModuleHandle(NULL), NULL);
    
    hStatusLabel = CreateWindowA("STATIC", "Статус: не подключен", WS_CHILD | WS_VISIBLE,
        10, 420, 360, 20, hwnd, (HMENU)ID_STATUS_LABEL, GetModuleHandle(NULL), NULL);
    
    // добавляем текстовые метки
    CreateWindowA("STATIC", "Client ID:", WS_CHILD | WS_VISIBLE,
        10, 20, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Состояние:", WS_CHILD | WS_VISIBLE,
        10, 120, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Детали:", WS_CHILD | WS_VISIBLE,
        10, 160, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Большое изображение:", WS_CHILD | WS_VISIBLE,
        10, 200, 120, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Текст большого изображения:", WS_CHILD | WS_VISIBLE,
        10, 240, 160, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Маленькое изображение:", WS_CHILD | WS_VISIBLE,
        10, 280, 140, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    CreateWindowA("STATIC", "Текст маленького изображения:", WS_CHILD | WS_VISIBLE,
        10, 320, 180, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    // начальное состояние кнопок
    EnableWindow(hDisconnectButton, FALSE);
    EnableWindow(hUpdateButton, FALSE);
    
    // установка таймера для обработки событий
    SetTimer(hwnd, 1, 1000, NULL);
}

void onConnect() {
    char clientId[256];
    GetWindowTextA(hClientIdEdit, clientId, sizeof(clientId));
    
    if (strlen(clientId) == 0) {
        MessageBoxA(hwnd, "Ошибка: введите Client ID", "Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    
    if (discord.initialize(std::string(clientId))) {
        connected = true;
        EnableWindow(hConnectButton, FALSE);
        EnableWindow(hDisconnectButton, TRUE);
        EnableWindow(hUpdateButton, TRUE);
        EnableWindow(hClientIdEdit, FALSE);
        SetWindowTextA(hStatusLabel, "Статус: подключено");
    }
}

void onDisconnect() {
    discord.disconnect();
    connected = false;
    EnableWindow(hConnectButton, TRUE);
    EnableWindow(hDisconnectButton, FALSE);
    EnableWindow(hUpdateButton, FALSE);
    EnableWindow(hClientIdEdit, TRUE);
    SetWindowTextA(hStatusLabel, "Статус: не подключен");
}

void onUpdate() {
    char state[256], details[256], largeImage[256], largeText[256], smallImage[256], smallText[256];
    
    GetWindowTextA(hStateEdit, state, sizeof(state));
    GetWindowTextA(hDetailsEdit, details, sizeof(details));
    GetWindowTextA(hLargeImageEdit, largeImage, sizeof(largeImage));
    GetWindowTextA(hLargeTextEdit, largeText, sizeof(largeText));
    GetWindowTextA(hSmallImageEdit, smallImage, sizeof(smallImage));
    GetWindowTextA(hSmallTextEdit, smallText, sizeof(smallText));
    
    discord.updatePresence(std::string(state), std::string(details), 
                          std::string(largeImage), std::string(largeText),
                          std::string(smallImage), std::string(smallText));
}

void updateConnectionStatus() {
    if (connected) {
        discord.processEvents();
    }
}

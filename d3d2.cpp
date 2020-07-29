#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"
#include <d3d9.h>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
LPDIRECT3D9 d3d;                                 // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;                       // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer1 = NULL;    // the pointer to the vertex buffer


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HRESULT initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory
void init_graphics(void);    // 3D declarations

struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// this function initializes and prepares Direct3D for use
HRESULT initD3D(HWND hWnd)
{
    HRESULT hr = E_FAIL;
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
//    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = 1920;
    d3dpp.BackBufferHeight = 1080;

    // create a device class using this information and information from the d3dpp stuct
    hr = d3d->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3ddev);
        
    init_graphics();    // call the function to initialize the triangle

    return hr;
}


// this is the function used to render a single frame
void render_frame(void)
{
    // clear the window 
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    d3ddev->BeginScene();    // begins the 3D scene

    
    // select which vertex format we are using
    d3ddev->SetFVF(CUSTOMFVF);

    
    d3ddev->SetStreamSource(0, v_buffer1, 0, sizeof(CUSTOMVERTEX));


    // copy the vertex buffer to the back buffer
    d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);


    d3ddev->EndScene();    // ends the 3D scene

    d3ddev->Present(NULL, NULL, NULL, NULL);    // displays the created frame
}



// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    v_buffer1->Release();    // close and release the vertex buffer
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices1[] =
    {
        { 800.0f, 0.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 800.0f, 800.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 0.0f, 800.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
    };

    VOID* pVoid;    // a void pointer


    for (int i = 0; i < 100000; i++)
    {
        // create a vertex buffer interface called v_buffer1
        d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX) * 2,
            0,
            CUSTOMFVF,
            D3DPOOL_MANAGED,
            &v_buffer1,
            NULL);

        // lock v_buffer and load the vertices into it
        v_buffer1->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid, vertices1, sizeof(vertices1));
        v_buffer1->Unlock();

    }



}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_D3D2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    case WM_KEYDOWN:
    {
        //Esc keydown
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            break;
        }

    }


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    }
    return (INT_PTR)FALSE;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    hInst = hInstance; // Store instance handle in our global variable
    MSG msg{ 0 };


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    HWND hWnd = CreateWindowExW(
        NULL,
        szWindowClass, 
        szTitle, 
        WS_OVERLAPPEDWINDOW,
        0, 0,
        800, 600,
        nullptr, 
        nullptr, 
        hInstance, 
        nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HRESULT hr = initD3D(hWnd);
    LPWSTR pszError = nullptr;
    WCHAR szMessage[255] = {};

    if (hr != D3D_OK)
    {
        int iRet = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            hr,
            NULL,
            (LPWSTR)&pszError,
            NULL,
            NULL);

        if (iRet != NULL)
        {
            wsprintf(szMessage, L"Error 0x%X: %s", hr, pszError);

            MessageBoxW(hWnd, szMessage, (LPCWSTR)L"InitD3D", MB_ICONWARNING | MB_OK);

            LocalFree(pszError);

            ExitProcess(hr);
        }

        
    }


    // Main message loop:
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        render_frame();

    }

    // clean up DirectX and COM
    cleanD3D();

    return (int)msg.wParam;
}

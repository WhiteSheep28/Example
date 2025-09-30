#include <d3dx9.h>
#include <d3d9.h>


/*------------------------------------------------------------------------------
 * 전역변수
 *------------------------------------------------------------------------------
 */
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

struct CUSTOMVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};

HRESULT InitVB()
{
    CUSTOMVERTEX vertices[] =
    {
        {-0.4f, 0.8f, 0.0f, 0xffff0000, },
        {0.0f, -0.8f, 0.0f, 0xff00ff00, },
        {-0.8f, -0.8f, 0.0f, 0xff00ffff, }
    };

    g_pd3dDevice->CreateVertexBuffer(3 * 12, 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);

    VOID* pVertices;

    g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0);

    memcpy(pVertices, vertices, sizeof(vertices));

    g_pVB->Unlock();

    return S_OK;
}

/*------------------------------------------------------------------------------
 * Direct3D 초기화 : 자세히 살펴봐야 할 가장 중요한 함수.
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D(HWND hWnd)
{
    // 디바이스를 생성하기위한 D3D객체 생성
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    /* 디바이스를 다음과 같은 설정으로 생성한다.
         1. 디폴트 비디오카드를 사용(대부분은 비디오카드가 1개 이다.)
         2. HAL디바이스를 생성한다.(HW가속장치를 사용하겠다는 의미)
         3. 정점처리는 모든 카드에서 지원하는 SW처리로 생성한다.(HW로 생성할경우 더욱 높은 성능을 낸다.) */

         // 실제 디바이스를 생성하는 함수. (중요하다.)
    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pd3dDevice)))
    {
        return E_FAIL;
    }
    // 디바이스 상태정보를 처리할경우 여기에서 한다.
    return S_OK;
}

/*------------------------------------------------------------------------------
 * 초기화된 객체들을 소거한다.
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
    // 해제 순서. 해제시 반드시 생성 순서의 역순으로 해제 해줄 것. 
    if (g_pVB != NULL)
        g_pVB->Release();
    if (g_pd3dDevice != NULL)
        g_pd3dDevice->Release();
    if (g_pD3D != NULL)
        g_pD3D->Release();
}

/*
 *------------------------------------------------------------------------------
 * 화면을 그린다.
 * Render() 함수 알고리즘
 * 1. 먼저 화면을 깨끗이 지운다. ( Clear() )
 * 2. 폴리곤을 그리겠다고 D3D에게 알림.( BeginScene() )
 * 3. 폴리곤을 다 그렸다고 D3D에게 알림.( EndScene() )
 * 4. 화면에 나타나게 한다. ( Present() )
 *------------------------------------------------------------------------------
 */
VOID Render()
{
    if (NULL == g_pd3dDevice)
        return;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        // 실제 렌더링 명령들이 나열될 곳
        // TODO :     
        D3DXMATRIX tempTM;
        D3DXMatrixIdentity(&tempTM);
        g_pd3dDevice->SetTransform(D3DTS_WORLD, &tempTM);
        g_pd3dDevice->SetTransform(D3DTS_VIEW, &tempTM);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &tempTM);
        
        g_pd3dDevice->SetStreamSource(0, g_pVB, 0, 12);
        g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
        g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


/*
 *------------------------------------------------------------------------------
 * 윈도우 프로시져
 *------------------------------------------------------------------------------
 */
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        Render();
        ValidateRect(hWnd, NULL);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


/*------------------------------------------------------------------------------
 * 이 프로그램의 시작점
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    // 윈도우 클래스 등록
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx(&wc);
    // 윈도우 생성
    HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice",
        WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);
    // Direct3D 초기화
    if (SUCCEEDED(InitD3D(hWnd)))
    {
        // 윈도우 출력
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);
        InitD3D(hWnd);
        InitVB();

        // 메시지 루프
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    // 등록된 클래스 소거
    UnregisterClass("D3D Tutorial", wc.hInstance);
    return 0;
}
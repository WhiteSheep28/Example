#include <d3dx9.h>
#include <d3d9.h>


/*------------------------------------------------------------------------------
 * ��������
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
 * Direct3D �ʱ�ȭ : �ڼ��� ������� �� ���� �߿��� �Լ�.
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D(HWND hWnd)
{
    // ����̽��� �����ϱ����� D3D��ü ����
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    /* ����̽��� ������ ���� �������� �����Ѵ�.
         1. ����Ʈ ����ī�带 ���(��κ��� ����ī�尡 1�� �̴�.)
         2. HAL����̽��� �����Ѵ�.(HW������ġ�� ����ϰڴٴ� �ǹ�)
         3. ����ó���� ��� ī�忡�� �����ϴ� SWó���� �����Ѵ�.(HW�� �����Ұ�� ���� ���� ������ ����.) */

         // ���� ����̽��� �����ϴ� �Լ�. (�߿��ϴ�.)
    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pd3dDevice)))
    {
        return E_FAIL;
    }
    // ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.
    return S_OK;
}

/*------------------------------------------------------------------------------
 * �ʱ�ȭ�� ��ü���� �Ұ��Ѵ�.
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
    // ���� ����. ������ �ݵ�� ���� ������ �������� ���� ���� ��. 
    if (g_pVB != NULL)
        g_pVB->Release();
    if (g_pd3dDevice != NULL)
        g_pd3dDevice->Release();
    if (g_pD3D != NULL)
        g_pD3D->Release();
}

/*
 *------------------------------------------------------------------------------
 * ȭ���� �׸���.
 * Render() �Լ� �˰���
 * 1. ���� ȭ���� ������ �����. ( Clear() )
 * 2. �������� �׸��ڴٰ� D3D���� �˸�.( BeginScene() )
 * 3. �������� �� �׷ȴٰ� D3D���� �˸�.( EndScene() )
 * 4. ȭ�鿡 ��Ÿ���� �Ѵ�. ( Present() )
 *------------------------------------------------------------------------------
 */
VOID Render()
{
    if (NULL == g_pd3dDevice)
        return;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        // ���� ������ ��ɵ��� ������ ��
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
 * ������ ���ν���
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
 * �� ���α׷��� ������
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    // ������ Ŭ���� ���
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx(&wc);
    // ������ ����
    HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice",
        WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);
    // Direct3D �ʱ�ȭ
    if (SUCCEEDED(InitD3D(hWnd)))
    {
        // ������ ���
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);
        InitD3D(hWnd);
        InitVB();

        // �޽��� ����
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    // ��ϵ� Ŭ���� �Ұ�
    UnregisterClass("D3D Tutorial", wc.hInstance);
    return 0;
}
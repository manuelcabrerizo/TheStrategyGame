#include <windows.h>
#include <DirectXMath.h>
#include <d3d9.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Tile.h"

using namespace DirectX;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FVF_NORMAL_TEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
    Vertex(XMFLOAT3 position_,XMFLOAT3 normal_, XMFLOAT2 uv_) 
        : position(position_), normal(normal_), uv(uv_) { }
};

static bool gRunning;

// Message Loop Callback Function prototype
LRESULT CALLBACK fnMessageProcessor(HWND, UINT, WPARAM, LPARAM);

// Entry point of the program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance, PSTR szCmdLine, int iCmdShow) 
{
    HWND hWnd;
    MSG msg;
    WNDCLASSEX wndclass;
    
    // set up window class
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = fnMessageProcessor;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "TheStrategyGame"; // Class Name
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Register the window class
    if (RegisterClassEx(&wndclass) == 0)
    {
        // the program failed, exit
        exit(1);
    }

    // Create the window
    hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "TheStrategyGame",
        "The Strategy Game", WS_OVERLAPPEDWINDOW,
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    // Display the window
    ShowWindow(hWnd, iCmdShow);

    // Init D3D9
    IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    D3DCAPS9 caps;

    d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    int vertexProcessing = 0;
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    D3DDISPLAYMODE d3ddm;
    if (FAILED(d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
        return 0;
    }


    D3DPRESENT_PARAMETERS d3dpp;
    d3dpp.BackBufferWidth = WINDOW_WIDTH;
    d3dpp.BackBufferHeight = WINDOW_HEIGHT;
    d3dpp.BackBufferFormat = d3ddm.Format; //pixel format
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Windowed = true; // fullscreen
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; // depth format
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    

    IDirect3DDevice9* device = 0;
    HRESULT hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vertexProcessing, &d3dpp, &device);
    if (FAILED(hr)) 
    {
        MessageBox(0, "CreateDevice() - FAILED", 0, 0);
        return 0;
    }

    D3DVIEWPORT9 vp{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 1 };
    device->SetViewport(&vp);


    IDirect3DVertexBuffer9* vb;
    device->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, FVF_NORMAL_TEX, D3DPOOL_MANAGED, &vb, 0);
    
    // Fill the vertex buffer
    Vertex* vertices;
    vb->Lock(0, 0, (void**)&vertices, 0);
    // quad built from two triangles, note texture coordinates:
    vertices[0] = Vertex(XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
    vertices[1] = Vertex(XMFLOAT3(-1.0f,  1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
    vertices[2] = Vertex(XMFLOAT3( 1.0f,  1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
    vertices[3] = Vertex(XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
    vertices[4] = Vertex(XMFLOAT3( 1.0f,  1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
    vertices[5] = Vertex(XMFLOAT3( 1.0f, -1.0f, 0), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
    vb->Unlock();

    // Position and aim the camera
    XMFLOAT3 position(0.0f, 0.0f, -2.0f);
    XMFLOAT3 target(0.0f, 0.0f, 0.0f);
    XMFLOAT3 up(0.0f, 1.0f, 0.0f);
    XMFLOAT4X4 V;
    XMStoreFloat4x4(&V, XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), XMLoadFloat3(&up)));
    device->SetTransform(D3DTS_VIEW, reinterpret_cast<D3DMATRIX*>(&V));

    // Set projection matrix
    XMFLOAT4X4 P;
    XMStoreFloat4x4(&P, XMMatrixPerspectiveFovLH((60.0f / 180.0f) * XM_PI, (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 1.0f, 1000.0f));
    //XMStoreFloat4x4(&P, XMMatrixOrthographicLH(WINDOW_WIDTH*(1.0f/200.0f), WINDOW_HEIGHT*(1.0f/200.0f), 1.0f, 100.0f));

    device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<D3DMATRIX*>(&P));

    // Set Render State
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    
    device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    device->SetRenderState(D3DRS_LIGHTING, false);
    device->SetRenderState(D3DRS_SPECULARENABLE, true);

    D3DCOLORVALUE redColor;
    redColor.r = 1;
    redColor.g = 1;
    redColor.b = 1;
    redColor.a = 1;

    D3DCOLORVALUE darkYellowColor;
    darkYellowColor.r = 1*0.2f;
    darkYellowColor.g = 1*0.2f;
    darkYellowColor.b = 1*0.2f;
    darkYellowColor.a = 1;

    D3DCOLORVALUE yellowColor;
    yellowColor.r = 1;
    yellowColor.g = 1;
    yellowColor.b = 1;
    yellowColor.a = 1;

    D3DMATERIAL9 redMaterial;
    ZeroMemory(&redMaterial, sizeof(redMaterial));
    redMaterial.Diffuse = redColor;
    redMaterial.Ambient = redColor;
    redMaterial.Specular = redColor;
    redMaterial.Power = 5.0f;

    XMFLOAT3 direction(0, 0, 1);

    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Ambient = darkYellowColor;
    light.Diffuse = yellowColor;
    light.Specular = yellowColor;
    light.Direction = *reinterpret_cast<D3DVECTOR*>(&direction);

    device->SetLight(0, &light);
    device->LightEnable(0, true);


    // Texture
    int width, height, channels;
    unsigned char* data = stbi_load("rainbow.jpg", &width, &height, &channels, 4);
    if (!data) {
        return 0;
    }


    IDirect3DTexture9* texture;
    device->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, 0);
    if (texture)
    {
        D3DLOCKED_RECT rect;
        texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
        unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
        memcpy(dest, data, width * height * 4);
        texture->UnlockRect(0);
    }

    device->SetTexture(0, texture);

    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 4);

    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);




    // Process messages until the program is terminated
    gRunning = true;
    while (gRunning)
    {
        while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE) != 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (device)
        {
            static float time = 0.0f;
            time += 0.01f;

            // Set the world matrix
            XMMATRIX world = XMMatrixRotationZ(time);
            XMFLOAT4X4 W;
            XMStoreFloat4x4(&W, world);
            device->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&W));

            device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00AABBCC*0.5, 1.0f, 0);

            device->BeginScene();

            device->SetStreamSource(0, vb, 0, sizeof(Vertex));
            device->SetFVF(FVF_NORMAL_TEX);
            device->SetMaterial(&redMaterial);
            device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
            device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

            device->EndScene();

            device->Present(0, 0, 0, 0);
        }
    }

    vb->Release();

    DestroyWindow(hWnd);

    return msg.wParam;
}


LRESULT CALLBACK fnMessageProcessor(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (iMsg)
    {
        case WM_CLOSE:
        {
            gRunning = false;
        } break;
        default:
        {
            result = DefWindowProc(hWnd, iMsg, wParam, lParam);
        }break;
    }
    return result;
}
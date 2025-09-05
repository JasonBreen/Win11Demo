// Minimal Win32 + OpenGL 2.1 demoscene starter
#include <windows.h>
#include <GL/gl.h>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <cstring>

// Globals
HWND hwnd;
HGLRC hglrc;
HDC hdc;
bool running = true, fullscreen = false, reloadShader = false;
POINT mousePos = {0};
GLuint prog = 0;
float iTime = 0.0f;
int winW = 1280, winH = 720;
float iMouse[4] = {0};
// Cache uniform locations to avoid repeated lookups every frame
GLint iTimeLoc = -1, iResolutionLoc = -1, iMouseLoc = -1;

std::string LoadFile(const char* path) {
    std::ifstream f(path);
    if (!f.is_open())
        return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint CompileFragShader(const char* src) {
    GLuint s = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, 1024, nullptr, log);
        MessageBoxA(hwnd, log, "Shader Error", MB_OK);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

void ReloadShader() {
    std::string src = LoadFile("shader.frag");
    GLuint newFrag = CompileFragShader(src.c_str());
    if (!newFrag) return;

    GLuint newProg = glCreateProgram();
    glAttachShader(newProg, newFrag);
    glLinkProgram(newProg);
    glDeleteShader(newFrag);

    GLint linked = GL_FALSE;
    glGetProgramiv(newProg, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[1024];
        glGetProgramInfoLog(newProg, 1024, nullptr, log);
        MessageBoxA(hwnd, log, "Link Error", MB_OK);
        glDeleteProgram(newProg);
        return;
    }

    // Replace old program and cache uniform locations
    if (prog) glDeleteProgram(prog);
    prog = newProg;
    iTimeLoc = glGetUniformLocation(prog, "iTime");
    iResolutionLoc = glGetUniformLocation(prog, "iResolution");
    iMouseLoc = glGetUniformLocation(prog, "iMouse");
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_CLOSE: running = false; break;
    case WM_SIZE:
        winW = LOWORD(l); winH = HIWORD(l);
        glViewport(0, 0, winW, winH);
        break;
    case WM_KEYDOWN:
        if (w == VK_ESCAPE) running = false;
        if (w == VK_RETURN && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
            fullscreen = !fullscreen;
            SetWindowLong(hwnd, GWL_STYLE, fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, fullscreen ? GetSystemMetrics(SM_CXSCREEN) : 1280, fullscreen ? GetSystemMetrics(SM_CYSCREEN) : 720, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        }
        if (w == 'R') reloadShader = true;
        break;
    case WM_MOUSEMOVE:
        mousePos.x = LOWORD(l); mousePos.y = HIWORD(l);
        iMouse[0] = (float)mousePos.x;
        iMouse[1] = (float)mousePos.y;
        break;
    }
    return DefWindowProc(h, m, w, l);
}

void InitGL() {
    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0,0,0,0,0,0,0,0,0,0,0,0,24,8,0, PFD_MAIN_PLANE, 0,0,0,0};
    hdc = GetDC(hwnd);
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);
}

void Render() {
    if (!prog) return;
    glUseProgram(prog);
    if (iTimeLoc != -1)
        glUniform1f(iTimeLoc, iTime);
    if (iResolutionLoc != -1)
        glUniform2f(iResolutionLoc, (float)winW, (float)winH);
    if (iMouseLoc != -1)
        glUniform4f(iMouseLoc, iMouse[0], iMouse[1], iMouse[2], iMouse[3]);
    glBegin(GL_QUADS);
    glVertex2f(-1, -1); glVertex2f(1, -1); glVertex2f(1, 1); glVertex2f(-1, 1);
    glEnd();
    SwapBuffers(hdc);
}

int main() {
    WNDCLASS wc = {CS_OWNDC, WndProc, 0,0, GetModuleHandle(0), 0,0,0,0, L"GLWin"};
    RegisterClass(&wc);
    hwnd = CreateWindowEx(0, wc.lpszClassName, L"Win11Demo", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, winW, winH, 0,0, wc.hInstance, 0);
    ShowWindow(hwnd, SW_SHOW);
    InitGL();
    ReloadShader();
    auto start = std::chrono::high_resolution_clock::now();
    FILETIME lastWrite = {0};
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
        auto now = std::chrono::high_resolution_clock::now();
        iTime = std::chrono::duration<float>(now - start).count();
        // Hot-reload shader.frag if file changed
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesEx(L"shader.frag", GetFileExInfoStandard, &fad)) {
            if (memcmp(&fad.ftLastWriteTime, &lastWrite, sizeof(FILETIME)) != 0) {
                lastWrite = fad.ftLastWriteTime;
                reloadShader = true;
            }
        }
        if (reloadShader) { ReloadShader(); reloadShader = false; }
        Render();
        Sleep(10);
    }
    if (prog) glDeleteProgram(prog);
    wglMakeCurrent(0,0); wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    return 0;
}

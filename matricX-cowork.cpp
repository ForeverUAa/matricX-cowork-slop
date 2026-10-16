// matricX-cowork.cpp — Coworking simulation and ambient mouse wander tool for Windows
// Compile: g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -static matricX-cowork.cpp -o matricX-cowork.exe -luser32 -lgdi32 -lgdiplus

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")

// ============================================================================
// Constants & Version
// ============================================================================

static const char* VERSION_STRING = "matricX-cowork 0.3.0";
static const UINT_PTR TIMER_ID = 1;
static const int TIMER_INTERVAL_MS = 10;
static const int GLOW_FPS_DIVISOR = 5; // every 5th tick = ~50ms = 20fps

// ============================================================================
// Enums for CLI options
// ============================================================================

enum class Speed { Slow, Normal, Fast };
enum class Pattern { Wander, Work, Edge };
enum class GlowTheme { Green, Cyan, Rainbow, Off };

// ============================================================================
// Global State
// ============================================================================

static volatile bool g_running = true;
static volatile LONG g_cursorChanged = 0;

static int g_screenW = 0;
static int g_screenH = 0;

// CLI options
static int g_durationSec = 60;
static bool g_infinite = false;
static Speed g_speed = Speed::Normal;
static Pattern g_pattern = Pattern::Wander;
static GlowTheme g_glowTheme = GlowTheme::Green;
static bool g_noCursor = false;
static bool g_noLock = false;

// Motion state
static float g_curX = 0.0f, g_curY = 0.0f;
static float g_p0x, g_p0y, g_p1x, g_p1y, g_p2x, g_p2y, g_p3x, g_p3y;
static int g_stepCurrent = 0;
static int g_stepTotal = 30;
static bool g_dwelling = false;
static int g_dwellTicks = 0;
static int g_dwellTotal = 0;

// Timing
static LARGE_INTEGER g_startTime;
static LARGE_INTEGER g_perfFreq;
static int g_tickCount = 0;

// Glow overlay
static HWND g_overlayHwnd = nullptr;
static HDC g_overlayDC = nullptr;
static HBITMAP g_overlayBmp = nullptr;
static HBITMAP g_overlayOldBmp = nullptr;
static uint32_t* g_overlayPixels = nullptr;
static std::vector<float> g_falloffMap;

// Mouse hook
static HHOOK g_mouseHook = nullptr;

// RNG
static std::mt19937 g_rng;

// ============================================================================
// HSV to RGB Helper
// ============================================================================

void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    int i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    switch (i % 6) {
        case 0: r=v; g=t; b=p; break;
        case 1: r=q; g=v; b=p; break;
        case 2: r=p; g=v; b=t; break;
        case 3: r=p; g=q; b=v; break;
        case 4: r=t; g=p; b=v; break;
        case 5: r=v; g=p; b=q; break;
    }
}

// ============================================================================
// Utility
// ============================================================================

static float smoothstep(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static double getElapsedSeconds() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - g_startTime.QuadPart) / (double)g_perfFreq.QuadPart;
}

static float randFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(g_rng);
}

static int randInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(g_rng);
}

// ============================================================================
// GraphicsRuntime — GDI+ startup/shutdown
// ============================================================================

struct GraphicsRuntime {
    ULONG_PTR token = 0;
    GraphicsRuntime() {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&token, &input, nullptr);
    }
    ~GraphicsRuntime() {
        if (token) Gdiplus::GdiplusShutdown(token);
    }
};

// ============================================================================
// Cursor Restoration
// ============================================================================

static void restoreSystemCursors() {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, SPIF_SENDCHANGE);
    InterlockedExchange(&g_cursorChanged, 0);
}

static void atExitHandler() {
    if (InterlockedCompareExchange(&g_cursorChanged, 0, 0) != 0) {
        restoreSystemCursors();
    }
}

// ============================================================================
// Console Handler
// ============================================================================

static BOOL WINAPI consoleHandler(DWORD event) {
    (void)event;
    g_running = false;
    if (InterlockedCompareExchange(&g_cursorChanged, 0, 0) != 0) {
        restoreSystemCursors();
    }
    return TRUE;
}

// ============================================================================
// AgentCursor — custom cursor from akar-icons_cursor.png
// ============================================================================

struct AgentCursor {
    HCURSOR hCur = nullptr;

    static const DWORD cursorIds[14];

    void apply() {
        if (!hCur) return;
        for (int i = 0; i < 14; i++) {
            HCURSOR copy = CopyCursor(hCur);
            if (copy) {
                SetSystemCursor(copy, cursorIds[i]);
            }
        }
        InterlockedExchange(&g_cursorChanged, 1);
    }

    ~AgentCursor() {
        if (hCur) DestroyCursor(hCur);
    }
};

const DWORD AgentCursor::cursorIds[14] = {
    OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP,
    OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS, OCR_SIZEALL,
    OCR_NO, OCR_HAND, OCR_APPSTARTING, 32651 /*OCR_HELP (undocumented value)*/
};

static HCURSOR createAgentCursor() {
    // Load akar-icons_cursor.png from same directory as executable
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring ws(path);
    auto pos = ws.find_last_of(L"\\/");
    if (pos != std::wstring::npos) ws = ws.substr(0, pos + 1);
    ws += L"akar-icons_cursor.png";

    Gdiplus::Bitmap* src = Gdiplus::Bitmap::FromFile(ws.c_str());
    if (!src || src->GetLastStatus() != Gdiplus::Ok) {
        delete src;
        return nullptr;
    }

    // DPI-aware target size
    HDC screenDC = GetDC(nullptr);
    int dpi = GetDeviceCaps(screenDC, LOGPIXELSX);
    ReleaseDC(nullptr, screenDC);
    int size = MulDiv(32, dpi, 96);

    Gdiplus::Bitmap canvas(size, size, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics gfx(&canvas);
        gfx.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        gfx.DrawImage(src, 0, 0, size, size);
    }
    delete src;

    HBITMAP hbmp = nullptr;
    canvas.GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hbmp);
    if (!hbmp) return nullptr;

    HBITMAP hMask = CreateBitmap(size, size, 1, 1, nullptr);

    ICONINFO ii = {};
    ii.fIcon = FALSE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = hMask;
    ii.hbmColor = hbmp;

    HCURSOR hc = CreateIconIndirect(&ii);
    DeleteObject(hbmp);
    DeleteObject(hMask);
    return hc;
}

// ============================================================================
// MouseMovementLock — low-level hook blocks non-injected mouse moves
// ============================================================================

static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MOUSEMOVE) {
        MSLLHOOKSTRUCT* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        if (!(ms->flags & LLMHF_INJECTED)) {
            return 1; // block
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

struct MouseMovementLock {
    bool active = false;
    void engage() {
        if (active) return;
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, mouseProc, nullptr, 0);
        active = (g_mouseHook != nullptr);
    }
    void release() {
        if (!active) return;
        if (g_mouseHook) {
            UnhookWindowsHookEx(g_mouseHook);
            g_mouseHook = nullptr;
        }
        active = false;
    }
    ~MouseMovementLock() { release(); }
};

// ============================================================================
// Target Generation
// ============================================================================

static void generateTarget(float& tx, float& ty) {
    float edgeBias = 0.45f;
    if (g_pattern == Pattern::Work)  edgeBias = 0.15f;
    if (g_pattern == Pattern::Edge)  edgeBias = 0.70f;

    int margin = 60;
    float fw = (float)g_screenW;
    float fh = (float)g_screenH;

    if (randFloat(0.0f, 1.0f) < edgeBias) {
        // Edge target
        int side = randInt(0, 3);
        switch (side) {
            case 0: tx = randFloat((float)margin, fw - margin); ty = randFloat(5.0f, (float)margin); break;
            case 1: tx = randFloat((float)margin, fw - margin); ty = randFloat(fh - margin, fh - 5.0f); break;
            case 2: tx = randFloat(5.0f, (float)margin); ty = randFloat((float)margin, fh - margin); break;
            case 3: tx = randFloat(fw - margin, fw - 5.0f); ty = randFloat((float)margin, fh - margin); break;
        }
    } else {
        // Interior target (work biases toward center)
        if (g_pattern == Pattern::Work) {
            float cx = fw * 0.5f, cy = fh * 0.5f;
            tx = cx + randFloat(-fw * 0.3f, fw * 0.3f);
            ty = cy + randFloat(-fh * 0.3f, fh * 0.3f);
        } else {
            tx = randFloat((float)margin, fw - margin);
            ty = randFloat((float)margin, fh - margin);
        }
    }
    tx = std::clamp(tx, 2.0f, fw - 2.0f);
    ty = std::clamp(ty, 2.0f, fh - 2.0f);
}

// ============================================================================
// Bézier Path Setup
// ============================================================================

static void setupBezierPath() {
    float tx, ty;
    generateTarget(tx, ty);

    g_p0x = g_curX; g_p0y = g_curY;
    g_p3x = tx;     g_p3y = ty;

    float dx = g_p3x - g_p0x;
    float dy = g_p3y - g_p0y;
    float pathLen = std::sqrt(dx * dx + dy * dy);
    if (pathLen < 1.0f) pathLen = 1.0f;

    // Direction unit vector
    float ux = dx / pathLen;
    float uy = dy / pathLen;

    // Perpendicular
    float px = -uy;
    float py = ux;

    float r1 = randFloat(-0.4f, 0.4f) * pathLen;
    float r2 = randFloat(-0.4f, 0.4f) * pathLen;

    g_p1x = g_p0x + ux * pathLen * 0.33f + px * r1;
    g_p1y = g_p0y + uy * pathLen * 0.33f + py * r1;
    g_p2x = g_p0x + ux * pathLen * 0.67f + px * r2;
    g_p2y = g_p0y + uy * pathLen * 0.67f + py * r2;

    // Step count by speed
    switch (g_speed) {
        case Speed::Slow:   g_stepTotal = randInt(30, 60); break;
        case Speed::Normal: g_stepTotal = randInt(15, 39); break;
        case Speed::Fast:   g_stepTotal = randInt(8, 20);  break;
    }
    g_stepCurrent = 0;
    g_dwelling = false;
}

// ============================================================================
// Bézier Evaluation
// ============================================================================

static void evalBezier(float t, float& ox, float& oy) {
    float u = 1.0f - t;
    float uu = u * u;
    float uuu = uu * u;
    float tt = t * t;
    float ttt = tt * t;

    ox = uuu * g_p0x + 3.0f * uu * t * g_p1x + 3.0f * u * tt * g_p2x + ttt * g_p3x;
    oy = uuu * g_p0y + 3.0f * uu * t * g_p1y + 3.0f * u * tt * g_p2y + ttt * g_p3y;

    ox = std::clamp(ox, 0.0f, (float)(g_screenW - 1));
    oy = std::clamp(oy, 0.0f, (float)(g_screenH - 1));
}

// ============================================================================
// Overlay Window Setup
// ============================================================================

static const wchar_t* OVERLAY_CLASS = L"MouseWanderOverlay";

static LRESULT CALLBACK overlayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void buildFalloffMap() {
    int w = g_screenW, h = g_screenH;
    g_falloffMap.resize((size_t)w * h);

    // Use the original's proven Gaussian edge glow formula
    const double spread = std::clamp(std::min(w, h) * 0.075, 40.0, 140.0);
    const auto glow = [spread](double distance) {
        double d = distance / spread;
        return std::exp(-d * d * 2.0);
    };

    for (int y = 0; y < h; y++) {
        double vertical = (1.0 - glow(y)) * (1.0 - glow(h - 1 - y));
        for (int x = 0; x < w; x++) {
            double fade = 1.0 - vertical * (1.0 - glow(x)) * (1.0 - glow(w - 1 - x));
            float val = static_cast<float>(fade);
            if (val < 0.003f) val = 0.0f;
            g_falloffMap[(size_t)y * w + x] = val;
        }
    }
}

static void createOverlay() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = overlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = OVERLAY_CLASS;
    RegisterClassExW(&wc);

    g_overlayHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        OVERLAY_CLASS, L"",
        WS_POPUP,
        0, 0, g_screenW, g_screenH,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    // Create DIB section
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_screenW;
    bmi.bmiHeader.biHeight = -g_screenH; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    g_overlayDC = CreateCompatibleDC(screenDC);
    g_overlayBmp = CreateDIBSection(g_overlayDC, &bmi, DIB_RGB_COLORS,
                                     reinterpret_cast<void**>(&g_overlayPixels), nullptr, 0);
    g_overlayOldBmp = (HBITMAP)SelectObject(g_overlayDC, g_overlayBmp);
    ReleaseDC(nullptr, screenDC);

    buildFalloffMap();

    ShowWindow(g_overlayHwnd, SW_SHOWNOACTIVATE);
}

static void destroyOverlay() {
    if (g_overlayDC) {
        SelectObject(g_overlayDC, g_overlayOldBmp);
        DeleteObject(g_overlayBmp);
        DeleteDC(g_overlayDC);
        g_overlayDC = nullptr;
    }
    if (g_overlayHwnd) {
        DestroyWindow(g_overlayHwnd);
        g_overlayHwnd = nullptr;
    }
    UnregisterClassW(OVERLAY_CLASS, GetModuleHandleW(nullptr));
}

// ============================================================================
// GDI+ Rounded Rectangle Helper
// ============================================================================

static void addRoundedRect(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius) {
    float d = radius * 2.0f;
    path.AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

// ============================================================================
// Overlay Rendering (animated glow + HUD)
// ============================================================================

static void renderOverlay(double elapsed) {
    int w = g_screenW, h = g_screenH;
    size_t totalPixels = (size_t)w * h;

    // Pulse factor
    float pulse = 0.7f + 0.3f * (float)std::sin(elapsed * 1.8);

    // Determine glow color components
    float cr = 0.0f, cg = 0.0f, cb = 0.0f;
    float mix = 0.5f + 0.5f * (float)std::sin(elapsed * 0.7);

    switch (g_glowTheme) {
        case GlowTheme::Green:
            cr = 0.12f * (1.0f - mix * 0.3f);
            cg = 0.94f;
            cb = 0.48f + mix * 0.15f;
            break;
        case GlowTheme::Cyan:
            cr = 0.0f;
            cg = 0.75f;
            cb = 0.95f;
            break;
        case GlowTheme::Rainbow: {
            float hue = (float)std::fmod(elapsed * 0.08, 1.0);
            hsvToRgb(hue, 1.0f, 1.0f, cr, cg, cb);
            break;
        }
        case GlowTheme::Off:
            cr = 0.0f; cg = 0.0f; cb = 0.0f;
            break;
    }

    // Write pixels
    uint8_t rr = (uint8_t)(cr * 255.0f);
    uint8_t gg = (uint8_t)(cg * 255.0f);
    uint8_t bb = (uint8_t)(cb * 255.0f);

    for (size_t i = 0; i < totalPixels; i++) {
        float f = g_falloffMap[i];
        if (f <= 0.0f) {
            g_overlayPixels[i] = 0;
            continue;
        }
        uint8_t a = (uint8_t)(f * pulse * 0.72f * 255.0f);
        // Premultiplied alpha for UpdateLayeredWindow
        uint8_t pr = (uint8_t)((rr * a) / 255);
        uint8_t pg = (uint8_t)((gg * a) / 255);
        uint8_t pb = (uint8_t)((bb * a) / 255);
        g_overlayPixels[i] = ((uint32_t)a << 24) | ((uint32_t)pr << 16) | ((uint32_t)pg << 8) | pb;
    }

    // Draw HUD via GDI+
    {
        Gdiplus::Graphics gfx(g_overlayDC);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        // Build text
        std::wstring hudText;
        if (g_infinite) {
            hudText = L"ESC to quit \u2502 \u221E";
        } else {
            double remaining = (double)g_durationSec - elapsed;
            if (remaining < 0.0) remaining = 0.0;
            int secs = (int)remaining;
            hudText = L"ESC to quit \u2502 " + std::to_wstring(secs) + L"s";
        }

        Gdiplus::FontFamily fontFamily(L"Segoe UI");
        Gdiplus::Font font(&fontFamily, 11.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

        // Measure text
        Gdiplus::RectF textBounds;
        gfx.MeasureString(hudText.c_str(), -1, &font, Gdiplus::PointF(0, 0), &textBounds);

        float padX = 10.0f, padY = 5.0f;
        float pillW = textBounds.Width + padX * 2.0f;
        float pillH = textBounds.Height + padY * 2.0f;
        float pillX = 10.0f, pillY = 10.0f;

        Gdiplus::RectF pillRect(pillX, pillY, pillW, pillH);

        // Draw pill background
        Gdiplus::GraphicsPath pillPath;
        addRoundedRect(pillPath, pillRect, 6.0f);
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(200, 10, 10, 10));
        gfx.FillPath(&bgBrush, &pillPath);

        // Border color matches glow theme
        uint8_t br_r = rr, br_g = gg, br_b = bb;
        if (g_glowTheme == GlowTheme::Off) { br_r = 100; br_g = 100; br_b = 100; }
        Gdiplus::Pen borderPen(Gdiplus::Color(200, br_r, br_g, br_b), 1.0f);
        gfx.DrawPath(&borderPen, &pillPath);

        // Draw text
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
        gfx.DrawString(hudText.c_str(), -1, &font,
                        Gdiplus::PointF(pillX + padX, pillY + padY), &textBrush);
    }

    // Push to screen
    POINT ptSrc = {0, 0};
    POINT ptDst = {0, 0};
    SIZE sz = {(LONG)w, (LONG)h};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(g_overlayHwnd, nullptr, &ptDst, &sz, g_overlayDC, &ptSrc, 0, &blend, ULW_ALPHA);
}

// ============================================================================
// Timer Callback
// ============================================================================

static void CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD) {
    if (!g_running) {
        PostQuitMessage(0);
        return;
    }

    double elapsed = getElapsedSeconds();

    // Check duration
    if (!g_infinite && elapsed >= (double)g_durationSec) {
        g_running = false;
        PostQuitMessage(0);
        return;
    }

    // Check escape
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        g_running = false;
        PostQuitMessage(0);
        return;
    }

    g_tickCount++;

    // Mouse movement
    if (g_dwelling) {
        // Micro-jitter
        float jx = g_curX + randFloat(-0.5f, 0.5f);
        float jy = g_curY + randFloat(-0.5f, 0.5f);
        jx = std::clamp(jx, 0.0f, (float)(g_screenW - 1));
        jy = std::clamp(jy, 0.0f, (float)(g_screenH - 1));
        SetCursorPos((int)jx, (int)jy);

        g_dwellTicks++;
        if (g_dwellTicks >= g_dwellTotal) {
            setupBezierPath();
        }
    } else {
        g_stepCurrent++;
        float raw_t = (float)g_stepCurrent / (float)g_stepTotal;
        float t = smoothstep(raw_t);
        evalBezier(t, g_curX, g_curY);
        SetCursorPos((int)g_curX, (int)g_curY);

        if (g_stepCurrent >= g_stepTotal) {
            // Start dwelling
            g_dwelling = true;
            g_dwellTicks = 0;
            g_dwellTotal = randInt(20, 80); // 200-800ms
        }
    }

    // Render overlay at ~20fps
    if ((g_tickCount % GLOW_FPS_DIVISOR) == 0) {
        renderOverlay(elapsed);
    }
}

// ============================================================================
// CLI Parsing
// ============================================================================

static void printHelp() {
    printf(
        "Usage: matricX-cowork [options]\n\n"
        "Options:\n"
        "  --duration <sec>     Duration in seconds (default 60, 0 or 'infinite' = forever)\n"
        "  --speed <mode>       slow | normal | fast (default normal)\n"
        "  --pattern <mode>     wander | work | edge (default wander)\n"
        "  --glow <theme>       green | cyan | rainbow | off (default green)\n"
        "  --no-cursor          Skip custom cursor replacement\n"
        "  --no-lock            Don't block manual mouse movement\n"
        "  --restore-cursor     Restore default Windows cursors and exit\n"
        "  --help               Show this help\n"
        "  --version            Show version\n"
    );
}

static bool parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
        if (arg == "--version") {
            printf("%s\n", VERSION_STRING);
            return false;
        }
        if (arg == "--restore-cursor") {
            restoreSystemCursors();
            printf("System cursors restored.\n");
            return false;
        }
        if (arg == "--no-cursor") {
            g_noCursor = true;
            continue;
        }
        if (arg == "--no-lock") {
            g_noLock = true;
            continue;
        }
        if (arg == "--duration" && i + 1 < argc) {
            i++;
            std::string val = argv[i];
            if (val == "infinite" || val == "0") {
                g_infinite = true;
                g_durationSec = 0;
            } else {
                g_durationSec = std::atoi(argv[i]);
                if (g_durationSec <= 0) {
                    g_infinite = true;
                    g_durationSec = 0;
                }
            }
            continue;
        }
        if (arg == "--speed" && i + 1 < argc) {
            i++;
            std::string val = argv[i];
            if (val == "slow")       g_speed = Speed::Slow;
            else if (val == "fast")  g_speed = Speed::Fast;
            else                     g_speed = Speed::Normal;
            continue;
        }
        if (arg == "--pattern" && i + 1 < argc) {
            i++;
            std::string val = argv[i];
            if (val == "work")       g_pattern = Pattern::Work;
            else if (val == "edge")  g_pattern = Pattern::Edge;
            else                     g_pattern = Pattern::Wander;
            continue;
        }
        if (arg == "--glow" && i + 1 < argc) {
            i++;
            std::string val = argv[i];
            if (val == "cyan")         g_glowTheme = GlowTheme::Cyan;
            else if (val == "rainbow") g_glowTheme = GlowTheme::Rainbow;
            else if (val == "off")     g_glowTheme = GlowTheme::Off;
            else                       g_glowTheme = GlowTheme::Green;
            continue;
        }
        fprintf(stderr, "Unknown option: %s\n", arg.c_str());
        printHelp();
        return false;
    }
    return true;
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    if (!parseArgs(argc, argv)) return 0;

    // Register crash/exit handlers
    std::atexit(atExitHandler);
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    // DPI awareness
    SetProcessDPIAware();

    // Seed RNG
    g_rng.seed((unsigned)std::time(nullptr) ^ (unsigned)GetTickCount());

    // Init GDI+
    GraphicsRuntime gdiRuntime;

    // Screen dimensions
    g_screenW = GetSystemMetrics(SM_CXSCREEN);
    g_screenH = GetSystemMetrics(SM_CYSCREEN);

    // Get current cursor position as starting point
    POINT pt;
    GetCursorPos(&pt);
    g_curX = (float)pt.x;
    g_curY = (float)pt.y;

    // Custom cursor
    AgentCursor agentCursor;
    if (!g_noCursor) {
        agentCursor.hCur = createAgentCursor();
        if (agentCursor.hCur) {
            agentCursor.apply();
        }
    }

    // Mouse lock
    MouseMovementLock mouseLock;
    if (!g_noLock) {
        mouseLock.engage();
    }

    // Create overlay
    createOverlay();

    // Start timing
    QueryPerformanceFrequency(&g_perfFreq);
    QueryPerformanceCounter(&g_startTime);

    // Setup first path
    setupBezierPath();

    // Initial render
    renderOverlay(0.0);

    // Start timer
    SetTimer(g_overlayHwnd, TIMER_ID, TIMER_INTERVAL_MS, timerProc);

    printf("matricX-cowork active. Press ESC to stop.\n");
    if (g_infinite) printf("Duration: infinite\n");
    else            printf("Duration: %d seconds\n", g_durationSec);

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Cleanup
    KillTimer(g_overlayHwnd, TIMER_ID);
    destroyOverlay();
    mouseLock.release();

    if (InterlockedCompareExchange(&g_cursorChanged, 0, 0) != 0) {
        restoreSystemCursors();
    }

    printf("matricX-cowork stopped.\n");
    return 0;
}

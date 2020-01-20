#include <Window.hpp>
#include <Engine/UI/UI.hpp>

Window* g_window;

#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
    uiInput(hwnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_DESTROY:
		g_window->m_quit = true;
		PostQuitMessage(0);
		return 0;

    case WM_CREATE:
        return 0;
	}

    const float dt = 1.0f / 60.0f;
    Window::MouseMove mouse;

    bool hasMouseMessage = false;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        g_window->m_mouseKeys[0] = Window::KeyState::FirstDown;
        hasMouseMessage = true;
    }
    break;
    case WM_LBUTTONUP:
        g_window->m_mouseKeys[0] = Window::KeyState::FirstUp;
        hasMouseMessage = true;
        break;
    case WM_MOUSELEAVE:
        g_window->m_mouseKeys[0] = Window::KeyState::FirstUp;
        hasMouseMessage = true;
        break;
    case WM_MOUSEMOVE:
        hasMouseMessage = true;
        mouse.mouseX = GET_X_LPARAM(lParam);
        mouse.mouseY = GET_Y_LPARAM(lParam);
        mouse.hasMove = true;

        const float mouseSpeed = 10.0f * dt;

        if (g_window->m_cursorStick)
        {
            mouse.dx = float(mouse.mouseX - g_window->m_w / 2) * mouseSpeed;
            mouse.dy = float(g_window->m_h / 2 - mouse.mouseY) * mouseSpeed;
        }
        else
        {
            mouse.dx = float(mouse.mouseX - g_window->m_lastX) * mouseSpeed;
            mouse.dy = float(g_window->m_lastY - mouse.mouseY) * mouseSpeed;

            g_window->m_lastY = mouse.mouseY;
            g_window->m_lastX = mouse.mouseX;
        }

        if (g_window->m_cursorStick)
        {
            POINT pt;
            pt.x = g_window->m_w/2;
            pt.y = g_window->m_h/2;

            mouse.mouseX = pt.x;
            mouse.mouseY = pt.y;

            ClientToScreen(hwnd, &pt);
            SetCursorPos(pt.x, pt.y);
        }
        break;
    }

    if (hasMouseMessage)
    {
        for (auto it : g_window->m_mouseListeners)
        {
            it(g_window->m_mouseKeys, mouse);
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

Window::Window(int w, int h, HINSTANCE hInstance)
{
    for (int key = 0; key < 256; key++)
    {
        m_Keys[key] = KeyState::Up;
    }

    for (int key = 0; key < 3; key++)
    {
        m_mouseKeys[key] = KeyState::Up;
    }

	g_window = this;
	m_w = w;
	m_h = h;
	m_quit = false;
	WNDCLASSW wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = L"Main Window";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	m_hwnd = CreateWindowW(wc.lpszClassName, L"Main Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, w, h, NULL, NULL, hInstance, NULL);

}

void Window::peekMessages()
{
    for (int key = 7; key < 256; key++)
    {
        Window::KeyState& currentKey = m_Keys[key];
        const short unsigned int currentState = GetAsyncKeyState(key);
        const bool pressed = (currentState & 0x8000) != 0;

        switch (currentKey)
        {
        case Window::KeyState::Up:
            if (pressed) currentKey = Window::KeyState::FirstDown;
            break;
        case Window::KeyState::FirstDown:
            if (pressed) currentKey = Window::KeyState::Down;
            else currentKey = Window::KeyState::FirstUp;
            break;
        case Window::KeyState::Down:
            if (!pressed) currentKey = Window::KeyState::FirstUp;
            break;
        case Window::KeyState::FirstUp:
            if (!pressed) currentKey = Window::KeyState::Up;
            else currentKey = Window::KeyState::FirstDown;
            break;
        default:
            break;
        }
    }

    for (auto it : m_keysListeners)
    {
        it(m_Keys);
    }

	MSG msg;
    const int maxMessages(10);
    int processed(0);
	while (PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
        processed++;
        if (processed >= maxMessages)
        {
            break;
        }
	}

    bool newMessage = false;
    if (m_mouseKeys[0] == Window::KeyState::FirstDown)
    {
        m_mouseKeys[0] = Window::KeyState::Down;
        newMessage = true;
    }
    if (m_mouseKeys[0] == Window::KeyState::FirstUp)
    {
        m_mouseKeys[0] = Window::KeyState::Up;
        newMessage = true;
    }

    if (newMessage)
    {
        MouseMove m;
        for (auto it : g_window->m_mouseListeners)
        {
            it(g_window->m_mouseKeys, m);
        }
    }
}

void Window::setCursorVisibility(bool visible)
{
    if (m_cursorVisible != visible)
    {
        m_cursorVisible = visible;
        ShowCursor(visible);
    }
}

bool Window::isCursorVisible()
{
    return m_cursorVisible;
}

void Window::setStickCursorToCenter(bool stick)
{
    m_cursorStick = stick;
    if (m_cursorStick)
    {
        POINT pt;
        pt.x = m_w / 2;
        pt.y = m_h / 2;
        ClientToScreen(m_hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
    }
}

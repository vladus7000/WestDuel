#pragma once

#include <string>
#include <windows.h>
#include <functional>
#include <vector>
#include <array>

class Window
{
public:

    enum class KeyState
    {
        Up,
        FirstDown,
        Down,
        FirstUp
    };

#ifdef  _WIN32
    enum class Keys
    {
        Tab = VK_TAB,
        Enter = VK_RETURN,
        Esc = VK_ESCAPE,
        Space = VK_SPACE,
        Left = VK_LEFT,
        Up = VK_UP,
        Right = VK_RIGHT,
        Down = VK_DOWN,
        LShift = VK_LSHIFT,
        Ctrl = VK_CONTROL,
        //Alt,

        A = 0x41,
        D = 0x44,
        S = 0x53,
        W = 0x57,

        Num_1 = VK_NUMPAD1,
        Num_2 = VK_NUMPAD2,
        Num_3 = VK_NUMPAD3,
        Num_4 = VK_NUMPAD4,
        Num_5 = VK_NUMPAD5,

        MaxKeys
    };

    enum class MouseKeys
    {
        LeftButton,
        RightButton,
        MiddleButton,
    };

    struct MouseMove
    {
        int mouseX = 0;
        int mouseY = 0;
        float dx = 0.0f;
        float dy = 0.0f;
        bool hasMove = false;
    };

#endif //  _WIN32

    using PlatformMessageProc = std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)>;

	Window(int w, int h, HINSTANCE hInstance);

	void peekMessages();

	int getWidth() const { return m_w; }
	int getHeight() const { return m_h; }
	HWND getHWND() const { return m_hwnd; }

	bool shouldClose() const { return m_quit; }

    void setCursorVisibility(bool visible);
    bool isCursorVisible();
    void setStickCursorToCenter(bool stick);

    using KeysSignature = std::function<void(const std::array<KeyState, 256>& state)>;
    using MouseSignature = std::function<void(const std::array<KeyState, 3>& mouseKeys, const MouseMove& mouseMove)>;

    void addKeysListener(KeysSignature func) { m_keysListeners.push_back(func); }
    void addMouseListener(MouseSignature func) { m_mouseListeners.push_back(func); }

	Window(const Window& rhs) = delete;
	Window(Window&& rhs) = delete;
	Window& operator=(const Window& rhs) = delete;
	Window& operator=(Window&& rhs) = delete;

private:
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int m_w;
	int m_h;
	HWND m_hwnd;
    std::vector<PlatformMessageProc> m_userFunctions;
	volatile bool m_quit;
    bool m_cursorVisible = true;
    bool m_cursorStick = false;

    std::array<KeyState, 256> m_Keys;
    std::array<KeyState, 3> m_mouseKeys;
    int m_lastX = 0;
    int m_lastY = 0;
    std::vector<KeysSignature> m_keysListeners;
    std::vector<MouseSignature> m_mouseListeners;
};
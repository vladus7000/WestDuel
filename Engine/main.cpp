#include <Engine/Application.hpp>
#include <Engine/Window.hpp>
#include <Engine/World.hpp>
#include <Engine/Render/Renderer.hpp>
#include <Engine/Render/Resources.hpp>
#include <Engine/Physics/Physics.hpp>
#include <Engine/UI/UI.hpp>

CREATE_GAME_DECL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	Window mainWindow(800, 600, hInstance);
    Physics physic;
	World mainWorld(&physic);
	Resources resources;
	Renderer mainRenderer(mainWindow, resources);

	mainRenderer.setWorld(&mainWorld);
	mainWorld.initSun(resources);

    uiInit(&resources);

    Application* game = CREATE_GAME();

    EngineComponents components;
    components.renderer = &mainRenderer;
    components.resources = &resources;
    components.window = &mainWindow;
    components.world = &mainWorld;
    components.physics = &physic;

    game->engineStared(components);

	const float dt = 1.0f / 60.0f;

	while (!mainWindow.shouldClose())
	{
		mainWorld.updateSun(dt);
		mainWindow.peekMessages();
        game->logicalUpdate(dt);
        game->uiUpdate(dt);

        physic.step();
        physic.syncPhysicsTransformToRenderables();
		mainRenderer.beginFrame();
		mainRenderer.drawFrame(dt);
		uiDraw();
		mainRenderer.endFrame();
	}
	uiDeinit();
    game->engineStopped();
    delete game;

	return 0;
}
#include <Ren/Engine.h>
#include <vector>

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

class Game : public GameCore
{
public:
	Game(unsigned int width, unsigned int height) : GameCore(width, height) {}

	void Init()
	{
		ResourceManager::LoadTexture(ASSETS_DIR "awesomeface.png", true, "face");
	}
	void Delete()
	{
	}
	void ProcessInput()
	{
		if (Input->Pressed(Key::ESCAPE))
			Exit();
	}
	void Update(float dt)
	{
	}
	void Render()
	{
		glm::vec3 test(0.0f);
		ImGui::Begin("Hello, World!", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Hello, this is OpenGL template demo!");
		if (ImGui::Button("Exit"))
			Exit();
		ImGui::End();
	}
};

int main()
{
	GameCore* game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
	GameLauncher launcher(game);
	launcher.GuiTheme = ImGuiTheme::dark;
	launcher.Init().Launch();

	delete game;
	return 0;
}

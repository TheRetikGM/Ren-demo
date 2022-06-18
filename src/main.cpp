#include <Ren/Engine.h>
#include <vector>
#include <Ren/Renderer/Renderer.h>
#include "stb_image_write.h"

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

using namespace Ren;

class Game : public GameCore
{
	Ref<VertexArray> vao;
	Ref<TextureBatch> batch;

public:
	Game(unsigned int width, unsigned int height) : GameCore(width, height) {}

	void Init()
	{
		ResourceManager::LoadTexture(ASSETS_DIR "awesomeface.png", true, "face");
		ResourceManager::LoadShader(SHADERS_DIR "vertex.vert", SHADERS_DIR "fragment.frag", nullptr, "shader");

		float vertices[] = {
			-0.5f,  0.5f,  0.0f,
			 0.5f,  0.5f,  0.0,
			 0.5f, -0.5f,  0.0,
			-0.5f, -0.5f,  0.0,
		};
		float colors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f
		};
		uint32_t indices[] = {0, 1, 2, 0, 2, 3};

		auto vbo_pos = VertexBuffer::Create(vertices, sizeof(vertices));
		vbo_pos->SetLayout({{ 0, ShaderDataType::vec3 }});
		auto vbo_col = VertexBuffer::Create(colors, sizeof(colors));
		vbo_col->SetLayout({{ 1, ShaderDataType::vec3 }});
		auto ebo = ElementBuffer::Create(indices, sizeof(indices));
		vao = VertexArray::Create();
		vao->AddVertexBuffer(vbo_pos).AddVertexBuffer(vbo_col).SetElementBuffer(ebo);

		batch = std::make_shared<TextureBatch>();
		for (char c = 'a'; c <= 'f'; c++)
		{
			RawTexture tex = RawTexture::Load((ASSETS_DIR + std::string(1, c) + ".png").c_str());
			batch->AddTexture(tex);
			RawTexture::Delete(tex);
		}
		batch->Build();
	}
	void Delete()
	{
		batch->Delete();
	}
	void ProcessInput()
	{
		if (Input->Pressed(Key::ESCAPE))
			Exit();
		if (Input->Held(Key::LEFT_CONTROL) && Input->Pressed(Key::P))
		{
			auto fbo = Framebuffer();
			auto a = std::make_shared<FramebufferAttachment>();
			a->internalFormat = (AttachmentFormats)batch->Internal_format;
			a->storageType = StorageType::TEXTURE;
			a->bufferID = batch->ID;
			a->storage = *(dynamic_cast<Texture2D*>(batch.get()));
			fbo.AddAttachment(a);
			fbo.Generate(batch->Width, batch->Height);
			fbo.Bind();
			uint8_t channel_count = 4;
			uint8_t* data = new uint8_t[fbo.Width * fbo.Height * channel_count];
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glReadPixels(0, 0, fbo.Width, fbo.Height, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_flip_vertically_on_write(0);
			std::string filename = "test.png";
			stbi_write_png(filename.c_str(), fbo.Width, fbo.Height, channel_count, data, fbo.Width * channel_count);
			fbo.Unbind();
			delete[] data;
		}
	}
	void Update(float dt)
	{
	}
	void Render()
	{
		// Renderer::BeginScene();
		// ResourceManager::GetShader("shader").Use();
		// Renderer::Submit(vao);
		// Renderer::EndScene();
		// basic_renderer->RenderShape(br_Shape::circle, {10.0f, 10.0f}, {50.0f, 50.0f}, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		// Texture2D* tex = dynamic_cast<Texture2D*>(batch.get());
		// sprite_renderer->RenderSprite(*tex, glm::vec2(0.0f), glm::vec2(tex->Width, tex->Height) * 3.0f);
		int x = 0;
		for (int i = 0; i < 6; i++)
		{
			auto desc = batch->GetTextureDescriptor(i);
			sprite_renderer->RenderPartialSprite(*desc.pTexture, desc.offset, desc.size, glm::vec2(x, 10), glm::vec2(100.0f));
			x += 100.0f;
		}

		// ImGui::Begin("Hello, World!", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		// ImGui::Text("Hello, this is OpenGL template demo!");
		// if (ImGui::Button("Exit"))
		// 	Exit();
		// ImGui::End();
	}
	void OnResize() override 
	{
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

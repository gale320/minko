#include <ctime>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

scene::Node::Ptr
createPointLight(Vector3::Ptr color, Vector3::Ptr position, file::AssetLibrary::Ptr assets)
{
	auto pointLight = scene::Node::create("pointLight")
		->addComponent(PointLight::create(10.f))
		->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(position)))
		->addComponent(Surface::create(
			assets->geometry("quad"),
			material::Material::create()
				->set("diffuseMap",		assets->texture("texture/sprite-pointlight.png"))
				->set("diffuseTint",	Vector4::create(color->x(), color->y(), color->z(), 1.f)),
			assets->effect("effect/Sprite.effect")
		));
	pointLight->component<PointLight>()->color(color);
	pointLight->component<PointLight>()->diffuse(.1f);

	return pointLight;
}

float
hue2rgb(float p, float q, float t)
{
	if (t < 0.f)
		t += 1.f;
	if (t > 1.f)
		t -= 1.f;
	if (t < 1.f/6.f)
		return p + (q - p) * 6.f * t;
	if (t < 1.f/2.f)
		return q;
	if (t < 2.f/3.f)
		return p + (q - p) * (2.f/3.f - t) * 6.f;

	return p;
}

Vector3::Ptr
hslToRgb(float h, float s, float l)
{
    float r, g, b;

    if (s == 0)
        r = g = b = l; // achromatic
    else
	{
        float q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
        float p = 2.f * l - q;

        r = hue2rgb(p, q, h + 1.f/3.f);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.f/3.f);
    }

	return Vector3::create(r, g, b);
}

#ifndef EMSCRIPTEN
void
SDL_KeyboardHandler(scene::Node::Ptr		root,
					data::Provider::Ptr		data,
					file::AssetLibrary::Ptr	assets)
{
	static const std::string& normalMapPropName	= "normalMap";
	static const std::string& normalMapFilename	= "texture/normalmap-cells.png";

	static const uint MAX_NUM_LIGHTS	= 40;
	static std::vector<scene::Node::Ptr> newLights;

	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_SPACE])
	{
		bool hasNormalMap = data->hasProperty(normalMapPropName);

		std::cout << "mesh does" << (!hasNormalMap ? " not " : " ") << "have a normal map:\t" << (hasNormalMap ? "remove" : "add") << " it" << std::endl;
		if (hasNormalMap)
			data->unset(normalMapPropName);
		else
			data->set(normalMapPropName, assets->texture(normalMapFilename));
	}
	else if (keyboardState[SDL_SCANCODE_R])
	{
		auto lights = root->children()[4];

		if (lights->children().size() == 0)
		{
			std::cout << "no random light to remove" << std::endl;
			return;
		}

		lights->removeChild(lights->children().back());
	}
	else if (keyboardState[SDL_SCANCODE_A])
	{
		if (root->children()[4]->children().size() == MAX_NUM_LIGHTS)
		{
			std::cout << "cannot add more lights" << std::endl;
			return;
		}

		auto r = rand() / (float)RAND_MAX;
		auto theta = 2.0f * PI *  r;
		auto color = hslToRgb(r, 1.f, .5f);
		auto pos = Vector3::create(
			cosf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f),
			2.5f + rand() / (float)RAND_MAX,
			sinf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f)
		);

		root->children()[4]->addChild(createPointLight(color, pos, assets));
	}
}
#endif // EMSCRIPTEN

int main(int argc, char** argv)
{
	MinkoSDL::initialize("Minko Examples - Light", 800, 600);

	const clock_t startTime	= clock();

	auto sceneManager		= SceneManager::create(MinkoSDL::context());
	auto root				= scene::Node::create("root")->addComponent(sceneManager);
	auto sphereGeometry		= geometry::SphereGeometry::create(sceneManager->assets()->context(), 32, 32, true);
	auto sphereMaterial		= material::Material::create()
		->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f))
		->set("shininess",		32.f);
		//->set("normalMap",		assets->texture("texture/normalmap-cells.png")),
	auto lights				= scene::Node::create("lights");

	std::cout << "Press [SPACE]\tto toogle normal mapping\nPress [A]\tto add random light\nPress [R]\tto remove random light" << std::endl;

	sphereGeometry->computeTangentSpace(false);

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", sphereGeometry)
		->queue("texture/normalmap-cells.png")
		->queue("texture/sprite-pointlight.png")
		->queue("effect/Basic.effect")
		->queue("effect/Sprite.effect")
		->queue("effect/Phong.effect");

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		// camera
        auto camera	= scene::Node::create("camera")
			->addComponent(Renderer::create())
			->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(0.f, 2.f), Vector3::create(10.f, 10.f, 10.f))));
		//camera->component<Renderer>()->backgroundColor(0x7f7f7fff);
		root->addChild(camera);

		// ground
		auto ground = scene::Node::create("ground")
			->addComponent(Surface::create(
				assets->geometry("quad"),
				material::Material::create()
					->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f)),
					//->set("normalMap",		assets->texture("texture/window-normal.png")),
				assets->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendScale(50.f)->appendRotationX(-1.57f)));
		root->addChild(ground);

		// sphere
		auto sphere = scene::Node::create("sphere")
			->addComponent(Surface::create(
				assets->geometry("sphere"),
				sphereMaterial,
				assets->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(0.f, 2.f, 0.f)->prependScale(3.f)));
		root->addChild(sphere);

		// spotLight
		auto spotLight = scene::Node::create("spotLight")
			->addComponent(SpotLight::create(.15f, .4f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(15.f, 20.f, 0.f))));
		spotLight->component<SpotLight>()->diffuse(.4f);
		root->addChild(spotLight);

		lights->addComponent(Transform::create());
		root->addChild(lights);

		auto resized = MinkoSDL::resized()->connect([&](unsigned int width, unsigned int height)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);
		});

		auto keyDown = MinkoSDL::keyDown()->connect([&]()
		{
			const auto MAX_NUM_LIGHTS = 1000;

			if (lights->children().size() == MAX_NUM_LIGHTS)
			{
				std::cout << "cannot add more lights" << std::endl;
				return;
			}

			auto r = rand() / (float)RAND_MAX;
			auto theta = 2.0f * PI *  r;
			auto color = hslToRgb(r, 1.f, .5f);
			auto pos = Vector3::create(
				cosf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f),
				2.5f + rand() / (float)RAND_MAX,
				sinf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f)
			);

			lights->addChild(createPointLight(color, pos, sceneManager->assets()));

			std::cout << lights->children().size() << " lights" << std::endl;
		});
		auto enterFrame = MinkoSDL::enterFrame()->connect([&]()
		{
			lights->component<Transform>()->transform()->appendRotationY(.005f);

			sceneManager->nextFrame();

			//std::cout << MinkoSDL::framerate() << std::endl;
		});

		//for (auto i = 0; i < 10; ++i)
			//MinkoSDL::keyDown()->execute();

		MinkoSDL::run();

	});

	sceneManager->assets()->load();

	exit(EXIT_SUCCESS);
}

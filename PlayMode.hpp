#include "PPU466.hpp"
#include "Mode.hpp"
#include "AssetPipeline.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	void displayBackground(int levelIndex);
	void displayObstacle(int obsIndex);
	bool checkCollision(PPU466::Sprite player, PPU466::Sprite b, int8_t playerBuffer, bool death);

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	//some weird background animation:
	float background_fade = 0.0f;

	float totalTime = 0.0f;
	float waitTime = 1.0f;
	size_t currentWallCount = 0;
	float currentWallPos;
	float waitTimeD;

	float flashTime = 0.25f;
	float flashTimeD = 0.0f;

	bool flash = false;
	int score = 100;

	int totalLinesCleared = 0;

	//player position:
	glm::vec2 player_at = glm::vec2(50.0f, 200.0f);
	glm::vec2 player_vel = glm::vec2(0.0f);

	std::array<PPU466::Sprite, 15> windSprites = {};
	std::array<float, 15> windSpritesSpeedMult = {};

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
	AssetPipeline pipeline;
};

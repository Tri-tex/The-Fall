#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <corecrt_math_defines.h>

PlayMode::PlayMode(){
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	pipeline.setPPU(ppu);
	pipeline.loadSpriteSheet("../Assets/Spritesheet.png");
	pipeline.parseSprite(0, 0, 32);
	pipeline.parseSprite(1, 0, 35);
	pipeline.parseSprite(2, 0, 31);
	pipeline.parseSprite(10, 0, 29);

	for (int i = 0; i < 10; i++)
	{
		pipeline.parseSprite(i, 14, 15 + i);
	}

	pipeline.parseLevel(0);

	for (int i = 0; i < 14; i++)
	{
		pipeline.parseObstacle(i);
	}

	pipeline.parseSpriteNbyN(6,2,0,2,7);

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x8b, 0x5a, 0x2b, 0xff), 
		glm::u8vec4(0x4a, 0x3b, 0x32, 0xff), 
		glm::u8vec4(0x22, 0x8b, 0x22, 0xff) 
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[2] = {
		glm::u8vec4(0, 0, 0, 0),
		glm::u8vec4(255, 255, 0, 255),
		glm::u8vec4(15, 0, 240, 255),
		glm::u8vec4(0, 240, 0, 255),
	};

	ppu.palette_table[3] = {
		glm::u8vec4(0, 0, 0, 0),
		glm::u8vec4(220, 20, 20, 255),
		glm::u8vec4(60, 0, 90, 255),
		glm::u8vec4(0, 120, 30, 255),
	};

		ppu.palette_table[2] = {
		glm::u8vec4(0, 0, 0, 0),
		glm::u8vec4(255, 255, 0, 255),
		glm::u8vec4(15, 0, 240, 255),
		glm::u8vec4(0, 240, 0, 255),
	};

	ppu.palette_table[7] = {
		glm::u8vec4(0, 0, 0, 0),
		glm::u8vec4(70, 80, 95, 255),
		glm::u8vec4(150, 165, 180, 255),
		glm::u8vec4(235, 245, 255, 255),
	};

	ppu.palette_table[5] = {
		glm::u8vec4(0, 0, 0, 0),
		glm::u8vec4(0, 0, 0, 100),
		glm::u8vec4(0, 0, 0, 100),
		glm::u8vec4(255, 255, 255, 125),
	};

	ppu.palette_table[4] = {
		glm::u8vec4(0, 0, 0, 150),
		glm::u8vec4(0, 0, 0, 150),
		glm::u8vec4(0, 0, 0, 150),
		glm::u8vec4(0, 0, 0, 150),
	};

    for (size_t i = 0; i < windSprites.size(); ++i)
	{
        windSprites[i].x = uint8_t(40 + i * 30);
        windSprites[i].y = uint8_t(100 + i * 15);
        windSprites[i].index = 31;
        windSprites[i].attributes = 5;
		
		//Lerp applied to Random kinda cool
		windSpritesSpeedMult[i] = 0.5f + ((rand() % 100) / 100.0f) * (1.5f);
    }
	
	displayObstacle(0);
	waitTimeD = waitTime;
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_EVENT_KEY_DOWN) {
		if (evt.key.key == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		if (evt.key.key == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
		if (evt.key.key == SDLK_SPACE) {
			space.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed)
{
	if (flashTimeD >= 0)
	{
		flashTimeD -= 0.5f;
		elapsed *= 0.45f;
	}
	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed * 10.0f;
	//background_fade -= std::floor(background_fade);
	waitTimeD -= elapsed;
	flashTimeD -= elapsed;
	
	float accel = 650.0f;
	float friction = 600.0f;
	float maxS = 250.0f;

	float move_dir = 0.0f;
	if (left.pressed)  move_dir = -1.0f;
	if (right.pressed) move_dir = 1.0f;

	if (move_dir != 0.0f)
	{
		if (std::signbit(move_dir) == std::signbit(player_vel.x))
		{
 			player_vel.x += move_dir * accel * elapsed;
		}
		else
		{
 			player_vel.x += move_dir * accel * 3.5f * elapsed;
		}


		if (player_vel.x > maxS)  player_vel.x = maxS;
    	if (player_vel.x < -maxS) player_vel.x = -maxS;
	}
	else
	{
		if (player_vel.x > 0.0f)
		{
			player_vel.x -= friction * elapsed;
			if (player_vel.x < 0.0f) player_vel.x = 0.0f;
		}
		else if (player_vel.x < 0.0f)
		{
			player_vel.x += friction * elapsed;
			if (player_vel.x > 0.0f) player_vel.x = 0.0f;
		}

	}

	player_at += player_vel * elapsed;

	ppu.background_position.y += uint32_t(75.0f * elapsed);

	player_at.y += std::sin(totalTime * 6.0f) * 0.4f;

	for (size_t i = 0; i < windSprites.size(); i++)
	{
		windSprites[i].y += uint8_t(10.0f * 75.0f * windSpritesSpeedMult[i] * elapsed);
		windSprites[i].x += uint8_t(-60.0f * elapsed);

		if (windSprites[i].y > PPU466::ScreenHeight)
		{
        	windSprites[i].y = 0;
			windSprites[i].x = rand() % 256;
    	}
	}
	
	if (waitTimeD > 0.0f)
	{
		for (size_t i = windSprites.size() + 2; i < windSprites.size() + 2 + currentWallCount; i++)
		{
			currentWallPos -= (waitTimeD / waitTime) * 4.0f * elapsed;
			ppu.sprites[i].y = uint8_t(currentWallPos);
		}
	}

	if (waitTimeD <= 0.0f)
	{
		for (size_t i = windSprites.size() + 2; i < windSprites.size() + 2 + currentWallCount; i++)
		{
			ppu.sprites[i].y += uint8_t(450.0f * elapsed);
		}

		if (ppu.sprites[windSprites.size() + 2].y >= 240)
		{
			waitTimeD = waitTime;
			waitTime = std::max(0.2f, 1.0f - (totalLinesCleared / 60.0f));
			displayObstacle(rand() % 14);
			totalLinesCleared++;
			score += 100;
		}
	}

	for (size_t i = windSprites.size() + 2; i < windSprites.size() + 2 + currentWallCount; i++)
	{
		if (checkCollision(ppu.sprites[1], ppu.sprites[i], -2, true))
		{
			score -= 80;
		} 

		if (checkCollision(ppu.sprites[1], ppu.sprites[i], 8, false) && flashTimeD <= 0)
		{
			score += 1;
		}
	}

	if (flashTimeD >= 0)
	{
		flash = true;
	}
	else
	{
		flash = false;
	}


	totalTime += elapsed;

	if (score < 0){
		terminate();
	}
	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	if (flash)
	{
		ppu.background_color = glm::u8vec4(
			std::min(255,std::max(0,int32_t(200 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
			std::min(255,std::max(0,int32_t(200 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
			std::min(255,std::max(0,int32_t(200 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
			0xff
		);
	}
	else
	{
		ppu.background_color = glm::u8vec4(
			50,
			50,
			50,
			0xff
		);
	}


	//background scroll:
	ppu.background_position.x = int32_t(-0.2f * player_at.x);


	ppu.sprites[0].x = int8_t(player_at.x - 3);
	ppu.sprites[0].y = int8_t(player_at.y - 3);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 4;

	//player sprite:
	ppu.sprites[1].x = int8_t(player_at.x);
	ppu.sprites[1].y = int8_t(player_at.y);
	ppu.sprites[1].index = 32;
	ppu.sprites[1].attributes = 2;

	if (space.pressed) 
	{ 
		ppu.sprites[1].attributes = 3;
	}

	displayBackground(0);

	for (size_t i = 0; i < windSprites.size() && (i + 2) < 64; ++i)
	{
		ppu.sprites[i + 2] = windSprites[i];
	}

	int scoreRep = score;
	uint8_t i = 0;
	int currentXPos = 230;

	while (scoreRep > 0)
	{
		ppu.sprites[46 + i].index = 15 + (scoreRep % 10);
		ppu.sprites[46 + i].x = uint8_t(currentXPos - (i * 10));
		ppu.sprites[46 + i].y = 230;
		ppu.sprites[46 + i].attributes = 3;

		scoreRep /= 10;
		i++;
	}
	
	
	//--- actually draw ---
	ppu.draw(drawable_size);
}

void PlayMode::displayObstacle(int obsIndex)
{
	currentWallCount = 0;
	uint8_t tile_id = uint8_t(42 + obsIndex);
    PPU466::Tile const &tile = ppu.tile_table[tile_id];

    for (size_t x = 0; x < 32; ++x)
    {
        uint8_t bit0 = (tile.bit0[0] >> x / 4) & 1;
        uint8_t bit1 = (tile.bit1[0] >> x / 4) & 1;
        uint8_t color_index = (bit1 << 1) | bit0;

		size_t start = windSprites.size() + 2;
		
        if (color_index == 3)
        {
            ppu.sprites[start + currentWallCount].attributes = 7;
			ppu.sprites[start + currentWallCount].index = 29;
			ppu.sprites[start + currentWallCount].x = uint8_t(8 * x);
			ppu.sprites[start + currentWallCount].y = uint8_t(75);
			currentWallCount++;
        }
		else if (color_index == 1)
		{
            ppu.sprites[start + currentWallCount].attributes = 3;
			ppu.sprites[start + currentWallCount].index = 29;
			ppu.sprites[start + currentWallCount].x = uint8_t(8 * x);
			ppu.sprites[start + currentWallCount].y = uint8_t(75);
			currentWallCount++;
		}
    }

	currentWallPos = 75.0f;
}

void PlayMode::displayBackground(int levelIndex)
{

    uint8_t bl = uint8_t(7 + (4 * levelIndex));
    uint8_t br = uint8_t(8 + (4 * levelIndex));
    uint8_t tl = uint8_t(9 + (4 * levelIndex));
    uint8_t tr = uint8_t(10 + (4 * levelIndex));

    uint8_t palette_index = 6;

    auto bg_entry = [palette_index](uint8_t tile_idx) -> uint16_t {
        return uint16_t(tile_idx) | (uint16_t(palette_index) << 8);
    };

    for (size_t i = 0; i < PPU466::BackgroundWidth * PPU466::BackgroundHeight; ++i)
    {
        ppu.background[i] = 0;
    }

	for (int y = 0; y < PPU466::BackgroundHeight / 2; ++y)
	{
		for (int x = 0; x < PPU466::BackgroundWidth / 2; ++x)
		{
			int metaTileX = 2 * x;
			int metaTileY = 2 * y;
			ppu.background[(metaTileY) * PPU466::BackgroundWidth + (metaTileX)] = bg_entry(bl);
			ppu.background[(metaTileY) * PPU466::BackgroundWidth + (metaTileX + 1)] = bg_entry(br);
			ppu.background[(metaTileY + 1) * PPU466::BackgroundWidth + (metaTileX)] = bg_entry(tl);
			ppu.background[(metaTileY + 1) * PPU466::BackgroundWidth + (metaTileX + 1)] = bg_entry(tr);
			
		}
	}
}

bool PlayMode::checkCollision(PPU466::Sprite player, PPU466::Sprite b, int8_t playerBuffer, bool death)
{
	uint8_t aminX = player.x - playerBuffer;
	uint8_t amaxX = player.x + 8 + playerBuffer;
	uint8_t aminY = player.y - playerBuffer;
	uint8_t amaxY = player.y + 8 + playerBuffer;

	uint8_t bminX = b.x;
	uint8_t bmaxX = b.x + 8;
	uint8_t bminY = b.y;
	uint8_t bmaxY = b.y + 8;

	//blatently taken from jim Mccann's message on discord about AABB
	if (amaxX <= bminX || bmaxX <= aminX ||
        amaxY <= bminY || bmaxY <= aminY) {
        return false;
    }
	else
	{
		if (b.attributes == 3 && space.pressed)
		{
			return false;
		}

		if (death){
			flashTimeD = flashTime;
		}

	    return true;
	}
}

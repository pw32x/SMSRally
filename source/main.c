
#include <SMSlib.h>
#include <PSGlib.h>
#include "GSLib.h"

#include "bank2.h"

#include "resources.h"
#include "metatiles_lookup.h"

#include "base_types.h"
#include "base_defines.h"

#include "exported/animations/car001.h"
#include "exported/palettes/spritePalette.h"

#include "draw_utils.h"

void processSpritesActiveDisplay(void);
void processSpritesVBlank(void);
void processUserInput(void);

#define FIXED_POINT_FACTOR 4
// world to other spaces
#define W2S(x) (x >> FIXED_POINT_FACTOR) // world to screen
#define S2W(x) (x << FIXED_POINT_FACTOR) // screen to world

#define PLAYER_START_X 128
#define PLAYER_START_Y 128

#define PLAYER_SPEED 8
#define PLAYER_SPEED_LEFT (-PLAYER_SPEED)
#define PLAYER_SPEED_RIGHT (PLAYER_SPEED)
#define PLAYER_SPEED_UP (-PLAYER_SPEED)
#define PLAYER_SPEED_DOWN (PLAYER_SPEED)

s16 playerWorldX;
s16 playerWorldY;
s16 playerSpeedX;
s16 playerSpeedY;

s16 worldLimitX;
s16 worldLimitY;

s16 cameraX;
s16 cameraY;
s16 oldCameraX;
s16 oldCameraY;
s16 cameraDeltaX;
s16 cameraDeltaY;
s16 cameraLimitX;
s16 cameraLimitY;

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 16
#define PLAYER_HALF_WIDTH	8
#define PLAYER_HALF_HEIGHT	8

s16 playerMapMinX;
s16 playerMapMinY;
s16 playerMapMaxX;
s16 playerMapMaxY;

s16 playerScreenX;
s16 playerScreenY;

#define SPEED_FACTOR 4.0f
#define TO_SPEED(value) ((s16)(value * (1 << FIXED_POINT_FACTOR)))

#define MAKE_SPEED(value, x, y) \
value[0] = TO_SPEED(x * SPEED_FACTOR);\
value[1] = TO_SPEED(y * SPEED_FACTOR)

s16 iso_dir16_b[16][2];

void updateCameraPosition(void)
{
	cameraX = playerScreenX - SCREEN_HALF_WIDTH;
	cameraY = playerScreenY - SCREEN_HALF_HEIGHT;

	if (cameraX < 0) cameraX = 0;
	if (cameraY < 0) cameraY = 0;
	if (cameraX > cameraLimitX) cameraX = cameraLimitX;
	if (cameraY > cameraLimitY) cameraY = cameraLimitY;
}

void updatePlayerPhysics(void)
{
	playerWorldX += playerSpeedX;
	playerWorldY += playerSpeedY;

	if (playerWorldX < 0)
	{
		playerWorldX = 0;
	}

	if (playerWorldY < 0)
	{
		playerWorldY = 0;
	}

	if (playerWorldX >= worldLimitX)
	{
		playerWorldX = worldLimitX;
	}

	if (playerWorldY >= worldLimitY)
	{
		playerWorldY = worldLimitY;
	}

}

unsigned char scrolltable[testmap003_scrolltable_bin_size];

SMS_EMBED_SEGA_ROM_HEADER(9999,1);

void main(void)
{
	MAKE_SPEED(iso_dir16_b[0], 0.0f, 1.0f);
	MAKE_SPEED(iso_dir16_b[1], 0.382683f, 0.92388f);
	MAKE_SPEED(iso_dir16_b[2], 0.707107f, 0.707107f);
	MAKE_SPEED(iso_dir16_b[3], 0.92388f, 0.382683f);
	MAKE_SPEED(iso_dir16_b[4], 1.0f, -0.0f);
	MAKE_SPEED(iso_dir16_b[5], 0.92388f, -0.382683f);
	MAKE_SPEED(iso_dir16_b[6], 0.707107f, -0.707107f);
	MAKE_SPEED(iso_dir16_b[7], 0.382683f, -0.92388f);
	MAKE_SPEED(iso_dir16_b[8], 0.0f, -1.0f);
	MAKE_SPEED(iso_dir16_b[9], -0.382683f, -0.92388f);
	MAKE_SPEED(iso_dir16_b[10], -0.707107f, -0.707107f);
	MAKE_SPEED(iso_dir16_b[11], -0.92388f, -0.382683f);
	MAKE_SPEED(iso_dir16_b[12], -1.0f, 0.0f);
	MAKE_SPEED(iso_dir16_b[13], -0.92388f, 0.382683f);
	MAKE_SPEED(iso_dir16_b[14], -0.707107f, 0.707107f);
	MAKE_SPEED(iso_dir16_b[15], -0.382683f, 0.92388f);


	unsigned int ks;

	SMS_setSpriteMode(SPRITEMODE_TALL);

	SMS_VRAMmemset(0x4000, 0x00, 0x4000);
	SMS_VRAMmemset(0xC000, 0x00, 0x0020);
	SMS_loadTiles(&testmap003_tiles_bin, 0, testmap003_tiles_bin_size);
	SMS_loadBGPalette(&testmap003_palette_bin);
	//SMS_loadTiles(&sprite_tiles_bin, 256, sprite_tiles_bin_size);
	//SMS_loadSpritePalette(&sprite_palette_bin);

	SMS_loadSpritePalette(spritePalette.palette);


	//SMS_loadTiles(&testmap003_tiles_bin, 0, testmap003_tiles_bin_size);

	//SMS_loadTiles(car001.tileData, 256 + 4, car001.totalTileCount * 32);

	for(;;)
	{
		// copy scrolltable to RAM
		// NOTE: this allows us to modify the map for hightened player interaction
		// in this case cutting down lots of trees, activating bridges, etc.
		for (int i = 0; i < testmap003_scrolltable_bin_size; i++) 
		{
			scrolltable[i] = *(testmap003_scrolltable_bin + i);
		}

		// initalise General Scroll Library
		GSL_initializeMap(&scrolltable, &testmap003_metatiles_bin);

		playerMapMinX = PLAYER_WIDTH; // because the left column is hidden
		playerMapMinY = PLAYER_HALF_HEIGHT;
		playerMapMaxX = (GSL_getMapWidthInPixels() - 1) - PLAYER_HALF_WIDTH;
		playerMapMaxY = (GSL_getMapHeightInPixels() - 1) - PLAYER_HALF_HEIGHT;

		log("GSL_getMapWidthInPixels: %d\n", GSL_getMapWidthInPixels());
		log("GSL_getMapHeightInPixels: %d\n", GSL_getMapHeightInPixels());


		cameraLimitX = (GSL_getMapWidthInPixels() - 1) - SCREEN_WIDTH;
		cameraLimitY = (GSL_getMapHeightInPixels() - 1) - SCREEN_HEIGHT;


		playerScreenX = PLAYER_START_X;
		playerScreenY = PLAYER_START_Y;
		playerWorldX = S2W(PLAYER_START_X);
        playerWorldY = S2W(PLAYER_START_Y) << 1;

		worldLimitX = S2W(GSL_getMapWidthInPixels());
		worldLimitY = S2W(GSL_getMapHeightInPixels());

		updateCameraPosition();

		oldCameraX = cameraX;
		oldCameraY = cameraY;

		log("init\n");
		log("playerWorldX: %d\n", W2S(playerWorldX));
		log("playerWorldY: %d\n", W2S(playerWorldY));
		log("worldLimitX: %d\n", W2S(worldLimitX));
		log("worldLimitY: %d\n", W2S(worldLimitY));
		//log("cameraX: %d\n", cameraX);
		//log("cameraY: %d\n", cameraY);
		log("cameraLimitX: %d\n", cameraLimitX);
		log("cameraLimitY: %d\n", cameraLimitY);
		//log("playerScreenX: %d\n", playerScreenX);
		//log("playerScreenY: %d\n", playerScreenY);

		GSL_positionWindow(cameraX, cameraY);
		GSL_refreshVDP();

		SMS_VDPturnOnFeature(VDPFEATURE_HIDEFIRSTCOL);
		SMS_displayOn();

		//PSGPlay(&village_psg);



		for(;;)
		{
			//log("frame:\n");

			SMS_initSprites();
			ks = SMS_getKeysStatus();

			processUserInput();
			updatePlayerPhysics();

			playerScreenX = W2S(playerWorldX);
			playerScreenY = W2S(playerWorldY) >> 1;

			updateCameraPosition();

			cameraDeltaX = cameraX - oldCameraX;
			cameraDeltaY = cameraY - oldCameraY;

			oldCameraX = cameraX;
			oldCameraY = cameraY;


			
			//log("playerWorldX: %d\n", W2S(playerWorldX));
			//log("playerWorldY: %d\n", W2S(playerWorldY));
			//log("cameraX: %d\n", cameraX);
			//log("cameraY: %d\n", cameraY);
			//log("playerScreenX: %d\n", playerScreenX);
			//log("playerScreenY: %d\n", playerScreenY);

			GSL_scroll(cameraDeltaX, cameraDeltaY); // << GSL_scroll with offsets to scroll map.

			processSpritesActiveDisplay();

			SMS_waitForVBlank();
			GSL_VBlank();  // <<< Call GSL_VBlank to process any pending scroll / metatile updates.
			processSpritesVBlank();
			PSGFrame();
		}

		SMS_displayOff();
	}
}

#define START_CAR_DIRECTION 4
u8 carDirection = START_CAR_DIRECTION;
u8 carDirectionHigh = START_CAR_DIRECTION << 3;

void processSpritesActiveDisplay(void)
{
	// Add player sprite to sprite table for this frame.
	//SMS_addTwoAdjoiningSprites(playerScreenX - 8, playerScreenY - 8, 0);
	//SMS_addTwoAdjoiningSprites(playerScreenX - 8, playerScreenY, 2);

	DRAWUTILS_SETUP_BATCH((playerScreenX - cameraX) - 24, (playerScreenY - cameraY) - 24, car001.frames[carDirection]->spriteStrips, 4);
	DrawUtils_DrawBatchedStreamed();

	/*
	DRAWUTILS_SETUP_BATCH(object->screenx,
						  object->screeny,
						  object->currentBatchedAnimationFrame->spriteStrips,
						  *object->batchedAnimation->vdpLocation);
	*/
}

void UpdateStreamedBatchedAnimationFrame(void)
{
	//SMS_mapROMBank(gameObject->resourceInfo->bankNumber);



	//SMS_setBackdropColor(COLOR_ORANGE);
	const BatchedAnimationFrame* batchedAnimationFrame = car001.frames[carDirection];//gameObject->currentBatchedAnimationFrame;
	const BatchedAnimationSpriteStrip* runner = batchedAnimationFrame->spriteStrips;

	const BatchedAnimation* batchedAnimation = &car001;//gameObject->batchedAnimation;
	const u8* tileData = batchedAnimation->tileData;
	u16 vdpIndex = 256 + 4;//*batchedAnimation->vdpLocation;

	u16 tileCount = 0;

	const u8* tileOffset = tileData + (runner->tileIndex << 5);

	do 
	{
		tileCount += runner->count << 1;
		runner++;
	} while (runner->count);

	UNSAFE_SMS_loadNTiles(tileOffset, vdpIndex, tileCount);

	log("tileCount: %d\n", tileCount);

	// UNSAFE_SMS_VRAMmemcpy64(vdpIndex << 5, (const void *)tileOffset);
}



void processSpritesVBlank(void)
{
	// Update player sprites in VRAM. Tiles are updated each frame.
	UNSAFE_SMS_copySpritestoSAT();

	//UNSAFE_SMS_load4Tiles(sprite_tiles_bin + *(spriteTileOffsets + animationCount), 256);


	UpdateStreamedBatchedAnimationFrame();

}



// Test for user input and call appropriate method if so.
void processUserInput(void)
{
	unsigned int ks = SMS_getKeysStatus();

	playerSpeedX = 0;
	playerSpeedY = 0;


	if (ks & PORT_A_KEY_UP)
	{

	}
	else if (ks & PORT_A_KEY_DOWN)
	{
	}
	else if (ks & PORT_A_KEY_LEFT)
	{
		carDirection = ((++carDirectionHigh) >> 3) & 15;
	}
	else if (ks & PORT_A_KEY_RIGHT)
	{
		carDirection = ((--carDirectionHigh) >> 3) & 15;		
	}

	if (ks & PORT_A_KEY_2)
	{
		playerSpeedX = iso_dir16_b[carDirection][0];
		playerSpeedY = iso_dir16_b[carDirection][1];
	}

}

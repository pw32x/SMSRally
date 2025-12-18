
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

//static const s32 iso_dir16[16][2] =
//{
//    {      0, -65536 },   /*  0: S  */
//    {  26754, -59830 },   /*  1 */
//    {  46341, -46341 },   /*  2 */
//    {  59830, -26754 },   /*  3 */
//    {  65536,      0 },   /*  4: E  */
//    {  59830,  26754 },   /*  5 */
//    {  46341,  46341 },   /*  6 */
//    {  26754,  59830 },   /*  7 */
//    {      0,  65536 },   /*  8: N  */
//    { -26754,  59830 },   /*  9 */
//    { -46341,  46341 },   /* 10 */
//    { -59830,  26754 },   /* 11 */
//    { -65536,      0 },   /* 12: W  */
//    { -59830, -26754 },   /* 13 */
//    { -46341, -46341 },   /* 14 */
//    { -26754, -59830 }    /* 15 */
//};

//static const s16 iso_dir16[16][2] =
//{
//    { 0x0000, 0x0040 }, /*  0: S  (  0, +64) */
//    { 0x001A, 0x003A }, /*  1:     ( 26, +58) */
//    { 0x002D, 0x002D }, /*  2:     ( 45, +45) */
//    { 0x003A, 0x001A }, /*  3:     ( 58, +26) */
//    { 0x0040, 0x0000 }, /*  4: E   ( 64,   0) */
//    { 0x003A, 0xFFE6 }, /*  5 */
//    { 0x002D, 0xFFD3 }, /*  6 */
//    { 0x001A, 0xFFC6 }, /*  7 */
//    { 0x0000, 0xFFC0 }, /*  8: N   (  0, -64) */
//    { 0xFFE6, 0xFFC6 }, /*  9 */
//    { 0xFFD3, 0xFFD3 }, /* 10 */
//    { 0xFFC6, 0xFFE6 }, /* 11 */
//    { 0xFFC0, 0x0000 }, /* 12: W   (-64,   0) */
//    { 0xFFC6, 0x001A }, /* 13 */
//    { 0xFFD3, 0x002D }, /* 14 */
//    { 0xFFE6, 0x003A }  /* 15 */
//};


//static const s16 iso_dir16[16][2] =
//{
//    { 0x0000, 0x0020 }, /*  0: S  (  0, +32) */
//    { 0x000D, 0x001D }, /*  1:     ( 13, +29) */
//    { 0x0016, 0x0016 }, /*  2:     ( 22, +22) */
//    { 0x001D, 0x000D }, /*  3:     ( 29, +13) */
//    { 0x0020, 0x0000 }, /*  4: E   ( 32,   0) */
//    { 0x001D, 0xFFF3 }, /*  5 */
//    { 0x0016, 0xFFEA }, /*  6 */
//    { 0x000D, 0xFFE3 }, /*  7 */
//    { 0x0000, 0xFFE0 }, /*  8: N   (  0, -32) */
//    { 0xFFF3, 0xFFE3 }, /*  9 */
//    { 0xFFEA, 0xFFEA }, /* 10 */
//    { 0xFFE3, 0xFFF3 }, /* 11 */
//    { 0xFFE0, 0x0000 }, /* 12: W   (-32,   0) */
//    { 0xFFE3, 0x000D }, /* 13 */
//    { 0xFFEA, 0x0016 }, /* 14 */
//    { 0xFFF3, 0x001D }  /* 15 */
//};

static const s16 iso_dir16[16][2] =
{
    { 0x0000, 0x0010 }, /*  0: S  (  0, +16) */
    { 0x0005, 0x000F }, /*  1:     (  5, +15) */
    { 0x000A, 0x000E }, /*  2:     ( 10, +14) */
    { 0x000E, 0x000A }, /*  3:     ( 14, +10) */
    { 0x0010, 0x0000 }, /*  4: E   ( 16,   0) */
    { 0x000E, 0xFFF6 }, /*  5 */
    { 0x000A, 0xFFF2 }, /*  6 */
    { 0x0005, 0xFFF1 }, /*  7 */
    { 0x0000, 0xFFF0 }, /*  8: N   (  0, -16) */
    { 0xFFFB, 0xFFF1 }, /*  9 */
    { 0xFFF6, 0xFFF2 }, /* 10 */
    { 0xFFF2, 0xFFF6 }, /* 11 */
    { 0xFFF0, 0x0000 }, /* 12: W   (-16,   0) */
    { 0xFFF2, 0x000A }, /* 13 */
    { 0xFFF6, 0x000E }, /* 14 */
    { 0xFFFB, 0x000F }  /* 15 */
};

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
		playerSpeedX = iso_dir16[carDirection][0];
		playerSpeedY = iso_dir16[carDirection][1];
	}

}












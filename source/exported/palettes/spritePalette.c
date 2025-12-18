#include "spritePalette.h"

const u8 spritePaletteValues[16] = 
{
    0x04,
    0x00,
    0x15,
    0x2a,
    0x2a,
    0x20,
    0x34,
    0x38,
    0x03,
    0x0f,
    0x3f,
    0x33,
    0x33,
    0x33,
    0x33,
    0x33,
};

const PaletteResource spritePalette = 
{
    PALETTE_RESOURCE_TYPE, 
    spritePaletteValues,
};

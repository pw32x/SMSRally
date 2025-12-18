#ifndef GSLIB_INCLUDE_H
#define GSLIB_INCLUDE_H

void GSL_initializeMap(void *scrolltable, void *metatiles) __sdcccall(0);
void GSL_positionWindow(unsigned int X, unsigned int Y) __sdcccall(0);
unsigned int * GSL_metatileLookup(unsigned int x, unsigned int y) __sdcccall(0);
unsigned int GSL_tileLookup(unsigned int x, unsigned int y) __sdcccall(0);
void GSL_refreshVDP(void);
void GSL_scroll(signed char x, signed char y) __sdcccall(0);
void GSL_VBlank(void);
void GSL_metatileUpdate(void);
void GSL_metatileUpdateCustom(unsigned int x, unsigned int y, unsigned int offset) __sdcccall(0);
unsigned int GSL_getMapWidthInPixels(void);
unsigned int GSL_getMapHeightInPixels(void);
unsigned int GSL_getMapWidthInMetatiles(void);
unsigned int GSL_getMapHeightInMetatiles(void);
unsigned int GSL_getScrolltableSize(void);
unsigned int GSL_getCurrentX(void);
unsigned int GSL_getCurrentY(void);
unsigned char GSL_getCollisionCount(void);
unsigned char * GSL_getScrolltableAddress(void);

#endif
#pragma once

#include "windows.h"
#include <vector>
#include <string>
#include "TileTypes.h"

namespace SpriteMaster
{

class SpriteStripStore
{
public:
	class SpriteStrip
	{
	public:
		int count;
		int tileStartIndex;
		int frameStartIndex;
		TileSet tiles;
	};

	const SpriteStrip& AddOrGetSpriteStrip(const TileSet& stripTiles, 
										   bool removeDuplicates,
										   bool isStreamed,
										   int& frameTileCount);

	int GetTileCount() const { return m_tileCount; }

	void WriteTileStore(const std::string& outputName, 
						std::ofstream& sourceFile) const;

private:
	std::vector<SpriteStrip> m_spriteStrips;
	int m_tileCount = 0;
};


}
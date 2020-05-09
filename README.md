## LocalizeVMT

#### A tool which attempts to fix all broken paths recursively in a folder of VMTs.

## Author Information

* **Title**: LocalizeVMT
* **Written In**: C++
* **Date**: May 9th, 2020
* **Authors**: [polygonerror](https://github.com/PolygonError)

### Use

`LocalizeVMT <path-to-vmt-folder>`

The folder must be in the `materials` directory of a Source Engine game.

This was intended to be used to fix texture packs (I specifically made it to fix the Facepunch Asset Collaboration pack)

It will not work for all paths, as many may be in the game's files or simply do not exist.

It will save over the existing VMT, so make sure you have a backup because it could fuck up.

### To Build

This requires [VTFLib](https://github.com/NeilJed/VTFLib/) to build.

### Copyrights

LocalizeVMT
 * Copyright (c) 2020 polygonerror

VTFLib
 * Copyright (C) 2005-2010 Neil Jedrzejewski & Ryan Gregg

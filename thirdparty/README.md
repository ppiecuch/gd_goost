# Third party libraries

The following lists C/C++ libraries which are bundled and used by Goost.

## ExoQuant
- Upstream: https://github.com/exoticorn/exoquant
- Version: v0.7 (4ec54ab) (Feb 20, 2018)
- License: MIT

### Changes:
- removed `#include "malloc.h" as it's not available in iOS and MacOS.

## HQX
- Upstream: https://github.com/brunexgeek/hqx
- Version: git (e8088f0 2020), file structure differs
- License: Apache 2.0

### Notes
- Godot 3.2 includes `hq2x` version only. This module includes `hq3x` as well.

## Leptonica
- Upstream: https://github.com/DanBloomberg/leptonica
- Version: git (187250b), file structure differs (extracted `src`)
- License: [BSD-2-Clause](leptonica/leptonica-license.txt)

### Notes
- Function prototypes are generated inline in `allheaders.h` with the suggested
  `xtractprotos` utility to allow building only the necessary sources while also
  reducing binary size.

## Clipper
- Upstream: https://sourceforge.net/projects/polyclipping/
- Version: 10.0.0 r539 (sandbox)
- License: Boost Software License - Version 1.0

### Changes
- `Clipper.GetBounds` bug temporarily fixed:
    - https://sourceforge.net/p/polyclipping/bugs/176/
- Workaround for ill-formed `Clipper.PolyTree`, simply replaced with `Clipper.PolyPath`: 
    - https://sourceforge.net/p/polyclipping/bugs/177/
- Disabled exceptions (similarly to Godot's `clipper-no-exceptions.patch`)
- Removed Windows-specific debugging calls.
- Simple conditional version defines based on whether the older version was used.

See [`clipper.10.0.0-r539-no-except.patch`](clipper/clipper.10.0.0-r539-no-except.patch)
file for the exact changes.

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).

## Unreleased

## [0.1] - 2020-XX-XX

### Added

- `GoostImage` singleton for general-purpose image processing.
- `ImageIndexed` class which adds pseudo support for indexed images with ability
  to generate color palette.
- `ImageBlender` class for advanced image mixing (derived from OpenGL specification).
- `GoostGeometry2D` singleton for polygon clipping, offsetting, and
  decomposition methods, with various backends (Clipper, PolyPartition).
- `ShapeCast2D` node for physics collision sweep and immediate overlap queries.
- `VisualShape2D` node for quick prototyping and debugging of various collision shapes.
- GIF loading support via `ImageFrames` class and import as `AnimatedTexture`s.

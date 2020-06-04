# Image

The component provides `ImageExtension` singleton for easy image processing. It
aims to build on top of Godot's built-in `Image` class to cater specific user
needs.

### List of public methods:
| Method              | Short Description                                                           |
| ------------------- | --------------------------------------------------------------------------- |
| `replace_color`     | Replaces all occurrences of a given color with another one within an image. |
| `bucket_fill`       | Fills the area with a color confined by opaque pixels.                      |
| `resize_hqx`        | Expands the image using either HQ2X or HQ3X algorithms.                     |
| `has_pixel`         | Tells whether a pixel lies inside image coordinates.                        |
| `get_pixel_or_null` | Returns a pixel at specified image coordinates, or null if doesn't exist.   |

### List of classes:
| Class                                     | Short Description                                                             |
| ----------------------------------------- | ----------------------------------------------------------------------------- |
| [`ImageIndexed`](classes/ImageIndexed.md) | Adds pseudo support for indexed images with a color palette.                  |
| `ImageBlender`                            | Provides advanced image blending methods (derived from OpenGL specification). |
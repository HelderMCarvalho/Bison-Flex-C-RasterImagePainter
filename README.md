# Bison-Flex-C-RasterImagePainter
Program that paints Raster Images using Bison, Flex and C \
Must be used on Linux with Bison and Flex installed.

## Objective
Create Raster Images and paint them with shapes using commands.

## Commands
| Command | Description | Usage |
| --- | --- | --- |
| VAR | Creates a new variable <br> Creates a new variable with a random value up to the indicated maximum | VAR = `value`; <br> VAR = RAND `INT`; |
| NEW | Creates a new image with default color or given color | NEW `dimension`; <br> NEW `dimension` `color`; |
| SAVE | Saves the image | SAVE "`FILENAME`"; |
| LOAD | Loads the image into memory | LOAD "`FILENAME`"; |
| COLOR | Changes the default color | COLOR `color`; |
| REPEAT | Repeats commands a given number of times <br> (the `VAR` must be a **non existing** one) | REPEAT `VAR` IN [`value`..`value`] DO `(any commands)` END REPEAT';' |
| POINT | Paints a point at a coordinate with default color or given color | POINT `point`; <br> POINT `point` `color`; |
| LINE | Paints a line starting and ending at different given coordinates with default color or given color | LINE `point` `point`; <br> LINE `point` `point` `colot`; |
| RECT | Paints a rectangle starting and ending at different given coordinates **or** starting at given coordinate with given size with default color or given color | RECT `point` `pointOrDimension`; <br> RECT `point` `pointOrDimension` `color`; |
| RECTFILL | Paints a filled rectangle starting and ending at different given coordinates **or** starting at given coordinate with given size with default color or given color | RECTFILL `point` `pointOrDimension`; <br> RECTFILL `point` `pointOrDimension` `color`; |
| CIRC | Paints a circle with center at given coordinate and given radius value with default color or given color | CIRC `point` `value`; <br> CIRC `point` `value` `color`; |
| POLYLINE | Paints a polyline with default color or given color <br> Line between unlimited quantity of `point`'s (must be at least 2) | POLYLINE `polypoints`; <br> POLYLINE `polypoints` `color`; |


### Notation
| Expression | Description | Usage |
| --- | --- | --- |
| value | Value to be used in the execution of the command <br> Can be an integer or a variable | INT <br> VAR |
| dimension | Dimension to be used in the execution of the command | `value` x `value` |
| color | Color to be used in the execution of the command | `value` : `value` : `value` |
| FILENAME | Name of the final image (must type ".pnm") | example.pnm |
| point | Point to be used in the execution of the command <br> x and y coordinates | `value` , `value` |
| pointOrDimension | Point or Dimension to be used in the execution of the command | `dimension` <br> `point` |
| polypoints | Point's to be used in the execution of the command | `point` <br> `point` `polypoints` |

## Execution
1. Run `make`
2. Create a text file with the commands inside
3. Run `./logo < filenameYouJustCreated`

### Example of execution
```
NEW 100x100;
COLOR 0:0:0;
POLYLINE 10,90 10,10 50,50 90,10 90,90;
SAVE "polyline.pnm";
```

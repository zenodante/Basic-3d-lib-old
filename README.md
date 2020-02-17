# Basic-3d-lib
#### a 3d lab for cortex m4/7 mcu
![title image](/images/screenshot2.jpg)

![title image](/images/Annotation%202020-02-12%20225941.jpg)

This is a 3d lib for fast texture mapping 3d game on a mcu. Most in C with some asm functions to take advantage the cortex m fpu and SIMD.
Currently only texture mapping triangle drawing worked.

You could find configs in the b3dlib.h file, currently 32bit framebuff and 32bit z buff mode has been tested.

At init phase, call B3L_RenderInit(&B3Lrender,(frameBuff_t *)pFramebuffAddr); to init the lib

At the Render phase, call B3L_NewFrame( &B3Lrender) and B3L_RenderScence(&B3Lrender) to draw the frame. NewFrame function will clear the z buff and framebuff by default. Later I may change it to call some user callback function to customerize the framebuff cleaning operation.

All the obj in the lib should in the scene list. Only those in the render.scene.pActiveObjs list will be drawed. The per-defined objs buff number is defined in the h file.

Todo:
- [x] Texture mapping triangle drawing
- [ ] 2d Sprite drawing
- [ ] Polygon drawing
- [ ] 3d particles drawing
- [x] fast flat color triangle drawing

Model swapping system to manage the model loading from nor flash into sram buff.
Python scripts to convert obj model file to raw hex file which could be easily loaded into sram area.

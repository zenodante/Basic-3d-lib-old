#include "b3d2d.h"



_RAM_FUNC void B3L_DrawTiltmap(fBuff_t *pFBuff,B3L_tiltMap_t *pTiltMap,B3L_texture_t *pUVmap,
                            u32 screenX, u32 screenY,u32 lineSkip, u32 width, u32 height,u32 tiltMapX,u32 tiltMapY){
    fBuff_t *pDrawPixel = pFBuff + screenX + screenY*lineSkip; //the first draw pixel position
    texLUT_t *pLUT = pUVmap->pLUT;

    //calculate how many complete tilts in x and y
    u32 deltaTiltmapX = 8 - tiltMapX + ((tiltMapX>>3)<<3);
    u32 deltaDrawAreaX = width - ((width>>3)<<3);
    tiltMapX + deltaTiltmapX
}
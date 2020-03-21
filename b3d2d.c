#include "b3d2d.h"

#if B3L_ARM  == 1
__STATIC_FORCEINLINE s32   B3L_RoundingToS(f32 in){
    s32 result;
    __ASM ("vcvtr.s32.f32 %0,%1" : "=t"(result) : "t"(in));
    return result; 
}
#else
__STATIC_FORCEINLINE s32   B3L_RoundingToS(f32 in){
    return (s32)roundf(in); 
}
#endif
_RAM_FUNC void B3L_DrawTiltmap(fBuff_t *pFBuff,B3L_tiltMap_t *pTiltMap,B3L_texture_t *pUVmap,
                            u32 screenX, u32 screenY,u32 lineSkip, u32 width, u32 height,s32 tiltMapX,s32 tiltMapY){
    fBuff_t *pDrawPixel = pFBuff + screenX + screenY*lineSkip; //the first draw pixel position
    texLUT_t *pLUT = pUVmap->pLUT;
    u8 transIdx = ((pTiltMap->state)&TILT_TRANSPARENT_INDEX_MASK)>>TILT_TRANSPARENT_INDEX_SHIFT;

    //calculate how many complete tilts in x and y
    //u32 deltaTiltmapX = 8 - tiltMapX + ((tiltMapX>>3)<<3);
    //u32 deltaDrawAreaX = width - ((width>>3)<<3);
    //tiltMapX + deltaTiltmapX
    u32 xL;
    u32 xR;
    u32 yU;
    u32 yD;
    u32 column;
    u32 row;
}
//mode7 
__STATIC_FORCEINLINE void B3L_RastLineOnTiltmap_Repeat(fBuff_t *pFBuff,B3L_tiltMap_t *pTiltMap,B3L_texture_t *pUVmap,
                                     u32 screenX, u32 screenY, u32 lineSkip, u32 width,f32 tiltMapXS, f32 tiltMapXE, f32 tiltMapYS,f32 tiltMapYE){
    fBuff_t *pDrawPixel = pFBuff + screenX + screenY*lineSkip; //the first draw pixel position
    texLUT_t *pLUT = pUVmap->pLUT;

    f32 deltaX = (f32)(tiltMapXE-tiltMapXS);
    f32 deltaY = (f32)(tiltMapYE-tiltMapYS);
    f32 tiltX = tiltMapXS;
    f32 tiltY = tiltMapYS;
    //f32 tiltMapLength = B3L_Sqrtf(deltaX*deltaX + deltaY*deltaY);
    f32 stepX = deltaX/((f32)(width-1));
    f32 stepY = deltaY/((f32)(width-1));
    fBuff_t color;
    s32 intTiltX,intTiltY;
    u32 state  = pTiltMap->state;
    //u32 xRepeat = B3L_TEST(state,TILT_REPEAT_AT_X_DIRECTION);
    //u32 yRepeat = B3L_TEST(state,TILT_REPEAT_AT_Y_DIRECTION);
    f32 tiltMapXPixel = (f32)(pTiltMap->tiltsX*8);
    f32 tiltMapYPixel = (f32)(pTiltMap->tiltsY*8);
    u32 tiltMapX = pTiltMap->tiltsX;
    u8 *pTiltMapData = pTiltMap->data;
    u32 tiltIndex;
    u8 *uvData = pUVmap->pData;
    u8 colorIdx;
    texLUT_t *pLUT = pUVmap->pLUT;
    if (pUVmap->type == LUT256){
        while(width --){  
            if (tiltX<0){tiltX += tiltMapXPixel;}
            if (tiltX>=tiltMapXPixel){tiltX -= tiltMapXPixel;}
            if (tiltY<0){tiltY += tiltMapYPixel;}
            if (tiltY>=tiltMapYPixel){tiltY -= tiltMapYPixel;}
            intTiltX = B3L_RoundingToS(tiltX);
            intTiltY = B3L_RoundingToS(tiltY);
            //get the XY position color value
            tiltIndex = pTiltMapData[(intTiltX>>3)+(intTiltY>>3)*tiltMapX];
            //cal the uv map data offset

            colorIdx = uvData[((tiltIndex>>4)*128*8)+(tiltIndex&0x0f)*8+(intTiltX&0x07)+(intTiltY&0x07)*128];
            color = pLUT[colorIdx];
            *pDrawPixel = color;
            tiltX +=stepX;
            tiltY +=stepY;
            pDrawPixel++;
        }
    }else if (pUVmap->type == LUT16){
         while(width --){  
            if (tiltX<0){tiltX += tiltMapXPixel;}
            if (tiltX>=tiltMapXPixel){tiltX -= tiltMapXPixel;}
            if (tiltY<0){tiltY += tiltMapYPixel;}
            if (tiltY>=tiltMapYPixel){tiltY -= tiltMapYPixel;}
            intTiltX = B3L_RoundingToS(tiltX);
            intTiltY = B3L_RoundingToS(tiltY);
            //get the XY position color value
            tiltIndex = pTiltMapData[(intTiltX>>3)+(intTiltY>>3)*tiltMapX];
            //cal the uv map data offset
            colorIdx = uvData[((tiltIndex>>4)*64*8)+(tiltIndex&0x0f)*4+((intTiltX&0x07)>>1)+(intTiltY&0x07)*64];
            if(intTiltX&0x01){
                color = pLUT[colorIdx>>4];
            }else{
                color = pLUT[colorIdx&0x0F];
            }
            *pDrawPixel = color;
            tiltX +=stepX;
            tiltY +=stepY;
            pDrawPixel++;
        }
    }
}
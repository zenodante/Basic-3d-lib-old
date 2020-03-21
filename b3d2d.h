#ifndef __B3D_2D_H__
#define __B3D_2D_H__

#include "b3dlib.h"

/*
B3L_tiltMap_t state
   31     2423     1615      87
   ------------------------------------
31|$$$$$$$$|&&&&&&&&|********|******BA|0
  ------------------------------------
 
  A Repeat tilt map at x direction
  B Repeat tilt map at y direction
  & The outrange default index
  $ The transparent index
  

*/
#define TILT_REPEAT_AT_X_DIRECTION               (0)
#define TILT_REPEAT_AT_Y_DIRECTION               (1)
#define TILT_OUT_RANGE_INDEX_MASK         0x00FF0000
#define TILT_OUT_RANGE_INDEX_SHIFT              (16)
#define TILT_TRANSPARENT_INDEX_MASK       0xFF000000
#define TILT_TRANSPARENT_INDEX_SHIFT            (24)
typedef struct{
    u32    state;
    u16    tiltsX;//multi 8->pixel number
    u16    tiltsY;
    u8*    data;
}B3L_tiltMap_t;

//tilt coord could out of range
extern _RAM_FUNC void B3L_DrawTiltmap(fBuff_t *pFBuff,B3L_tiltMap_t *pTiltMap,B3L_texture_t *pUVmap,
                            u32 screenX, u32 screenY,u32 lineSkip, u32 width, u32 height,s32 tiltMapX,s32 tiltMapY);

//extern void     B3L_DrawHLine(s32 xa, s32 xb, s32 y, fBuff_t color);
//extern void     B3L_DrawBox(s32 xa, s32 xb, s32 ya, s32 yb, fBuff_t color);
//extern void     B3L_DrawVLine(s32 x, s32 ya, s32 yb, fBuff_t color);
//extern void     B3L_DrawChar(s32 x, s32 y, u8 c, fBuff_t fColor, fBuff_t bColor);
//extern void     B3L_SetFont(u32 index);
//extern void     B3L_SetFrontColor();
//extern void     B3L_SetBackColor();
//extern void     B3L_PrintStr();
//extern void     B3L_DrawLine();
//extern void     B3L_Draw


#endif
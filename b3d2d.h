#ifndef __B3D_2D_H__
#define __B3D_2D_H__

#include "b3dlib.h"



typedef struct{
    u16    tiltsX;//multi 8->pixel number
    u16    tiltsY;
    u8*    data;
}B3L_tiltMap_t;

extern _RAM_FUNC void B3L_DrawTiltmap(fBuff_t *pFBuff,B3L_tiltMap_t *pTiltMap,B3L_texture_t *pUVmap,
                            u32 screenX, u32 screenY,u32 lineSkip, u32 width, u32 height,u32 tiltMapX,u32 tiltMapY);

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
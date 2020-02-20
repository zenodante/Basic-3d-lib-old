#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "b3dlib.h"

#ifndef   __ASM
#define   __ASM                                  __asm
#endif

#pragma GCC optimize("-O3")


//config the ram position if necessary
screen3f_t     vectBuff[VECT_BUFF_SIZE]; //8KB
zBuff_t        zBuff[VIDEO_BUFF_LENTH];        //75KB

#ifdef B3L_USING_PARTICLE
B3L_Particle_t  particleBuff[B3L_PARTICLE_BUFF_DEPTH];
#endif

#define B3L_MATH_TABLE_SIZE      256
//not used fix math 
#define B3L_FIX_BITS            10


#if Z_BUFF_LEVEL == 2
    #define Z_LIMIT_NUM      (1.0f)
#endif
#if Z_BUFF_LEVEL == 1
    #define Z_LIMIT_NUM      (65535u)
#endif
#if Z_BUFF_LEVEL == 0
    #define Z_LIMIT_NUM      (255u)
#endif

#ifndef _swap_f32_t
#define _swap_f32_t(a, b) { f32 t = a; a = b; b = t; }
#endif
#ifndef _swap_int32_t
#define _swap_int32_t(a, b) { int32_t t = a; a = b; b = t; }
#endif

const u8 B3L_polygonIdx[24] = {
    0,1,0,2,1,3,2,3,0,4,1,5,2,6,3,7,4,5,4,6,5,7,6,7
};


const f32   B3L_boxVect[24] = {
    /* 0 front, bottom, right */
     0.5f, -0.5f, -0.5f,
     /* 1 front, bottom, left */
    -0.5f, -0.5f, -0.5f,
     /* 2 front, top,    right */
     0.5f,  0.5f, -0.5f,
     /* 3 front, top,    left */
    -0.5f,  0.5f, -0.5f,
     /* 4 back,  bottom, right */
     0.5f, -0.5f,  0.5f,
     /* 5 back,  bottom, left */
    -0.5f, -0.5f,  0.5f,
     /* 6 back,  top,    right */
     0.5f,  0.5f,  0.5f,
     /* 7 back,  top,    left */
    -0.5f,  0.5f,  0.5f
};

const u16 B3L_boxTri[36] ={
    3, 0, 2, /* front  */
    1, 0, 3,
    0, 4, 2, /* right  */
    2, 4, 6,
    4, 5, 6, /* back   */
    7, 6, 5,
    3, 7, 1, /* left   */
    1, 7, 5,
    6, 3, 2, /* top    */
    7, 3, 6,
    1, 4, 0, /* bottom */
    5, 4, 1
};

const u8 B3L_boxColorIdx[12]={
    0,1,2,3,4,5,
    6,7,8,9,10,11
};



#define m 15
const u8 B3L_boxUV[72]={
    0,0,  m,m,  m,0,\
    0,m,  m,m,  0,0,\
    m,m,  m,0,  0,m,\
    0,m,  m,0,  0,0,\
    m,0,  0,0,  m,m,\
    0,m,  m,m,  0,0,\
    0,0,  0,m,  m,0,\
    m,0,  0,m,  m,m,\
    0,0,  m,m,  m,0,\
    0,m,  m,m,  0,0,\
    m,0,  0,m,  m,m,\
    0,0,  0,m,  m,0
};
#undef m

const f32 B3L_boxNormal[36]={
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    1.0f,0.0f, 0.0f,
    1.0f,0.0f, 0.0f,
    0.0f,0.0f, 1.0f,
    0.0f,0.0f, 1.0f,
    -1.0f,0.0f, 0.0f,
    -1.0f,0.0f, 0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
};

#if (FRAME_BUFF_COLOR_TYPE == 0) ||(FRAME_BUFF_COLOR_TYPE == 2)
const u32 B3L_boxLUT32bit[16]={
    0XFF000000,0XFF1D2B53,0XFF7E2553,0XFF008751,
    0XFFAB5236,0XFF5F574F,0XFFC2C3C7,0XFFFFF1E8,
    0XFFFF004D,0XFFFFA300,0XFFFFEC27,0XFF00E436,
    0XFF29ADFF,0XFF83769C,0XFFFF77A8,0XFFFFCCAA
};
#endif
#if FRAME_BUFF_COLOR_TYPE == 1
const u16 B3L_boxLUT4444[16]={
    0XF000,0XF235,0XF825,0XF085,
    0XFB53,0XF655,0XFCCC,0XFFFF,
    0XFF05,0XFFA0,0XFFF2,0XF0E3,
    0XF3BF,0XF87A,0XFF7B,0XFFDB
};
#endif
#if FRAME_BUFF_COLOR_TYPE == 2
const u8 B3L_boxLUTL8Idx[16]={
    0,1,2,3,
    4,5,6,7,
    8,9,10,11,
    12,13,14,15
};
#endif

const u8 B3L_boxTexData[128]={
	0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X49, 
	0X14, 0X73, 0X77, 0XFF, 0XFF, 0X22, 0XD2, 0X49, 0X14, 0XA3, 0XAA, 0XAA, 0XAA, 0XAA, 0XDA, 0X49, 
	0X14, 0XB3, 0XBB, 0XBB, 0XBB, 0XBB, 0XDB, 0X49, 0X14, 0XC3, 0XCC, 0XCC, 0XCC, 0XCC, 0XDC, 0X49, 
	0X14, 0X83, 0X88, 0X88, 0X88, 0X88, 0XD8, 0X49, 0X14, 0XE3, 0XEE, 0XEE, 0XEE, 0XEE, 0XDE, 0X49, 
	0X14, 0XED, 0XEE, 0XEE, 0XEE, 0XEE, 0X5E, 0X49, 0X14, 0X8D, 0X88, 0X88, 0X88, 0X88, 0X58, 0X49, 
	0X14, 0XCD, 0XCC, 0XCC, 0XCC, 0XCC, 0X5C, 0X49, 0X14, 0XBD, 0XBB, 0XBB, 0XBB, 0XBB, 0X5B, 0X49, 
	0X14, 0XAD, 0XAA, 0XAA, 0XAA, 0XAA, 0X5A, 0X49, 0X14, 0X2D, 0X22, 0XFF, 0XFF, 0X77, 0X57, 0X49, 
	0X14, 0X11, 0X11, 0X11, 0X11, 0X11, 0X11, 0X44, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 
};
const B3L_Mesh_t B3L_box = {
                      .vectNum = 8,
                      .triNum  = 12,
                      .pVect   = (f32 *)B3L_boxVect,
                      .pTri    = (u16 *)B3L_boxTri,
                      .pUv     = (u8 *)B3L_boxUV,
                      .pNormal = (f32 *)B3L_boxNormal
                      };
const B3L_Mesh_NoTex_t B3L_box_noTex = {
                      .vectNum = 8,
                      .triNum  = 12,
                      .pVect   = (f32 *)B3L_boxVect,
                      .pTri    = (u16 *)B3L_boxTri,
                      .pColorIdx  = (u8  *)B3L_boxColorIdx,
                      .pNormal = (f32 *)B3L_boxNormal
                      };

const B3L_Polygon_t B3L_box_Polygon = {
                        .vectNum = 8,
                        .lineNum = 12,
                        .pVect = (f32 *)B3L_boxVect,
                        .pLine = (u8 *) B3L_polygonIdx
                        };


const B3L_texture_t B3L_boxTexture = { 
                                 .type    = LUT16,
                                 .uvSize  = 16,
                                 #if FRAME_BUFF_COLOR_TYPE == 0
                                 .pLUT    = (texLUT_t *)B3L_boxLUT32bit,
                                 #endif
                                 #if FRAME_BUFF_COLOR_TYPE == 1
                                 .pLUT    =  (texLUT_t *)B3L_boxLUT4444,
                                 #endif
                                 #if FRAME_BUFF_COLOR_TYPE == 2
                                 .pLUT    =  (texLUT_t *)B3L_boxLUTL8Idx,
                                 #endif
                                 .pData   = (u8 *)B3L_boxTexData,
                                 .transColorIdx = 16
};
/*Private Fuction declare ---------------------------------------------------*/ 
#ifdef B3L_ARM
__attribute__((always_inline)) static  inline f32   B3L_Sqrtf(f32 in);
__attribute__((always_inline)) static  inline f32   B3L_Absf(f32 in);

#else
#define B3L_Sqrtf   sqrtf
#define B3L_Absf    abs
#endif   
//math functions   
__attribute__((always_inline)) static  inline s32      VcvtF32ToS32_Fix(f32 in);
__attribute__((always_inline)) static  inline f32      VcvtS32ToF32_Fix(s32 in);
__attribute__((always_inline)) static  inline u32      SatToU8(u32 in);
__attribute__((always_inline)) static  inline u32      SatToU16(u32 in);
__attribute__((always_inline)) static  inline f32      NonZero(f32 value);
__attribute__((always_inline)) static  inline f32      Interp_f(f32 x1, f32 x2, f32 t);
__attribute__((always_inline)) static  inline s32      IntClamp(s32 v, s32 v1, s32 v2);
__attribute__((always_inline)) static  inline f32      B3L_Clamp_f(f32 v, f32 v1, f32 v2);
__attribute__((always_inline)) static  inline f32      Min_f(f32 v1, f32 v2);
__attribute__((always_inline)) static  inline s32      Min_s32(s32 v1, s32 v2);
__attribute__((always_inline)) static  inline u32      Min_u32(u32 v1, u32 v2);
__attribute__((always_inline)) static  inline f32      Max_f(f32 v1, f32 v2);
__attribute__((always_inline)) static  inline s32      Max_u32(u32 v1, s32 v2);
__attribute__((always_inline)) static  inline s32      Max_s32(s32 v1, s32 v2);
__attribute__((always_inline)) static  inline void     Vect3_Scale(vect3_t *pV,f32 scale,vect3_t *pResult);
__attribute__((always_inline)) static  inline void     Vect3_Add(vect3_t *pV1,vect3_t *pV2,vect3_t *pResult);
__attribute__((always_inline)) static  inline void     MakeClipMatrix(f32 focalLength, f32 aspectRatio,mat4_t *mat);
__attribute__((always_inline)) static  inline void     Vect3Xmat4(vect3_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline void     Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult);
__attribute__((always_inline)) static  inline void     Vect3Xmat4WithTest_f(vect3_t *pV, mat4_t *pMat, screen3f_t *pResult);
__attribute__((always_inline)) static  inline void     Norm3Xmat4(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void     Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void     Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline bool     Vect4BoundTest(vect4_t *pV);
__attribute__((always_inline)) static  inline f32      FastInvertSqrt(f32 x);
__attribute__((always_inline)) static  inline zBuff_t  CalZbuffValue(f32 z);
__attribute__((always_inline)) static  inline void     CopyMat4(mat4_t *target, mat4_t *source);
   
//light functions
__attribute__((always_inline)) static  inline void     UpdateLightVect(render_t *pRender);
__attribute__((always_inline)) static  inline u32      CalLightFactor(f32 normalDotLight, f32 lightFactor0,f32 lightFactor1);
//triangle testing functions   
__attribute__((always_inline)) static  inline bool     TriangleFaceToViewer(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2);
__attribute__((always_inline)) static  inline bool     TriangleFaceToViewer_f(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);
__attribute__((always_inline)) static  inline bool     TriVisable(u32 r0,u32 r1,u32 r2);
//draw functions   
__attribute__((always_inline)) static  inline fBuff_t  GetColorValue(texLUT_t *lut,u8 colorIdx,u32 lightFactor);
__attribute__((always_inline)) static  inline void     DrawPixel(fBuff_t color,s32 x,s32 y,f32 z,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff);
__attribute__((always_inline)) static  inline void     DrawPixelWithTest(fBuff_t color,s32 x,s32 y,f32 z,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff);                                                                        
__attribute__((always_inline)) static  inline void     DrawTriTexture(
                                                                        f32 x0,f32 y0,f32 u0,f32 v0,f32 z0,
                                                                        f32 x1,f32 y1,f32 u1,f32 v1,f32 z1,
                                                                        f32 x2,f32 y2,f32 u2,f32 v2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,B3L_texture_t *pTexture,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff);
__attribute__((always_inline)) static  inline void     DrawTriColor(
                                                                        f32 x0,f32 y0,f32 z0,
                                                                        f32 x1,f32 y1,f32 z1,
                                                                        f32 x2,f32 y2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,fBuff_t color,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff);
__attribute__((always_inline)) static  inline void     DrawTexHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
                                                                        f32 aU,f32 aV,f32 bU,f32 bV,u32 lightFactor, 
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff,
                                                                        B3L_texture_t *pTexture);
__attribute__((always_inline)) static  inline void     DrawColorHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
                                                                        fBuff_t finalColor, 
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff); 
__attribute__((always_inline)) static  inline void     DrawDepthLineNoClip(s32 Ax,s32 Ay,f32 Az,s32 Bx,s32 By,f32 Bz, 
                                                                        texLUT_t color,fBuff_t *pFrameBuff,zBuff_t *pZbuff);  
__attribute__((always_inline)) static  inline void     DrawDepthLineClip(s32 Ax,s32 Ay,f32 Az,s32 Bx,s32 By,f32 Bz, 
                                                                        texLUT_t color,fBuff_t *pFrameBuff,zBuff_t *pZbuff);             
//camera functions
static void SetCameraMatrix(camera_t *pCam);
//obj list functions
static void ResetObjList(scene_t *pScene);
static void AddObjToTwoWayList(B3LObj_t *pObj, B3LObj_t **pStart);
static void ResetParticleList(B3L_Particle_t *pPool,B3L_Particle_t **pStart,u32 num);
void B3L_ReturnParticleToPool(B3L_Particle_t *pParticle,scene_t *pScene);

//buff functions
static void ClearFrameBuff(fBuff_t *pFramebuff,fBuff_t value,u32 length);
static void ClearZbuff(zBuff_t *pZbuff,u32 length);
//Render call functions
static void RenderMeshObjs(render_t *pRender);
#ifdef B3L_USING_PARTICLE
static void UpdateParticleObjs(render_t *pRender, u32 time);
static void RenderParticleObjs(render_t *pRender);
#endif
static void RenderTexMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel);
static void RenderNoTexMesh(B3LMeshNoTexObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel);
static void RenderPolygon(B3LPolygonObj_t *pObj,render_t *pRender, mat4_t *pMat);


/*-----------------------------------------------------------------------------
Math function
-----------------------------------------------------------------------------*/
#ifdef B3L_ARM
__attribute__((always_inline)) static  inline f32 B3L_Sqrtf(f32 in){
    f32 result;
__ASM("vsqrt.f32 %0,%1" : "=t"(result) : "t"(in));
  return (result);
}
__attribute__((always_inline)) static  inline f32 B3L_Absf(f32 in){
    f32 result;
    __ASM("vabs.f32 %0,%1" : "=t"(result) : "t"(in));
    return (result); 
}
__attribute__((always_inline)) static  inline s32   VcvtF32ToS32_Fix(f32 in){
    s32 result;
    __ASM ("vcvt.s32.f32 %0,%1,#B3L_FIX_BITS" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline f32   VcvtS32ToF32_Fix(s32 in){
    s32 result;
    __ASM ("vcvt.f32.s32 %0,%1,#B3L_FIX_BITS" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline u32   SatToU8(u32 in){
    u32 result;
    __ASM ("usat %0,#8,%1" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline u32   SatToU16(u32 in){
    u32 result;
    __ASM ("usat %0,#16,%1" : "=t"(result) : "t"(in));
    return result; 
}

#else
__attribute__((always_inline)) static  inline s32   VcvtF32ToS32_Fix(f32 in){
    return ((s32)(in*((f32)(1<<B3L_FIX_BITS))));
}
__attribute__((always_inline)) static  inline f32   VcvtS32ToF32_Fix(s32 in){
    return ((f32)in)/((f32)(1<<B3L_FIX_BITS));
}
__attribute__((always_inline)) static  inline u32   SatToU8(u32 in){
    const uint32_t max = ((1U << 8) - 1U);
    if (in > max){
        return max;
    }else{
        return in;
    }
}
__attribute__((always_inline)) static  inline u32   SatToU16(u32 in){
    const uint32_t max = ((1U << 16) - 1U);
    if (in > max){
        return max;
    }else{
        return in;
    }
}

#endif

__attribute__((always_inline)) static  inline f32 B3L_Clamp_f(f32 v, f32 v1, f32 v2){
    return v > v1 ? (v < v2 ? v : v2) : v1;
}

__attribute__((always_inline)) static  inline s32 IntClamp(s32 v, s32 v1, s32 v2)
{
    return v >= v1 ? (v <= v2 ? v : v2) : v1;
}
__attribute__((always_inline)) static  inline u32 Min_u32(u32 v1, u32 v2){
    return v1 >= v2 ? v2 : v1;
}
__attribute__((always_inline)) static  inline s32 Min_s32(s32 v1, s32 v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static  inline s32 Max_u32(u32 v1, s32 v2){
    return v1 >= v2 ? v1 : v2;
}

__attribute__((always_inline)) static  inline s32 Max_s32(s32 v1, s32 v2){
    return v1 >= v2 ? v1 : v2;
}
__attribute__((always_inline)) static inline f32 Min_f(f32 v1, f32 v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static inline f32 Max_f(f32 v1, f32 v2){
    return v1 >= v2 ? v1 : v2;
}

//inv sqrt black magic from quake 
__attribute__((always_inline)) static  inline f32 FastInvertSqrt(f32 x){
    f32 xhalf = 0.5f * x;
        int i = *(int*)&x;            // store floating-point bits in integer
        i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
        x = *(float*)&i;              // convert new bits into float
        x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
        return x;
}

__attribute__((always_inline)) static inline f32 NonZero  (f32 value){
    if (value == 0.0f){
        return value + 0.00001f;
    }else{
        return value;
    }
}

__attribute__((always_inline)) static inline f32 Interp_f(f32 x1, f32 x2, f32 t){
    return x1 + (x2 - x1) * t;
}

__attribute__((always_inline)) static  inline void     Vect3_Scale(vect3_t *pV,f32 scale,vect3_t *pResult){
    pResult->x = scale*pV->x;
    pResult->y = scale*pV->y;
    pResult->z = scale*pV->z;
}

__attribute__((always_inline)) static  inline void     Vect3_Add(vect3_t *pV1,vect3_t *pV2,vect3_t *pResult){
    pResult->x = pV1->x + pV2->x;
    pResult->y = pV1->y + pV2->y;
    pResult->z = pV1->z + pV2->z;
}

/*
__attribute__((always_inline)) static inline void B3L_ClipToScreen(vect4Test_t *pV, screen4_t *pResult){
    f32 factor=1.0f / (pV->w);
    pResult->x = (int32_t)(HALF_RESOLUTION_X + pV->x *factor* HALF_RESOLUTION_X);
    pResult->y = (int32_t)(HALF_RESOLUTION_Y - pV->y *factor* HALF_RESOLUTION_Y);
    pResult->z = pV->z*factor;
}
*/
__attribute__((always_inline)) static inline void Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult){
    f32 x,y,z,w;
    x = pV->x;
    y = pV->y;
    z = pV->z;
    w = pV->w;
    #define dotCol(col)\
        ((x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) +\
        (w*(pMat->m##col##3)))
    
    pResult->x = dotCol(0);
    pResult->y = dotCol(1);
    pResult->z = dotCol(2);
    pResult->w = dotCol(3);
    #undef dotCol
}
__attribute__((always_inline)) static inline void Vect3Xmat4(vect3_t *pV, mat4_t *pMat, vect4_t *pResult){
    f32 x,y,z;
    x = pV->x;
    y = pV->y;
    z = pV->z;

    #define dotCol(col)\
        ((x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) +\
        (pMat->m##col##3))
    
    pResult->x = dotCol(0);
    pResult->y = dotCol(1);
    pResult->z = dotCol(2);
    pResult->w = dotCol(3);
    #undef dotCol

}
__attribute__((always_inline)) static  inline void  Vect3Xmat4WithTest_f(vect3_t *pV, mat4_t *pMat, screen3f_t *pResult){
    f32 x,y,z,rx,ry,rz,rw;
    x = pV->x;
    y = pV->y;
    z = pV->z;
    u32 testResult=0;
    #define dotCol(col)\
        ((x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) +\
        (pMat->m##col##3))
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);
    rw = dotCol(3);

    if (rz<0.0f){//if the near plane clip, then don't do the calculation, set bit and return directly
        B3L_SET(testResult,B3L_NEAR_PLANE_CLIP); 
        pResult->test = testResult;     
        return;
    }else{
        if((rx<=rw)&&(rx>=-rw)&&(ry<=rw)&&(ry>=-rw)&&(rz<=rw)){
            B3L_SET(testResult,B3L_IN_SPACE);
               
        }
    }
    
    f32 factor=1.0f / (rw);//rw won't be zero due we returned already rz<0 (rz>0, rw must >0)
    f32 intX = (HALF_RESOLUTION_X + rx *factor* HALF_RESOLUTION_X);
    f32 intY = (HALF_RESOLUTION_Y - ry *factor* HALF_RESOLUTION_Y);
    rz = rz*factor;
    pResult->test = testResult; 
    pResult->x = intX;
    pResult->y = intY;
    pResult->z = rz;
    #undef dotCol

}




__attribute__((always_inline)) static  inline void  Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult){
    f32 x,y,z,rx,ry,rz,rw;
    x = pV->x;
    y = pV->y;
    z = pV->z;
    u32 testResult=0;
    #define dotCol(col)\
        ((x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) +\
        (pMat->m##col##3))
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);
    rw = dotCol(3);

    if (rz<0.0f){//if the near plane clip, then don't do the calculation, set bit and return directly
        B3L_SET(testResult,B3L_NEAR_PLANE_CLIP); 
        pResult->test = testResult;     
        return;
    }else{
        if((rx<=rw)&&(rx>=-rw)&&(ry<=rw)&&(ry>=-rw)&&(rz<=rw)){
            B3L_SET(testResult,B3L_IN_SPACE);
            pResult->test = testResult;    
        }
    }

    f32 factor=1.0f / (rw);//rw won't be zero due we returned already rz<0 (rz>0, rw must >0)
    s32 intX = (int32_t)(HALF_RESOLUTION_X + rx *factor* HALF_RESOLUTION_X);
    s32 intY = (int32_t)(HALF_RESOLUTION_Y - ry *factor* HALF_RESOLUTION_Y);
    rz = rz*factor;

    pResult->x = intX;
    pResult->y = intY;
    pResult->z = rz;
    #undef dotCol

}

__attribute__((always_inline)) static  inline void  Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult){
    f32 x,y,z,rx,ry,rz,factor;
    x = pV->x;
    y = pV->y;
    z = pV->z;

    #define dotCol(col)\
        (x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) 
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);
    factor = FastInvertSqrt(rx * rx + ry * ry + rz * rz);
    pResult->x = rx*factor;
    pResult->y = ry*factor;
    pResult->z = rz*factor;
    #undef dotCol 

}

void     B3L_Vect3MulMat4(vect3_t *pV, mat4_t *mat, vect3_t *pResult){
    f32 x,y,z,rx,ry,rz;
    x = pV->x;
    y = pV->y;
    z = pV->z;

    #define dotCol(col)\
        (x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) 
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);

    pResult->x = rx;
    pResult->y = ry;
    pResult->z = rz;
    #undef dotCol 
}

void     B3L_Point3MulMat4(vect3_t *pV, mat4_t *mat, vect3_t *pResult){
    f32 x,y,z,rx,ry,rz;
    x = pV->x;
    y = pV->y;
    z = pV->z;

    #define dotCol(col)\
        (x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) +\
        pMat->m##col##3
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);

    pResult->x = rx;
    pResult->y = ry;
    pResult->z = rz;
    #undef dotCol
}

__attribute__((always_inline)) static  inline void  Norm3Xmat4(vect3_t *pV, mat4_t *pMat, vect3_t *pResult){
    f32 x,y,z,rx,ry,rz;
    x = pV->x;
    y = pV->y;
    z = pV->z;

    #define dotCol(col)\
        (x*(pMat->m##col##0)) +\
        (y*(pMat->m##col##1)) +\
        (z*(pMat->m##col##2)) 
    
    rx = dotCol(0);
    ry = dotCol(1);
    rz = dotCol(2);

    pResult->x = rx;
    pResult->y = ry;
    pResult->z = rz;
    #undef dotCol 
}
//todo
__attribute__((always_inline)) static  inline bool Vect4BoundTest(vect4_t *pV){
    f32 x,y,z,w;

    x=pV->x;
    y=pV->y;
    z=pV->z;
    w=pV->w;
    if((x<=w)&&(x>=-w)&&(y<=w)&&(y>=-w)&&(z>=0)&&(z<=w)){
        return true;
    }else{
        return false;
    }

}

__attribute__((always_inline)) static  inline void     CopyMat4(mat4_t *target, mat4_t *source){
    target->m00 =   source->m00;
    target->m01 =   source->m01;
    target->m02 =   source->m02;
    target->m03 =   source->m03;

    target->m10 =   source->m10;
    target->m11 =   source->m11;
    target->m12 =   source->m12;
    target->m13 =   source->m13;

    target->m20 =   source->m20;
    target->m21 =   source->m21;
    target->m22 =   source->m22;
    target->m23 =   source->m23;

    target->m30 =   source->m30;
    target->m31 =   source->m31;
    target->m32 =   source->m32;
    target->m33 =   source->m33;
    
}

vect2_t B3L_Vect2(f32 x,f32 y){
    vect2_t output = {.x=x,.y=y};
    return output;
}


vect3_t B3L_Vect3(f32 x,f32 y,f32 z){
    vect3_t output = {.x=x,.y=y,.z=z};
    return output;
}

vect4_t B3L_Vect4(f32 x,f32 y,f32 z,f32 w){
    vect4_t output = {.x=x,.y=y,.z=z,.w=w};
    return output;
}

//math functions
static const f32 sinTable_f32[B3L_MATH_TABLE_SIZE + 1] = {
      0.000000000f,0.024541229f,0.049067674f,0.073564564f,0.098017140f,0.122410675f,0.146730474f,0.170961889f,
      0.195090322f,0.219101240f,0.242980180f,0.266712757f,0.290284677f,0.313681740f,0.336889853f,0.359895037f,
      0.382683432f,0.405241314f,0.427555093f,0.449611330f,0.471396737f,0.492898192f,0.514102744f,0.534997620f,
      0.555570233f,0.575808191f,0.595699304f,0.615231591f,0.634393284f,0.653172843f,0.671558955f,0.689540545f,
      0.707106781f,0.724247083f,0.740951125f,0.757208847f,0.773010453f,0.788346428f,0.803207531f,0.817584813f,
      0.831469612f,0.844853565f,0.857728610f,0.870086991f,0.881921264f,0.893224301f,0.903989293f,0.914209756f,
      0.923879533f,0.932992799f,0.941544065f,0.949528181f,0.956940336f,0.963776066f,0.970031253f,0.975702130f,
      0.980785280f,0.985277642f,0.989176510f,0.992479535f,0.995184727f,0.997290457f,0.998795456f,0.999698819f,
      1.000000000f,0.999698819f,0.998795456f,0.997290457f,0.995184727f,0.992479535f,0.989176510f,0.985277642f,
      0.980785280f,0.975702130f,0.970031253f,0.963776066f,0.956940336f,0.949528181f,0.941544065f,0.932992799f,
      0.923879533f,0.914209756f,0.903989293f,0.893224301f,0.881921264f,0.870086991f,0.857728610f,0.844853565f,
      0.831469612f,0.817584813f,0.803207531f,0.788346428f,0.773010453f,0.757208847f,0.740951125f,0.724247083f,
      0.707106781f,0.689540545f,0.671558955f,0.653172843f,0.634393284f,0.615231591f,0.595699304f,0.575808191f,
      0.555570233f,0.534997620f,0.514102744f,0.492898192f,0.471396737f,0.449611330f,0.427555093f,0.405241314f,
      0.382683432f,0.359895037f,0.336889853f,0.313681740f,0.290284677f,0.266712757f,0.242980180f,0.219101240f,
      0.195090322f,0.170961889f,0.146730474f,0.122410675f,0.098017140f,0.073564564f,0.049067674f,0.024541229f,
      0.000000000f,-0.024541229f,-0.049067674f,-0.073564564f,-0.098017140f,-0.122410675f,-0.146730474f,-0.170961889f,
     -0.195090322f,-0.219101240f,-0.242980180f,-0.266712757f,-0.290284677f,-0.313681740f,-0.336889853f,-0.359895037f,
     -0.382683432f,-0.405241314f,-0.427555093f,-0.449611330f,-0.471396737f,-0.492898192f,-0.514102744f,-0.534997620f,
     -0.555570233f,-0.575808191f,-0.595699304f,-0.615231591f,-0.634393284f,-0.653172843f,-0.671558955f,-0.689540545f,
     -0.707106781f,-0.724247083f,-0.740951125f,-0.757208847f,-0.773010453f,-0.788346428f,-0.803207531f,-0.817584813f,
     -0.831469612f,-0.844853565f,-0.857728610f,-0.870086991f,-0.881921264f,-0.893224301f,-0.903989293f,-0.914209756f,
     -0.923879533f,-0.932992799f,-0.941544065f,-0.949528181f,-0.956940336f,-0.963776066f,-0.970031253f,-0.975702130f,
     -0.980785280f,-0.985277642f,-0.989176510f,-0.992479535f,-0.995184727f,-0.997290457f,-0.998795456f,-0.999698819f,
     -1.000000000f,-0.999698819f,-0.998795456f,-0.997290457f,-0.995184727f,-0.992479535f,-0.989176510f,-0.985277642f,
     -0.980785280f,-0.975702130f,-0.970031253f,-0.963776066f,-0.956940336f,-0.949528181f,-0.941544065f,-0.932992799f,
     -0.923879533f,-0.914209756f,-0.903989293f,-0.893224301f,-0.881921264f,-0.870086991f,-0.857728610f,-0.844853565f,
     -0.831469612f,-0.817584813f,-0.803207531f,-0.788346428f,-0.773010453f,-0.757208847f,-0.740951125f,-0.724247083f,
     -0.707106781f,-0.689540545f,-0.671558955f,-0.653172843f,-0.634393284f,-0.615231591f,-0.595699304f,-0.575808191f,
     -0.555570233f,-0.534997620f,-0.514102744f,-0.492898192f,-0.471396737f,-0.449611330f,-0.427555093f,-0.405241314f,
     -0.382683432f,-0.359895037f,-0.336889853f,-0.313681740f,-0.290284677f,-0.266712757f,-0.242980180f,-0.219101240f,
     -0.195090322f,-0.170961889f,-0.146730474f,-0.122410675f,-0.098017140f,-0.073564564f,-0.049067674f,-0.024541229f,
      0.000000000f
};
f32 B3L_sin(f32 in){
    f32 sinVal, fract;                   /* Temporary input, output variables */
  uint16_t index;                                /* Index variable */
  f32 a, b;                                /* Two nearest output values */
  int32_t n;
  f32 findex;

  /* input x is in radians */
  /* Scale input to [0 1] range from [0 2*PI] , divide input by 2*pi */


  /* Calculation of floor value of input */
  n = (int32_t) in;

  /* Make negative values towards -infinity */
  if (in < 0.0f)
  {
    n--;
  }

  /* Map input value to [0 1] */
  in = in - (f32) n;

  /* Calculation of index of the table */
  findex = (f32)B3L_MATH_TABLE_SIZE * in;
  index = (uint16_t)findex;

  /* when "in" is exactly 1, we need to rotate the index down to 0 */
  if (index >= B3L_MATH_TABLE_SIZE) {
    index = 0;
    findex -= (f32)B3L_MATH_TABLE_SIZE;
  }

  /* fractional value calculation */
  fract = findex - (f32) index;

  /* Read two nearest values of input value from the sin table */
  a = sinTable_f32[index];
  b = sinTable_f32[index+1];

  /* Linear interpolation process */
  sinVal = (1.0f - fract) * a + fract * b;

  /* Return output value */
  return (sinVal);

}

f32 B3L_cos(f32 in){
    return B3L_sin(in + 0.25f);
}

f32 B3L_asin(f32 in){
    in = B3L_Clamp_f(in, -1.0f,1.0f);
    float negate = (f32)(in < 0);
    
    if (in < 0.0f){
        in = -1.0*in;
    }
    float ret = -0.0187293f;
    ret *= in;
    ret += 0.0742610f;
    ret *= in;
    ret -= 0.2121144f;
    ret *= in;
    ret += 1.5707288f;
    //#ifdef B3L_ARM
    ret = 3.14159265358979f*0.5f - B3L_Sqrtf(1.0 - in)*ret;
    //#else
    //ret = 3.14159265358979f*0.5f - sqrtf(1.0 - in)*ret;
    //#endif
    ret = (ret - 2 * negate * ret)*0.15915494309f;
    //ret = sign + ret*0.3183098861837f;
    //printf("%.3f\n",ret);
    return ret;
}

void B3L_NormalizeVec3(vect3_t *pV){
    f32 factor = FastInvertSqrt(pV->x*pV->x+pV->y*pV->y+pV->z*pV->z);
    pV->x = (pV->x*factor);
    pV->y = (pV->y*factor);
    pV->z = (pV->z*factor);
}

f32 B3L_Vec2Length(vect2_t *pV){
    
    //#ifdef B3L_ARM
    return B3L_Sqrtf(pV->x*pV->x+pV->y*pV->y);
    //#else
    //return sqrtf(pV->x*pV->x+pV->y*pV->y);
   // #endif
}

f32 B3L_Vec3Length(vect3_t *pV){
    //#ifdef B3L_ARM
    return B3L_Sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);  
    //#else
    //return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);  
    //#endif
}

void B3L_Vec3Add(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc){
    pVc->x = pVa->x + pVb->x;
    pVc->y = pVa->y + pVb->y;
    pVc->z = pVa->z + pVb->z;
}

void B3L_VecInterp(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc,f32 t){
    pVc->x = Interp_f(pVa->x,pVb->x,t);
    pVc->y = Interp_f(pVa->y,pVb->y,t);
    pVc->z = Interp_f(pVa->z,pVb->z,t);
}

void B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult){
    pResult->x = pA->y * pB->z - pA->z * pB->y;
    pResult->y = pA->z * pB->x - pA->x * pB->z;
    pResult->z = pA->x * pB->y - pA->y * pB->x;  
}

f32 B3L_DotProductVect3(vect3_t *pA, vect3_t *pB){
    return (pA->x*pB->x+pA->y*pB->y+pA->z*pB->z);
}

//random function
u32 prng_lfsr = 0;
const u16 prng_tap = 0x74b8;

void   B3L_SetSeed(u32 seed){
    prng_lfsr = seed;
}
u32    B3L_Random(void){
    u8 lsb = prng_lfsr & 1;
    prng_lfsr = prng_lfsr>>1;
    if (lsb){
        prng_lfsr ^=prng_tap;
    }
    return prng_lfsr;
}
/*-----------------------------------------------------------------------------
Matrix functions
-----------------------------------------------------------------------------*/
__attribute__((always_inline)) static inline void MakeClipMatrix(f32 focalLength, f32 aspectRatio,mat4_t *mat){

    #define M(x,y) (mat)->m##x##y
    M(0,0) = focalLength; M(1,0) = 0.0f;   M(2,0) = 0.0f;   M(3,0) = 0.0f; 
    M(0,1) = 0.0f;   M(1,1) = focalLength*aspectRatio; M(2,1) = 0.0f;   M(3,1) = 0.0f; 
    M(0,2) = 0.0f;   M(1,2) = 0.0f;   M(2,2) = FAR_PLANE/(FAR_PLANE-NEAR_PLANE); M(3,2) = 1.0f; 
    M(0,3) = 0.0f;   M(1,3) = 0.0f;   M(2,3) =-1.0f*NEAR_PLANE*FAR_PLANE/(FAR_PLANE-NEAR_PLANE);   M(3,3) = 0.0f; 

    #undef M

}

void B3L_InitMat4One(mat4_t *pMat){
    #define M(x,y) (pMat)->m##x##y
    M(0,0) = 1.0f; M(1,0) = 0.0f; M(2,0) = 0.0f; M(3,0) = 0.0f; 
    M(0,1) = 0.0f; M(1,1) = 1.0f; M(2,1) = 0.0f; M(3,1) = 0.0f; 
    M(0,2) = 0.0f; M(1,2) = 0.0f; M(2,2) = 1.0f; M(3,2) = 0.0f; 
    M(0,3) = 0.0f; M(1,3) = 0.0f; M(2,3) = 0.0f; M(3,3) = 1.0f; 
    
    #undef M
}

void B3L_TransposeMat4(mat4_t *pMat){
    f32 temp;
    temp = ((f32 *)pMat)[1];
    ((f32 *)pMat)[1]=((f32 *)pMat)[4];
    ((f32 *)pMat)[4] = temp;

    temp = ((f32 *)pMat)[2];
    ((f32 *)pMat)[2]=((f32 *)pMat)[8];
    ((f32 *)pMat)[8] = temp;

    temp = ((f32 *)pMat)[6];
    ((f32 *)pMat)[6]=((f32 *)pMat)[9];
    ((f32 *)pMat)[9] = temp;
    
    temp = ((f32 *)pMat)[3];
    ((f32 *)pMat)[3]=((f32 *)pMat)[12];
    ((f32 *)pMat)[12] = temp;

    temp = ((f32 *)pMat)[7];
    ((f32 *)pMat)[7]=((f32 *)pMat)[13];
    ((f32 *)pMat)[13] = temp;

    temp = ((f32 *)pMat)[11];
    ((f32 *)pMat)[11]=((f32 *)pMat)[14];
    ((f32 *)pMat)[14] = temp;
}

//mat1 * mat2 -> mat1
void B3L_Mat4Xmat4(mat4_t *pMat1, mat4_t *pMat2){
    f32 t0,t1,t2,t3;
    #define M(x,y) (pMat1)->m##x##y
    #define N(x,y) (pMat2)->m##x##y
    t0 = M(0,0)*N(0,0)+M(1,0)*N(0,1)+M(2,0)*N(0,2)+M(3,0)*N(0,3);
    t1 = M(0,0)*N(1,0)+M(1,0)*N(1,1)+M(2,0)*N(1,2)+M(3,0)*N(1,3);
    t2 = M(0,0)*N(2,0)+M(1,0)*N(2,1)+M(2,0)*N(2,2)+M(3,0)*N(2,3);
    t3 = M(0,0)*N(3,0)+M(1,0)*N(3,1)+M(2,0)*N(3,2)+M(3,0)*N(3,3);
    M(0,0) = t0;
    M(1,0) = t1;
    M(2,0) = t2;
    M(3,0) = t3;
    
    t0 = M(0,1)*N(0,0)+M(1,1)*N(0,1)+M(2,1)*N(0,2)+M(3,1)*N(0,3);
    t1 = M(0,1)*N(1,0)+M(1,1)*N(1,1)+M(2,1)*N(1,2)+M(3,1)*N(1,3);
    t2 = M(0,1)*N(2,0)+M(1,1)*N(2,1)+M(2,1)*N(2,2)+M(3,1)*N(2,3);
    t3 = M(0,1)*N(3,0)+M(1,1)*N(3,1)+M(2,1)*N(3,2)+M(3,1)*N(3,3);
    M(0,1) = t0;
    M(1,1) = t1;
    M(2,1) = t2;
    M(3,1) = t3;

    t0 = M(0,2)*N(0,0)+M(1,2)*N(0,1)+M(2,2)*N(0,2)+M(3,2)*N(0,3);
    t1 = M(0,2)*N(1,0)+M(1,2)*N(1,1)+M(2,2)*N(1,2)+M(3,2)*N(1,3);
    t2 = M(0,2)*N(2,0)+M(1,2)*N(2,1)+M(2,2)*N(2,2)+M(3,2)*N(2,3);
    t3 = M(0,2)*N(3,0)+M(1,2)*N(3,1)+M(2,2)*N(3,2)+M(3,2)*N(3,3);
    M(0,2) = t0;
    M(1,2) = t1;
    M(2,2) = t2;
    M(3,2) = t3;

    t0 = M(0,3)*N(0,0)+M(1,3)*N(0,1)+M(2,3)*N(0,2)+M(3,3)*N(0,3);
    t1 = M(0,3)*N(1,0)+M(1,3)*N(1,1)+M(2,3)*N(1,2)+M(3,3)*N(1,3);
    t2 = M(0,3)*N(2,0)+M(1,3)*N(2,1)+M(2,3)*N(2,2)+M(3,3)*N(2,3);
    t3 = M(0,3)*N(3,0)+M(1,3)*N(3,1)+M(2,3)*N(3,2)+M(3,3)*N(3,3);
    M(0,3) = t0;
    M(1,3) = t1;
    M(2,3) = t2;
    M(3,3) = t3;
    #undef M
    #undef N
}

void B3L_MakeRotationMatrixZXY(f32 byX,f32 byY,f32 byZ,mat4_t *pMat){
    byX *= -1.0f;
    byY *= -1.0f;
    byZ *= -1.0f;

    f32 sx = B3L_sin(byX);
    f32 sy = B3L_sin(byY);
    f32 sz = B3L_sin(byZ);

    f32 cx = B3L_cos(byX);
    f32 cy = B3L_cos(byY);
    f32 cz = B3L_cos(byZ);
    #define M(x,y) (pMat)->m##x##y
    M(0,0) = (cy * cz)  + (sy * sx * sz);
    M(1,0) = (cx * sz) ;
    M(2,0) = (cy * sx * sz)  - (cz * sy);
    M(3,0) = 0.0f;

    M(0,1) = (cz * sy * sx)  - (cy * sz);
    M(1,1) = (cx * cz);
    M(2,1) = (cy * cz * sx)+ (sy * sz) ;
    M(3,1) = 0.0f;

    M(0,2) = (cx * sy) ;
    M(1,2) = -1.0f * sx;
    M(2,2) = (cy * cx) ;
    M(3,2) = 0.0f;

    M(0,3) = 0.0f;
    M(1,3) = 0.0f;
    M(2,3) = 0.0f;
    M(3,3) = 1.0f;
    #undef M
}

void B3L_MakeScaleMatrix(f32 scaleX,f32 scaleY,f32 scaleZ,mat4_t *pMat){
    #define M(x,y) (pMat)->m##x##y

    M(0,0) = scaleX; M(1,0) = 0.0f;   M(2,0) = 0.0f;   M(3,0) = 0.0f; 
    M(0,1) = 0.0f;   M(1,1) = scaleY; M(2,1) = 0.0f;   M(3,1) = 0.0f; 
    M(0,2) = 0.0f;   M(1,2) = 0.0f;   M(2,2) = scaleZ; M(3,2) = 0.0f; 
    M(0,3) = 0.0f;   M(1,3) = 0.0f;   M(2,3) = 0.0f;   M(3,3) = 1.0f; 

    #undef M
}

void B3L_MakeTranslationMat(f32 offsetX,f32 offsetY,f32 offsetZ,mat4_t *pMat){
    #define M(x,y) (pMat)->m##x##y
    f32 one = 1.0f;
    M(0,0) = one; M(1,0) = 0.0f; M(2,0) = 0.0f; M(3,0) = 0.0f; 
    M(0,1) = 0.0f; M(1,1) = one; M(2,1) = 0.0f; M(3,1) = 0.0f; 
    M(0,2) = 0.0f; M(1,2) = 0.0f; M(2,2) = one; M(3,2) = 0.0f; 
    M(0,3) = offsetX; M(1,3) = offsetY; M(2,3) = offsetZ; M(3,3) = one;
    #undef M
}

void B3L_MakeWorldMatrix(transform3D_t *pWorldTransform, mat4_t *pMat){
    mat4_t temp;
    B3L_MakeScaleMatrix(pWorldTransform->scale.x,
                        pWorldTransform->scale.y,
                        pWorldTransform->scale.z,pMat);
    
    B3L_MakeRotationMatrixZXY(pWorldTransform->rotation.x,
                                pWorldTransform->rotation.y,
                                pWorldTransform->rotation.z,&temp);

    B3L_Mat4Xmat4(pMat,&temp); 
    /*   
    B3L_MakeTranslationMat(pWorldTransform->translation.x,
                                pWorldTransform->translation.y,
                                pWorldTransform->translation.z,&temp);
    B3L_Mat4Xmat4(pMat,&temp);   
    */
    pMat->m03 = pWorldTransform->translation.x;
    pMat->m13 = pWorldTransform->translation.y;
    pMat->m23 = pWorldTransform->translation.z;
#ifdef B3L_DEBUG
printf("pWorldTransform %.3f,%.3f,%.3f,\n",pWorldTransform->rotation.x,pWorldTransform->rotation.y,pWorldTransform->rotation.z);
printf("In make world matrix, temp matrix:\n");
B3L_logMat4(temp);  
#endif  
              

}
__attribute__((always_inline)) static  inline zBuff_t CalZbuffValue(f32 z){
             #if  (Z_BUFF_LEVEL == 0) 
            u32 tempZ = (u32)(z*255.0f);
            u8  compZ = SatToU8(tempZ);
            #endif
            #if (Z_BUFF_LEVEL == 1)
            u32 tempZ = (u32)(z*65535.0f);
            u16 compZ = SatToU16(tempZ);
            #endif
            #if (Z_BUFF_LEVEL == 2)
            f32 compZ = z;           
            #endif 
            return compZ;
}
/*-----------------------------------------------------------------------------
Triangle testing functions
-----------------------------------------------------------------------------*/
__attribute__((always_inline)) static inline bool TriangleFaceToViewer_f(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2){
  f32 winding =
    (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) ;
    // ^ cross product for points with z == 0

  return winding >= 0.0f ? true : false;
}


__attribute__((always_inline)) static inline bool TriangleFaceToViewer(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2){
  int32_t winding =
    (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) ;
    // ^ cross product for points with z == 0

  return winding >= 0 ? true : false;
}

__attribute__((always_inline)) static  inline bool TriVisable(u32 r0,u32 r1,u32 r2){
    bool returnVal=true;
    //u32 nearPlaneCount = 0;
    //u32 inSpaceCount = 0;

    if(B3L_TEST(r0,B3L_IN_SPACE)||B3L_TEST(r1,B3L_IN_SPACE)||B3L_TEST(r2,B3L_IN_SPACE)){
        //test near plan
        if (B3L_TEST(r0,B3L_NEAR_PLANE_CLIP)||B3L_TEST(r1,B3L_NEAR_PLANE_CLIP)||B3L_TEST(r2,B3L_NEAR_PLANE_CLIP)){
            returnVal= false;
        }
    }else{
        returnVal= false;
    }
    return returnVal;
    
}
/*-----------------------------------------------------------------------------
light functions
-----------------------------------------------------------------------------*/
void B3L_ResetLight(light_t *pLight){
    B3L_CLR(pLight->state,LIGHT_TYPE_BIT); //parallel light
    pLight->lightVect.x = 0.0f;
    pLight->lightVect.y = 1.0f;
    pLight->lightVect.z = 0.0f;
    pLight->color = 0xFF000000;
    pLight->factor_0 = 1.01f; //make sure it is larger than 0, now the range is 0.01~2.01

#if LIGHT_BIT == 8
    pLight->factor_1 =126.7f;
#endif
#if LIGHT_BIT == 4
    pLight->factor_1 =7.91f;
#endif
}

__attribute__((always_inline)) static inline u32 CalLightFactor(f32 normalDotLight, f32 lightFactor0,f32 lightFactor1){
    s32 lightValue;
    normalDotLight += lightFactor0;
    lightValue =(s32) (normalDotLight*lightFactor1);
    lightValue = Max_s32(lightValue,0);
#if FRAME_BUFF_COLOR_TYPE == 1
    lightValue = lightValue>>4; //only use high 4 bit
#endif
    return lightValue;
}

__attribute__((always_inline)) static  inline void  UpdateLightVect(render_t *pRender){
    //if it is a point light type, then we need to calculate the current light position in camera space 
    if (B3L_TEST((pRender->light.state),LIGHT_TYPE_BIT)){
        Vect3Xmat4(&(pRender->light.lightVect), &(pRender->camera.camMat), &(pRender->light.pointLightVectInCamSpaceBuff));
    }else{
        Norm3Xmat4Normalize(&(pRender->light.lightVect), &(pRender->camera.camMat) , (vect3_t *)&(pRender->light.pointLightVectInCamSpaceBuff)); 
    }
}

void B3L_SetLightType(render_t *pRender,lightType_e type){
    if (parallelLight == type ){
        B3L_CLR(pRender->light.state,LIGHT_TYPE_BIT);
    }
    if (dotLight == type ){
        B3L_SET(pRender->light.state,LIGHT_TYPE_BIT);
    }
}

void B3L_SetLightVect(render_t *pRender, f32 x,f32 y,f32 z){
    pRender->light.lightVect.x = x;
    pRender->light.lightVect.y = y;
    pRender->light.lightVect.z = z;
}
/*-----------------------------------------------------------------------------
Camera functions
-----------------------------------------------------------------------------*/
void B3L_InitCamera(camera_t *pCam){
    pCam->aspectRate = DEFAULT_ASPECT_RATIO;
    pCam->focalLength = DEFAULT_FOCUS_LENGTH;
    pCam->transform.rotation.x = 0.0f;
    pCam->transform.rotation.y = 0.0f;
    pCam->transform.rotation.z = 0.0f;
    pCam->transform.scale.x = 1.0f;
    pCam->transform.scale.y = 1.0f;
    pCam->transform.scale.z = 1.0f;
    pCam->transform.translation.x = 0.0f;
    pCam->transform.translation.y = 0.0f;
    pCam->transform.translation.z = 0.0f;
    SetCameraMatrix(pCam);
}

static void SetCameraMatrix(camera_t *pCam){

    B3L_MakeTranslationMat(-1.0f * pCam->transform.translation.x,
                            -1.0f * pCam->transform.translation.y,
                            -1.0f * pCam->transform.translation.z,
                           &(pCam->camMat));
    mat4_t temp;
    B3L_MakeRotationMatrixZXY(pCam->transform.rotation.x,
                              pCam->transform.rotation.y,
                              pCam->transform.rotation.z,&(temp)); 
    B3L_TransposeMat4(&(temp));     
    B3L_Mat4Xmat4(&(pCam->camMat),&temp); 
    MakeClipMatrix(pCam->focalLength,pCam->aspectRate,&temp);
    B3L_Mat4Xmat4(&(pCam->camMat),&temp);   
            
}
void B3L_CameraMoveTo(vect3_t position,camera_t *pCam){
    pCam->transform.translation.x = position.x;
    pCam->transform.translation.y = position.y;
    pCam->transform.translation.z = position.z;
}


void B3L_CameraLookAt(camera_t *pCam, vect3_t *pAt){
    vect3_t v;

    v.x = pAt->x - pCam->transform.translation.x;
    v.y = pAt->z - pCam->transform.translation.z;

    f32 dx = v.x;
    f32 l = B3L_Vec2Length((vect2_t *)(&v));

    dx = v.x / NonZero(l); // normalize

    pCam->transform.rotation.y = -1.0f * B3L_asin(dx);

    if (v.y < 0){
        pCam->transform.rotation.y = 0.5f - pCam->transform.rotation.y ;
    }
    

    v.x = pAt->y - pCam->transform.translation.y;
    v.y = l;
 
    l = B3L_Vec2Length((vect2_t *)(&v));
 
    dx = v.x / NonZero(l);

    pCam->transform.rotation.x = B3L_asin(dx);
    //printf("rotation y %.3f,rotation x %.3f\n",pCam->transform.rotation.y,pCam->transform.rotation.x);
}




/*-----------------------------------------------------------------------------
obj functions
-----------------------------------------------------------------------------*/
#ifdef B3L_USING_PARTICLE
static void UpdateParticleObjs(render_t *pRender, u32 time){
    B3LObj_t  *pCurrentObj =(pRender->scene.pActiveParticleGenObjs);
    u32 state;
    mat4_t mat;
    u32 i;

    //switch(state & OBJ_TYPE_MASK)
    while(pCurrentObj != (B3LObj_t  *)NULL){
        state = pCurrentObj->state;
        //only update those active particle generater
        if (B3L_TEST(state ,OBJ_PARTICLE_ACTIVE)==0){  //obj active is fail
            pCurrentObj = pCurrentObj->next;
            continue;
        }
        //Create generator -> world matrix into mat1
//TODO: generate matrix
        //if it has a mother obj
        if(B3L_TEST(pCurrentObj->state,OBJ_USING_CUSTOMERIZE_MAT)){
            ((B3LParticleGenObj_t *)pCurrentObj)->PtlUpdFunc(time,(B3LParticleGenObj_t *)pCurrentObj,
                                                         pCurrentObj->pCustMat,&(pRender->scene.freeParticleNum),
                                                        pRender->scene.pfreeParticles);
        }else{
            f32 x = ((B3LParticleGenObj_t *)pCurrentObj)->rotation.x;
            f32 y = ((B3LParticleGenObj_t *)pCurrentObj)->rotation.y;
            f32 z = ((B3LParticleGenObj_t *)pCurrentObj)->rotation.z;
            B3L_MakeRotationMatrixZXY(x,y,z,&mat);
            mat.m03 = ((B3LParticleGenObj_t *)pCurrentObj)->translation.x;
            mat.m13 = ((B3LParticleGenObj_t *)pCurrentObj)->translation.y;
            mat.m23 = ((B3LParticleGenObj_t *)pCurrentObj)->translation.z; 
            ((B3LParticleGenObj_t *)pCurrentObj)->PtlUpdFunc(time,(B3LParticleGenObj_t *)pCurrentObj,
                                                         &mat,pRender);   
        }
        pCurrentObj = pCurrentObj->next;    
    }
}

static void RenderParticleObjs(render_t *pRender) {
    B3LObj_t  *pCurrentObj =(pRender->scene.pActiveParticleGenObjs);
    u32 state;
    mat4_t *pCamMat = &(pRender->camera.camMat);
    screen3_t screenVect;
    u32 i;
    B3L_Particle_t *pParticle;
    fBuff_t *pFBuff = pRender->pFrameBuff;
    zBuff_t *pZBuff = pRender->pZBuff;

    
    while(pCurrentObj != (B3LObj_t  *)NULL){
        B3L_DrawFunc_t DrawFunc = ((B3LParticleGenObj_t *)pCurrentObj)->DrawFunc;
        state = pCurrentObj->state;
        i =((B3LParticleGenObj_t *)pCurrentObj)->particleNum;
        if ((B3L_TEST(state ,OBJ_VISUALIZABLE)==0)||(i == 0)){  //obj visual is false, or no particle is alive
            pCurrentObj = (pCurrentObj->next);
            continue;
        }
       
        pParticle = ((B3LParticleGenObj_t *)pCurrentObj)->pParticleActive;        
        //project the particle from world space to screen space
        while(i--){
            Vect3Xmat4WithTest(&(pParticle->position), pCamMat, &screenVect);
            //get the screen position
            u32 test = screenVect.test;
            if (B3L_TEST(test,B3L_IN_SPACE)){
                //draw it
                //((B3LParticleGenObj_t *)pCurrentObj)->DrawFunc(pParticle,&screenVect,pFBuff,pZBuff);
                DrawFunc(pParticle,&screenVect,pFBuff,pZBuff);
            } 
            pParticle = pParticle->next;
        }
        pCurrentObj = pCurrentObj->next;    
    }
}
void B3L_DefaultParticleDrawFunc(B3L_Particle_t *pParticle, screen3f_t *pScreenVect,fBuff_t *pFBuff,zBuff_t *pZBuff){

    zBuff_t compZ = CalZbuffValue(pScreenVect->z);
    s32     intX = pScreenVect->x;
    s32     intY = pScreenVect->y;
    u32     shift = RENDER_RESOLUTION_X*intY + intX;
    pZBuff = (pZBuff+shift);
    pFBuff = (pFBuff+shift);
    if (compZ<= *pZBuff){
        *pZBuff = compZ;
        *pFBuff = 0XFFFFFFFF;
    }

}

void     B3L_DefaultParticleUpdFunc(u32 time,B3LParticleGenObj_t *pSelf,mat4_t *pMat,render_t *pRender){
//TODO here
    u32 deltaTime;
    u32 i;
    B3L_Particle_t *pParticle;
    B3L_Particle_t *pPrevParticle;
    vect3_t  delta;
    vect3_t  force ={.x=0.0f,.y=-0.01f,.z=0.0f};
    u32      newParticleNum =  (u32)(time*0.1f);
    if(pSelf->lastTime == 0){//this is the first time a generator is updated, only get the time
        pSelf->lastTime = time;
        return;
    }else{
        //calculate the delta time 
        deltaTime = time - pSelf->lastTime;
        pSelf->lastTime = time;
        //add necessary new particles into the list
        i = Min_u32(pRender->scene.freeParticleNum,newParticleNum);
        u32 randValue;
        f32 inv256 = 0.00390625f;
        while(i--){
            pParticle = B3L_GetFreeParticle(&(pRender->scene));
            //setup position
            pParticle->position.x = pMat->m03;
            pParticle->position.y = pMat->m13;
            pParticle->position.z = pMat->m23;
            //setup lifetime
            pParticle->life = 2000;
            //setup init delta
            randValue = B3L_Random();
            randValue = randValue&0x000000FF;
            delta.x = 0.05f*((f32)randValue -128.0f)*inv256;//need a fast rendom function here!
            randValue = B3L_Random();
            randValue = randValue&0x000000FF;
            delta.y = 0.05f*((f32)randValue)*inv256;
            randValue = B3L_Random();
            randValue = randValue&0x000000FF;
            delta.z = 0.05f*((f32)randValue -128.0f)*inv256;
            B3L_Vect3MulMat4(&delta, pMat, &delta);
            pParticle->delta.x = delta.x;
            pParticle->delta.y = delta.y;
            pParticle->delta.z = delta.z;
            B3L_AddParticleToGenerator(pParticle,pSelf);
        }
        //update all particle statement
        i = pSelf->particleNum ;

        pParticle = pSelf->pParticleActive;
        Vect3_Scale(&force,(f32)deltaTime,&force);//force * time

        while(i--){
            pParticle->life -= deltaTime;
            if (pParticle->life <= 0){
                //dead particle, remove it back to the free particle pool
                if (pSelf->pParticleActive == pParticle){ //the first one
                    pSelf->pParticleActive = pParticle->next;
                }else{
                    pPrevParticle->next = pParticle->next;
                }           
                pSelf->particleNum -=1;   
                B3L_ReturnParticleToPool(pParticle,&(pRender->scene));
                
            }else{
                Vect3_Add(&(pParticle->delta), &force ,&(pParticle->delta));//update the delta
                Vect3_Scale(&(pParticle->delta),(f32)deltaTime,&delta);
                Vect3_Add(&(pParticle->position),&(delta),&(pParticle->position));//update the position

            }
            pPrevParticle = pParticle;
            pParticle = pParticle->next;
        }
    }
}

#endif

static void RenderMeshObjs(render_t *pRender){
    
    mat4_t mat; //64 byte
    vect4_t boundBoxBuffVec; //128 byte
    int32_t i;
    bool inClipSpace;
    u32 state;
    u32 renderLevel;
    f32 distance;
//get the list enter point obj
    B3LObj_t *pCurrentObj = pRender->scene.pActiveMeshObjs;
//printf("start draw obj\n");
    f32 lvl0Distance = pRender->lvl0Distance;
    f32 lvl1Distance = pRender->lvl1Distance;
    while(pCurrentObj != (B3LObj_t *)NULL){
      
        state = pCurrentObj->state;
        if (B3L_TEST(state ,OBJ_VISUALIZABLE)==0){  //obj visual is false

            pCurrentObj = pCurrentObj->next;
            continue;
        }
        //create the obj->clip matrix
        if(B3L_TEST(pCurrentObj->state,OBJ_USING_CUSTOMERIZE_MAT)){
            //copy mat
            CopyMat4(&mat,pCurrentObj->pCustMat);
        }else{    
            B3L_MakeWorldMatrix(&(pCurrentObj->transform), &mat);
        }
        B3L_Mat4Xmat4(&mat, &(pRender->camera.camMat));
        //B3L_logMat4(mat);
        //calculate the bound box position in the clip space
        inClipSpace = false;
        //test 0,0,0 point of obj if it is in the space
        boundBoxBuffVec.x =mat.m03;
        boundBoxBuffVec.y =mat.m13;
        boundBoxBuffVec.z =mat.m23;
        boundBoxBuffVec.w =mat.m33;
        //printf("middle position\n");
        //B3L_logVec4(boundBoxBuffVec);

        if (Vect4BoundTest( &boundBoxBuffVec)){
            inClipSpace = true;    
        }else{
            for (i=7;i>=0;i--){
                Vect3Xmat4((vect3_t *)&(pCurrentObj->pBoundBox[i*3]), &mat, &(boundBoxBuffVec));
                if (Vect4BoundTest( &boundBoxBuffVec)){
                    inClipSpace = true;
                    break;
                }
                
            }
        }
        //test boundBoxTestFactor to check if the obj out of clip range
        if (inClipSpace != true){  //all points outside the clip range
            pCurrentObj = pCurrentObj->next;
            continue;
        }
        //calculate the distance
        f32 x = mat.m03;
        f32 y = mat.m13;
        f32 z = mat.m23;
        //#ifdef B3L_ARM
        distance = B3L_Sqrtf(x*x+y*y+z*z);
        //#else
        //distance = sqrtf(x*x+y*y+z*z);
       // #endif
        //decide render level info
        
        if (B3L_TEST(state ,OBJ_IGNORE_RENDER_LEVEL)){
            renderLevel = (state & OBJ_RENDER_LEVEL_MASK)>>OBJ_FIX_RENDER_LEVEL_SHIFT;
        }else{
            if (distance < lvl0Distance){
                renderLevel = 0;
            }else if (distance < lvl1Distance){
                renderLevel = 1;
            }else{
                renderLevel = 2;
            }            
        }
        

        switch(state & OBJ_TYPE_MASK){
            case (1<<MESH_OBJ):
                RenderTexMesh((B3LMeshObj_t *)pCurrentObj,pRender,&mat,renderLevel);
                break;
            case (1<<POLYGON_OBJ):
                RenderPolygon((B3LPolygonObj_t *)pCurrentObj,pRender,&mat);
                break;
            case (1<<NOTEX_MESH_OBJ):                
                RenderNoTexMesh((B3LMeshNoTexObj_t *)pCurrentObj,pRender,&mat,renderLevel);
                break;
        }      
        //point to the next obj
        pCurrentObj = pCurrentObj->next;
    }

}

void B3L_RenderScence(render_t *pRender){

    //printf("start render\n");
    //set world to clip matrix
    SetCameraMatrix(&(pRender->camera));

    UpdateLightVect(pRender);

    RenderMeshObjs(pRender);
    //draw bitmapObj
    //TODO
    //draw particleObj
#ifdef B3L_USING_PARTICLE
    RenderParticleObjs(pRender);
#endif

}

void B3L_Update(render_t *pRender,u32 time){
    //TODO: Add particle update and other hook here
#ifdef B3L_USING_PARTICLE
    UpdateParticleObjs(pRender, time);

#endif
}

void B3L_NewRenderStart(render_t *pRender){
    ClearFrameBuff(pRender->pFrameBuff,0xFF003423,VIDEO_BUFF_LENTH);
    ClearZbuff(pRender->pZBuff,VIDEO_BUFF_LENTH);
}    

void B3L_ResetScene(scene_t *pScene){
    pScene->freeObjNum = OBJ_BUFF_SIZE; //reset free obj numbers
    pScene->pActiveMeshObjs = (B3LObj_t *)NULL; //reset active obj list
    ResetObjList(pScene);
#ifdef B3L_USING_PARTICLE
    pScene->pActiveParticleGenObjs = (B3LObj_t *)NULL;//reset particle generator obj list 
    pScene->freeParticleNum = B3L_PARTICLE_BUFF_DEPTH;   //reset particle numbers
    ResetParticleList(particleBuff,&(pScene->pfreeParticles),B3L_PARTICLE_BUFF_DEPTH); //reset particle list
    //call reset particle one-way list function
#endif
}

static void ResetObjList(scene_t *pScene){
    u32 i;
    pScene->pFreeObjs = pScene->objBuff;  //reset all the obj buffer
    pScene->objBuff[0].state = 0x00000000;
    pScene->objBuff[0].privous = pScene->pFreeObjs;
    pScene->objBuff[0].next= &(pScene->objBuff[1]);
    for (i = 1;i<OBJ_BUFF_SIZE;i++){
        pScene->objBuff[i].state = 0x00000000;
        pScene->objBuff[i].privous = &(pScene->objBuff[i-1]);
        pScene->objBuff[i].next = &(pScene->objBuff[i+1]);
    }
    pScene->objBuff[OBJ_BUFF_SIZE - 1].next = (B3LObj_t *)NULL;
}

#ifdef B3L_USING_PARTICLE
static void ResetParticleList(B3L_Particle_t *pPool,B3L_Particle_t **pStart,u32 num){
    u32 i;
    *pStart = pPool;
    for (i=0;i<(num-1);i++){
        pPool->state = 0x00000000;
        pPool[i].next = &(pPool[i+1]);
    }
    pPool[(num-1)].next = (B3L_Particle_t *)NULL;

}
void B3L_ReturnParticleToPool(B3L_Particle_t *pParticle,scene_t *pScene){
    
    B3L_Particle_t *temp=pScene->pfreeParticles;
    pParticle->state = 0x00000000;
    pScene->pfreeParticles = pParticle;
    pParticle->next = temp;
    pScene->freeParticleNum +=1;
}

void B3L_AddParticleToGenerator(B3L_Particle_t *pParticle,B3LParticleGenObj_t  *pGenerator){
    B3L_Particle_t *pTemp = pGenerator->pParticleActive;
    pGenerator->pParticleActive = pParticle;
    pParticle->next = pTemp;
    pGenerator->particleNum +=1;
}


B3L_Particle_t *B3L_GetFreeParticle(scene_t *pScene){
    //todo
    if (pScene->freeParticleNum == 0){
        return (B3L_Particle_t *)NULL;
    }
    pScene->freeParticleNum --;
    B3L_Particle_t *popParticle = pScene->pfreeParticles;
    pScene->pfreeParticles = pScene->pfreeParticles->next;
    return popParticle;
}
#endif

static void AddObjToTwoWayList(B3LObj_t *pObj, B3LObj_t **pStart){
    pObj->next = *pStart;
    *pStart = pObj;
    pObj->privous = pObj;
    if (pObj->next != (B3LObj_t *)NULL){
        pObj->next->privous = pObj;
        
    }
    
}
u32      B3L_GetFreeObjNum(render_t *pRender){
    return pRender->scene.freeObjNum;
}

u32      B3L_GetFreeParticleNum(render_t *pRender){
    #ifdef  B3L_USING_PARTICLE 
    return pRender->scene.freeParticleNum;
    #else
    return 0;
    #endif
}

B3LObj_t * B3L_GetFreeObj(render_t *pRender){
    B3LObj_t *returnObj;
    if (pRender->scene.pFreeObjs != (B3LObj_t *)NULL){
        pRender->scene.freeObjNum -=1;
        returnObj = pRender->scene.pFreeObjs;
        pRender->scene.pFreeObjs = pRender->scene.pFreeObjs->next;
        pRender->scene.pFreeObjs->privous = pRender->scene.pFreeObjs;
        //isolate the returned obj 
        returnObj->next = (B3LObj_t *)NULL;
        returnObj->privous = (B3LObj_t *)NULL;
        return returnObj;
    }else{
        return (B3LObj_t *)NULL;
    }
}

B3LMeshObj_t *B3L_GetFreeMeshObj(render_t *pRender){
    B3LObj_t *pObj = B3L_GetFreeObj(pRender);
    if (pObj !=  (B3LObj_t *)NULL){
        B3L_SET(pObj->state,MESH_OBJ);
        B3L_SET(pObj->state,OBJ_VISUALIZABLE);
        pObj->transform.translation.x = 0.0f;
        pObj->transform.translation.y = 0.0f;
        pObj->transform.translation.z = 0.0f;
        pObj->transform.rotation.x = 0.0f;
        pObj->transform.rotation.y = 0.0f;
        pObj->transform.rotation.z = 0.0f;
        pObj->transform.scale.x = 1.0f;
        pObj->transform.scale.y = 1.0f;
        pObj->transform.scale.z = 1.0f;
    }
    return (B3LMeshObj_t *)pObj;
}

B3LMeshNoTexObj_t *B3L_GetFreeMeshNoTexObj(render_t *pRender){
    B3LObj_t *pObj = B3L_GetFreeObj(pRender);
    if (pObj !=  (B3LObj_t *)NULL){
        B3L_SET(pObj->state,NOTEX_MESH_OBJ);
        B3L_SET(pObj->state,OBJ_VISUALIZABLE);
        pObj->transform.translation.x = 0.0f;
        pObj->transform.translation.y = 0.0f;
        pObj->transform.translation.z = 0.0f;
        pObj->transform.rotation.x = 0.0f;
        pObj->transform.rotation.y = 0.0f;
        pObj->transform.rotation.z = 0.0f;
        pObj->transform.scale.x = 1.0f;
        pObj->transform.scale.y = 1.0f;
        pObj->transform.scale.z = 1.0f;
    }
    return (B3LMeshNoTexObj_t *)pObj;
}

B3LPolygonObj_t    *B3L_GetFreePolygonObj(render_t *pRender){
    B3LObj_t *pObj = B3L_GetFreeObj(pRender);
    if (pObj !=  (B3LObj_t *)NULL){
        B3L_SET(pObj->state,POLYGON_OBJ);
        B3L_SET(pObj->state,OBJ_VISUALIZABLE);
        pObj->transform.translation.x = 0.0f;
        pObj->transform.translation.y = 0.0f;
        pObj->transform.translation.z = 0.0f;
        pObj->transform.rotation.x = 0.0f;
        pObj->transform.rotation.y = 0.0f;
        pObj->transform.rotation.z = 0.0f;
        pObj->transform.scale.x = 1.0f;
        pObj->transform.scale.y = 1.0f;
        pObj->transform.scale.z = 1.0f;
    }
    return (B3LPolygonObj_t *)pObj;
}

B3LParticleGenObj_t  *B3L_GetFreeParticleGeneratorObj(render_t *pRender){
    B3LObj_t *pObj = B3L_GetFreeObj(pRender);
    if (pObj !=  (B3LObj_t *)NULL){
        B3L_SET(pObj->state,PARTICLE_GEN_OBJ);
        B3L_SET(pObj->state,OBJ_VISUALIZABLE);
        B3L_SET(pObj->state,OBJ_PARTICLE_ACTIVE);
        ((B3LParticleGenObj_t  *)pObj)->rotation.x = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->rotation.y = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->rotation.z = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->translation.x = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->translation.y = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->translation.z = 0.0f;
        ((B3LParticleGenObj_t  *)pObj)->particleNum = 0;
        ((B3LParticleGenObj_t  *)pObj)->pParticleActive = (B3L_Particle_t *)NULL;
        ((B3LParticleGenObj_t  *)pObj)->lastTime = 0;
    }
    return (B3LParticleGenObj_t  *)pObj;
}

void B3L_AddObjToRenderList(B3LObj_t *pObj, render_t *pRender){
    //get the statement
    u32 type = (pObj->state & OBJ_TYPE_MASK);

    if ((type == (1<<MESH_OBJ))||(type == (1<<POLYGON_OBJ))||(type == (1<<NOTEX_MESH_OBJ))){
        AddObjToTwoWayList(pObj, &(pRender->scene.pActiveMeshObjs));    
    }
    #ifdef B3L_USING_PARTICLE
    if (type == (1<<PARTICLE_GEN_OBJ)){
        AddObjToTwoWayList(pObj, &(pRender->scene.pActiveParticleGenObjs)); 
    }
    #endif
    
}



void B3L_PopObjFromRenderList(B3LObj_t *pObj, render_t *pRender){
    if (pObj->privous != pObj){
        if (pObj->next != (B3LObj_t *)NULL){
            pObj->next->privous = pObj->privous;
        }
        pObj->privous->next = pObj->next; 
    }else{
        u32 type = (pObj->state & OBJ_TYPE_MASK);
        if ((type == (1<<MESH_OBJ))||(type == (1<<POLYGON_OBJ))||(type == (1<<NOTEX_MESH_OBJ))){
            pRender->scene.pActiveMeshObjs = pObj->next;
            if (pObj->next != (B3LObj_t *)NULL){
                pObj->next->privous = pRender->scene.pActiveMeshObjs;
            }
        }
        #ifdef B3L_USING_PARTICLE
        if (type == (1<<PARTICLE_GEN_OBJ)){
            pRender->scene.pActiveParticleGenObjs = pObj->next;
            if (pObj->next != (B3LObj_t *)NULL){
                pObj->next->privous = pRender->scene.pActiveParticleGenObjs;
            }
        }
        #endif
        
    }
    
    pObj->next = (B3LObj_t *)NULL;
    pObj->privous = (B3LObj_t *)NULL;
}

void B3L_ReturnObjToInactiveList(B3LObj_t *pObj,  render_t *pRender){
    
    
    if (pObj->privous != (B3LObj_t *)NULL){ //it is now in the active list
        B3L_PopObjFromRenderList(pObj,  pRender);
    }
    //clean the obj statement
    pObj->state = 0x00000000;
    pRender->scene.freeObjNum +=1;
    AddObjToTwoWayList(pObj, &(pRender->scene.pFreeObjs));
}



void B3L_RenderInit(render_t *pRender,fBuff_t *pFrameBuff){
    pRender->pFrameBuff = pFrameBuff;
    pRender->pZBuff = zBuff;
    pRender->pVectBuff = vectBuff;
    B3L_ResetScene(&(pRender->scene));
    B3L_InitCamera(&(pRender->camera));
    B3L_ResetLight(&(pRender->light));
    pRender->lvl0Distance = LEVEL_0_DEFAULT_DISTANCE;
    pRender->lvl1Distance = LEVEL_1_DEFAULT_DISTANCE;
    pRender->lvl1Light = LEVEL_1_DEFAULT_LIGHT;
}
/*-----------------------------------------------------------------------------
Testing functions
-----------------------------------------------------------------------------*/
void B3L_InitBoxObj(B3LMeshObj_t *pObj,f32 size){
    pObj->privous = (B3LObj_t *)NULL;
    pObj->next = (B3LObj_t *)NULL;
    pObj->pMesh = (B3L_Mesh_t *)&B3L_box;
    pObj->pTexture = (B3L_texture_t *)&B3L_boxTexture;
    pObj->transform.translation.x = 0.0f;
    pObj->transform.translation.y = 0.0f;
    pObj->transform.translation.z = 0.0f;
    pObj->transform.rotation.x = 0.0f;
    pObj->transform.rotation.y = 0.0f;
    pObj->transform.rotation.z = 0.0f;
    pObj->transform.scale.x = size;
    pObj->transform.scale.y = size;
    pObj->transform.scale.z = size;
    pObj->pBoundBox = B3L_box.pVect;
    B3L_SET(pObj->state,MESH_OBJ); 
    B3L_SET(pObj->state,OBJ_VISUALIZABLE);

    B3L_SET(pObj->state,OBJ_BACK_CULLING_CLOCK);
}
void B3L_InitBoxObjNoTexture(B3LMeshNoTexObj_t *pObj,f32 size){
    pObj->privous = (B3LObj_t *)NULL;
    pObj->next = (B3LObj_t *)NULL;
    pObj->pMesh = (B3L_Mesh_NoTex_t *)&B3L_box_noTex;

    pObj->transform.translation.x = 0.0f;
    pObj->transform.translation.y = 0.0f;
    pObj->transform.translation.z = 0.0f;
    pObj->transform.rotation.x = 0.0f;
    pObj->transform.rotation.y = 0.0f;
    pObj->transform.rotation.z = 0.0f;
    pObj->transform.scale.x = size;
    pObj->transform.scale.y = size;
    pObj->transform.scale.z = size;
    pObj->pBoundBox = B3L_box.pVect;
    #if FRAME_BUFF_COLOR_TYPE == 0
    pObj->pLUT =  (texLUT_t *)B3L_boxLUT32bit;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1
    pObj->pLUT =  (texLUT_t *)B3L_boxLUT4444;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
    pObj->pLUT =  (texLUT_t *)B3L_boxLUTL8Idx;
    #endif
    B3L_SET(pObj->state,NOTEX_MESH_OBJ); 
    B3L_SET(pObj->state,OBJ_VISUALIZABLE);

    B3L_SET(pObj->state,OBJ_BACK_CULLING_CLOCK);

}

void B3L_InitBoxObjPolygon(B3LPolygonObj_t *pObj,f32 size){
    pObj->privous = (B3LObj_t *)NULL;
    pObj->next = (B3LObj_t *)NULL;
    pObj->pPolygon = (B3L_Polygon_t *)&B3L_box_Polygon;

    pObj->transform.translation.x = 0.0f;
    pObj->transform.translation.y = 0.0f;
    pObj->transform.translation.z = 0.0f;
    pObj->transform.rotation.x = 0.0f;
    pObj->transform.rotation.y = 0.0f;
    pObj->transform.rotation.z = 0.0f;
    pObj->transform.scale.x = size;
    pObj->transform.scale.y = size;
    pObj->transform.scale.z = size;
    pObj->pBoundBox = B3L_box.pVect;
    #if FRAME_BUFF_COLOR_TYPE == 0
    pObj->color = 0xFF00FF00;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1
    pObj->color = 0xF0F0;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
    pObj->color = 1;
    #endif
    B3L_SET(pObj->state,POLYGON_OBJ); 
    B3L_SET(pObj->state,OBJ_VISUALIZABLE);

    //B3L_SET(pObj->state,OBJ_BACK_CULLING_CLOCK);

}

void     B3L_InitDemoParticleGenObj(B3LParticleGenObj_t  *pParticleGen){
    //todo create a generator based on default functions
    pParticleGen->DrawFunc = B3L_DefaultParticleDrawFunc;
    pParticleGen->PtlUpdFunc = B3L_DefaultParticleUpdFunc;
    pParticleGen->lastTime = 0;
    pParticleGen->particleNum = 0;
    pParticleGen->rotation.x = 0.0f;
    pParticleGen->rotation.y = 0.0f;
    pParticleGen->rotation.z = 0.0f;
    pParticleGen->translation.x = 0.0f;
    pParticleGen->translation.y = 0.0f;
    pParticleGen->translation.z = 0.0f;
    pParticleGen->pParticleActive = (B3L_Particle_t *)NULL;
    B3L_SET(pParticleGen->state,PARTICLE_GEN_OBJ);
    B3L_SET(pParticleGen->state,OBJ_VISUALIZABLE);
    B3L_SET(pParticleGen->state,OBJ_PARTICLE_ACTIVE);


}
/*-----------------------------------------------------------------------------
Draw functions
-----------------------------------------------------------------------------*/
__attribute__((always_inline)) static  inline fBuff_t     GetColorValue(texLUT_t *lut,u8 colorIdx,u32 lightFactor){
    fBuff_t color;
    #if FRAME_BUFF_COLOR_TYPE == 0
    color = lut[colorIdx];
    color = (color&0x00FFFFFF)|((lightFactor)<<24);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1
    color = lut[colorIdx];
    color = (color&0x0FFF)|(((u16)lightFactor)<<12);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
    color = lut[colorIdx];
    color = ((u16)color)|(((u16)lightFactor)<<8);
    #endif
    return color;
}

__attribute__((always_inline)) static  inline void     DrawPixel(fBuff_t color,s32 x,s32 y,f32 z,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff){
        zBuff_t *pCurrentPixelZ = pZbuff + (y*RENDER_RESOLUTION_X) + x;
        fBuff_t *pixel= pFrameBuff + (y*RENDER_RESOLUTION_X) + x; 
        zBuff_t compZ = CalZbuffValue(z);
        if (compZ< *pCurrentPixelZ){          
            *pCurrentPixelZ = compZ;
            *pixel =color;           
        }

}
__attribute__((always_inline)) static  inline void     DrawPixelWithTest(fBuff_t color,s32 x,s32 y,f32 z,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff){
        if ((x<0)||(y<0)||(x>=RENDER_RESOLUTION_X)||(y>=RENDER_RESOLUTION_Y)){
            return;
        }
        zBuff_t *pCurrentPixelZ = pZbuff + (y*RENDER_RESOLUTION_X) + x;
        fBuff_t *pixel= pFrameBuff + (y*RENDER_RESOLUTION_X) + x; 
        zBuff_t compZ = CalZbuffValue(z);
        if (compZ< *pCurrentPixelZ){          
            *pCurrentPixelZ = compZ;
            *pixel =color;           
        }

}
static void ClearFrameBuff(fBuff_t *pFramebuff,fBuff_t value,u32 length){
//in stm32, we could use DMA to do this job   
    int32_t i;   
    #define Addr pFramebuff
    //fBuff_t value = 0;
    for (i=(length>>4) - 1;i>=0;i--){
        
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
    }

    switch(length&0x0000000F){
        case 15:
            *Addr = value;*Addr++;
        case 14:
            *Addr = value;*Addr++;
        case 13:
            *Addr = value;*Addr++;
        case 12:
            *Addr = value;*Addr++;
        case 11:
            *Addr = value;*Addr++;
        case 10:
            *Addr = value;*Addr++;
        case 9:
            *Addr = value;*Addr++;
        case 8:
            *Addr = value;*Addr++;
        case 7:
            *Addr = value;*Addr++;
        case 6:
            *Addr = value;*Addr++;
        case 5:
            *Addr = value;*Addr++;
        case 4:
            *Addr = value;*Addr++;    
        case 3:
            *Addr = value;*Addr++;
        case 2:
            *Addr = value;*Addr++;
        case 1:
            *Addr = value;
        case 0:
            break;
    }
    #undef Addr
}

static void ClearZbuff(zBuff_t *pZbuff,u32 length){
//in stm32, we could use DMA to do this job   
    int32_t i;   
    #define Addr pZbuff
    zBuff_t value = Z_LIMIT_NUM;
    for (i=(length>>4) - 1;i>=0;i--){
        
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
        *Addr = value;*Addr++;
    }

    switch(length&0x0000000F){
        case 15:
            *Addr = value;*Addr++;
        case 14:
            *Addr = value;*Addr++;
        case 13:
            *Addr = value;*Addr++;
        case 12:
            *Addr = value;*Addr++;
        case 11:
            *Addr = value;*Addr++;
        case 10:
            *Addr = value;*Addr++;
        case 9:
            *Addr = value;*Addr++;
        case 8:
            *Addr = value;*Addr++;
        case 7:
            *Addr = value;*Addr++;
        case 6:
            *Addr = value;*Addr++;
        case 5:
            *Addr = value;*Addr++;
        case 4:
            *Addr = value;*Addr++;    
        case 3:
            *Addr = value;*Addr++;
        case 2:
            *Addr = value;*Addr++;
        case 1:
            *Addr = value;
        case 0:
            break;
    }
    #undef Addr
}

static void RenderNoTexMesh(B3LMeshNoTexObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel){

    int32_t i;
    B3L_Mesh_NoTex_t *pMesh= pObj->pMesh;
    vect3_t *pVectSource = ((vect3_t *)(pMesh->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    u8  *pColorIdx = pMesh->pColorIdx;
    texLUT_t       *pLUT = pObj->pLUT;
    fBuff_t *pFrameBuff =pRender->pFrameBuff;
    zBuff_t *pZBuff =  pRender->pZBuff;
//process all the vectors
    for(i=pMesh->vectNum - 1;i>=0;i--){ 
        Vect3Xmat4WithTest_f(pVectSource+i, pMat, pVectTarget+i);
    }
    f32 lightX,lightY,lightZ;
    f32 normalFact;
    f32   lightFactor0;
    f32   lightFactor1;
    if (renderLevel==0){//light calculation is needed, so normalized the normal

        pVectSource = ((vect3_t *)(pMesh->pNormal));// now the vectsource point to the normal vect
        //calculate the Light vect in clip space;      
        if(B3L_TEST(pRender->light.state,LIGHT_TYPE_BIT)){
            lightX = pRender->light.pointLightVectInCamSpaceBuff.x - pMat->m03;
            lightY = pRender->light.pointLightVectInCamSpaceBuff.y - pMat->m13;
            lightZ = pRender->light.pointLightVectInCamSpaceBuff.z - pMat->m23;
            normalFact = FastInvertSqrt(lightX*lightX+lightY*lightY+lightZ*lightZ);
            lightX = lightX * normalFact;
            lightY = lightY * normalFact;
            lightZ = lightZ * normalFact;
        }else{
            lightX = pRender->light.pointLightVectInCamSpaceBuff.x;
            lightY = pRender->light.pointLightVectInCamSpaceBuff.y;
            lightZ = pRender->light.pointLightVectInCamSpaceBuff.z;
        }
        
        lightFactor0 = pRender->light.factor_0;
        lightFactor1 = pRender->light.factor_1;
    }               

    u16 *pTriIdx = pMesh->pTri;


    u32 cullingState = ((pObj->state)&OBJ_CULLING_MASK)>>OBJ_CULLING_SHIFT;

    
    u32 vect0Idx,vect1Idx,vect2Idx;
    vect3_t normalVect;
    f32   normalDotLight;
    fBuff_t color;
    //if the render level is not zero, then the lightValue would fix at 0xff
    u32 lightValue=pRender->lvl1Light;
//draw tri loop
    for (i=pMesh->triNum -1;i>=0;i--){
        //pTriRenderState[i]=0;
        vect0Idx = pTriIdx[i*3];
        vect1Idx = pTriIdx[i*3+1];
        vect2Idx = pTriIdx[i*3+2];
        u32 result0 = pVectTarget[vect0Idx].test;
        u32 result1 = pVectTarget[vect1Idx].test;
        u32 result2 = pVectTarget[vect2Idx].test;
        if(!TriVisable(result0,result1,result2)){
            continue;
        }
        //screen3_t *screenVect = pVectTarget;
        f32 x0 = pVectTarget[vect0Idx].x;
        f32 y0 = pVectTarget[vect0Idx].y;
        f32 x1 = pVectTarget[vect1Idx].x;
        f32 y1 = pVectTarget[vect1Idx].y;
        f32 x2 = pVectTarget[vect2Idx].x;
        f32 y2 = pVectTarget[vect2Idx].y;

        bool backFaceCullingResult = TriangleFaceToViewer_f(x0, y0, x1, y1, x2, y2);
#ifdef B3L_DEBUG
        printf("backFaceCullingResult = %d\n",backFaceCullingResult);
#endif
            
        if ((cullingState==1) && backFaceCullingResult){     
            continue;
        }
        if ((cullingState==2) && (!backFaceCullingResult)){     
            continue;
        }

        if (renderLevel==0){
            Norm3Xmat4Normalize(pVectSource+i, pMat, &normalVect); 
            //dot multi light and normalvect to get the light factor
            normalDotLight = normalVect.x*lightX + normalVect.y*lightY + normalVect.z*lightZ;
            //normalDotLight is in the range -1.0f to 1.0f
            lightValue = CalLightFactor(normalDotLight,lightFactor0,lightFactor1);
        }
        //printf("renderLevel %d,lightValue %d\n",renderLevel,lightValue);
        #if (FRAME_BUFF_COLOR_TYPE  == 0) || (FRAME_BUFF_COLOR_TYPE  == 1)
        color = pLUT[pColorIdx[i]];
        #endif
        #if (FRAME_BUFF_COLOR_TYPE  == 2)
        color = pColorIdx[i];
        #endif
        DrawTriColor(
            x0,y0,pVectTarget[vect0Idx].z,
            x1,y1,pVectTarget[vect1Idx].z,
            x2,y2,pVectTarget[vect2Idx].z,
            renderLevel,lightValue,color,
            pFrameBuff,pZBuff);

    }      
    
}

static void RenderPolygon(B3LPolygonObj_t *pObj,render_t *pRender, mat4_t *pMat){
    int32_t i;
    fBuff_t *pFrameBuff =pRender->pFrameBuff;
    zBuff_t *pZBuff =  pRender->pZBuff;
    B3L_Polygon_t *pPoly = pObj->pPolygon;
    texLUT_t color= pObj->color;
    vect3_t *pVectSource = ((vect3_t *)(pPoly->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    //project all the vect into screen space
    for(i=pPoly->vectNum - 1;i>=0;i--){ 
        Vect3Xmat4WithTest_f(pVectSource+i, pMat, pVectTarget+i);
    }
    u8 *pLine = pPoly->pLine;
    u8 lineIdxA,lineIdxB;
    u32 testA, testB;
    for (i=pPoly->lineNum -1;i>=0;i--){
        
        //draw every line
        lineIdxA = pLine[i*2];
        lineIdxB = pLine[i*2+1];
        testA = pVectTarget[lineIdxA].test;
        testB = pVectTarget[lineIdxB].test;
        if (B3L_TEST(testA,B3L_NEAR_PLANE_CLIP)||
            B3L_TEST(testB,B3L_NEAR_PLANE_CLIP)){
                continue;
        }

        s32 Ax = (s32)(pVectTarget[lineIdxA].x);
        s32 Ay = (s32)(pVectTarget[lineIdxA].y);
        f32 Az = pVectTarget[lineIdxA].z;
        s32 Bx = (s32)(pVectTarget[lineIdxB].x);
        s32 By = (s32)(pVectTarget[lineIdxB].y);
        f32 Bz = pVectTarget[lineIdxB].z;
        if (B3L_TEST(testA,B3L_IN_SPACE )&&
            B3L_TEST(testB,B3L_IN_SPACE )){

            DrawDepthLineNoClip(Ax,Ay,Az,Bx,By,Bz,color,pFrameBuff,pZBuff);
        }else{
            DrawDepthLineClip(Ax,Ay,Az,Bx,By,Bz,color,pFrameBuff,pZBuff);
        }

    }


}


static void RenderTexMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel){
#ifdef B3L_DEBUG
printf("Draw a mesh");
#endif
    int32_t i;
    B3L_Mesh_t *pMesh= pObj->pMesh;
    vect3_t *pVectSource = ((vect3_t *)(pMesh->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    u8 * pUV = pObj->pMesh->pUv;
    B3L_texture_t *pTexture = pObj->pTexture;
    fBuff_t *pFrameBuff =pRender->pFrameBuff;
    zBuff_t *pZBuff =  pRender->pZBuff;
//process all the vectors
    for(i=pMesh->vectNum - 1;i>=0;i--){ 
        Vect3Xmat4WithTest_f(pVectSource+i, pMat, pVectTarget+i);
    }
    f32 lightX,lightY,lightZ;
    f32 normalFact;
    f32   lightFactor0;
    f32   lightFactor1;
    if (renderLevel==0){//light calculation is needed, so normalized the normal

        pVectSource = ((vect3_t *)(pMesh->pNormal));// now the vectsource point to the normal vect
        //calculate the Light vect in clip space;      
        if(B3L_TEST(pRender->light.state,LIGHT_TYPE_BIT)){  
             //dot light, calculate the vect point  to light from obj (both already in camera space)
            lightX = pRender->light.pointLightVectInCamSpaceBuff.x - pMat->m03;
            lightY = pRender->light.pointLightVectInCamSpaceBuff.y - pMat->m13;
            lightZ = pRender->light.pointLightVectInCamSpaceBuff.z - pMat->m23;
            normalFact = FastInvertSqrt(lightX*lightX+lightY*lightY+lightZ*lightZ);
            lightX = lightX * normalFact;
            lightY = lightY * normalFact;
            lightZ = lightZ * normalFact;
        }else{
            //parallel light, the point to light vect is already in camera space
            lightX = pRender->light.pointLightVectInCamSpaceBuff.x;
            lightY = pRender->light.pointLightVectInCamSpaceBuff.y;
            lightZ = pRender->light.pointLightVectInCamSpaceBuff.z;
        }
        
        lightFactor0 = pRender->light.factor_0;
        lightFactor1 = pRender->light.factor_1;
    }               

    u16 *pTriIdx = pMesh->pTri;


    u32 cullingState = ((pObj->state)&OBJ_CULLING_MASK)>>OBJ_CULLING_SHIFT;

    u32 vect0Idx,vect1Idx,vect2Idx;
    vect3_t normalVect;
    f32   normalDotLight;
    //if the render level is not zero, then the lightValue would fix at 0xff
    u32 lightValue=pRender->lvl1Light;
//draw tri loop
    for (i=pMesh->triNum -1;i>=0;i--){
        //pTriRenderState[i]=0;
        vect0Idx = pTriIdx[i*3];
        vect1Idx = pTriIdx[i*3+1];
        vect2Idx = pTriIdx[i*3+2];
        u32 result0 = pVectTarget[vect0Idx].test;
        u32 result1 = pVectTarget[vect1Idx].test;
        u32 result2 = pVectTarget[vect2Idx].test;
        if(!TriVisable(result0,result1,result2)){
            continue;
        }
        //screen3f_t *screenVect = pVectTarget;
        f32 x0 = pVectTarget[vect0Idx].x;
        f32 y0 = pVectTarget[vect0Idx].y;
        f32 x1 = pVectTarget[vect1Idx].x;
        f32 y1 = pVectTarget[vect1Idx].y;
        f32 x2 = pVectTarget[vect2Idx].x;
        f32 y2 = pVectTarget[vect2Idx].y;

        bool backFaceCullingResult = TriangleFaceToViewer_f(x0, y0, x1, y1, x2, y2);
#ifdef B3L_DEBUG
        printf("backFaceCullingResult = %d\n",backFaceCullingResult);
#endif


            
        if ((cullingState==1) && backFaceCullingResult){    
            continue;
        }
        if ((cullingState==2) && (!backFaceCullingResult)){  
            continue;
        }


        if (renderLevel==0){
            Norm3Xmat4Normalize(pVectSource+i, pMat, &normalVect); 
            //dot multi light and normalvect to get the light factor
            normalDotLight = normalVect.x*lightX + normalVect.y*lightY + normalVect.z*lightZ;
            //normalDotLight is in the range -1.0f to 1.0f
            lightValue = CalLightFactor(normalDotLight,lightFactor0,lightFactor1);
        }


        DrawTriTexture(
            x0,y0,(f32)(pUV[i*6]),(f32)(pUV[i*6+1]),pVectTarget[vect0Idx].z,
            x1,y1,(f32)(pUV[i*6+2]),(f32)(pUV[i*6+3]),pVectTarget[vect1Idx].z,
            x2,y2,(f32)(pUV[i*6+4]),(f32)(pUV[i*6+5]),pVectTarget[vect2Idx].z,
            renderLevel,lightValue,pTexture,
            pFrameBuff,pZBuff);

    }      
    
}

__attribute__((always_inline)) static inline void DrawDepthLineNoClip(s32 Ax,s32 Ay,f32 Az,s32 Bx,s32 By,f32 Bz, 
                                                            texLUT_t color,fBuff_t *pFrameBuff,zBuff_t *pZbuff){
//todo here~
    s32 steep = abs(Ay - By) > abs(Ax - Bx);
    fBuff_t drawColor;
    if (steep){
        _swap_int32_t(Ax,Ay);
        _swap_int32_t(Bx,By);
    }
    if (Ax>Bx){
        //swap a,b point
        _swap_int32_t(Ax, Bx);
        _swap_int32_t(Ay, By);
        _swap_f32_t(Az, Bz);
    }
    s32 dx, dy;
    dx = Bx - Ax;
    dy = abs(By - Ay);
    f32 dz = (Bz - Az)/((f32)dx);
    s32 err = dx / 2;
    s32 ystep;
    if (Ay < By) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; Ax <= Bx; Ax++) {
    if (steep) {
        DrawPixel(color,Ay,Ax,Az,pFrameBuff,pZbuff);
    } else {
        DrawPixel(color,Ax,Ay,Az,pFrameBuff,pZbuff);
    }
    Az = Az + dz;
    err -= dy;
    if (err < 0) {
        Ay += ystep;
        err += dx;
    }
  }
    //now A is in the left and B is in the right

}

__attribute__((always_inline)) static inline void DrawDepthLineClip(s32 Ax,s32 Ay,f32 Az,s32 Bx,s32 By,f32 Bz, 
                                                                        texLUT_t color,fBuff_t *pFrameBuff,zBuff_t *pZbuff){
//todo here~

    //now A is in the left and B is in the right
        s32 steep = abs(Ay - By) > abs(Ax - Bx);
    fBuff_t drawColor;
    if (steep){
        _swap_int32_t(Ax,Ay);
        _swap_int32_t(Bx,By);
    }
    if (Ax>Bx){
        //swap a,b point
        _swap_int32_t(Ax, Bx);
        _swap_int32_t(Ay, By);
        _swap_f32_t(Az, Bz);
    }
    s32 dx, dy;
    dx = Bx - Ax;
    dy = abs(By - Ay);
    f32 dz = (Bz - Az)/((f32)dx);
    s32 err = dx / 2;
    s32 ystep;
    if (Ay < By) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; Ax <= Bx; Ax++) {
    if (steep) {
        DrawPixelWithTest(color,Ay,Ax,Az,pFrameBuff,pZbuff);
    } else {
        DrawPixelWithTest(color,Ax,Ay,Az,pFrameBuff,pZbuff);
    }
    Az = Az + dz;
    err -= dy;
    if (err < 0) {
        Ay += ystep;
        err += dx;
    }
  }
}


__attribute__((always_inline)) static inline void DrawColorHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
                                       fBuff_t finalColor, fBuff_t *pFrameBuff,zBuff_t *pZbuff) {
    s32 intx = (s32)x,inty=y,intb=((s32)b);
    s32 clipL = 0;
    s32 clipR = RENDER_RESOLUTION_X ;
    f32 invlength = 1.0f/((f32)(intb-intx));
    intb = intb - 1;
    if ((intx>=clipR)||(b<clipL)){
        return;
    }
    f32 dZ = (bZ - aZ)*invlength;
    //printf("du %.3f, dv %.3f\n",du,dv);
    //s32 sU = du;
    //s32 sV = dv;
    f32 skip;
    u32 intu,intv;
    if (intx< clipL){
        skip = (f32)(clipL -intx);
        intx = clipL;
        aZ += (skip)*dZ;

    }
    if (intb>=clipR){
        intb = clipR-1;
    }
    s32 i = intb-intx;

    u32 shift = inty*RENDER_RESOLUTION_X  + intx;
    fBuff_t *pixel = pFrameBuff +shift;
    zBuff_t  *pCurrentPixelZ = pZbuff + shift;  

    zBuff_t compZ;
    for (;i>=0;i--){ //don't draw the most right pixel, so the b has already -1
        compZ = CalZbuffValue(aZ);
        if (compZ< *pCurrentPixelZ){          
            *pCurrentPixelZ = compZ;
            *pixel =finalColor;           
        }
        aZ = aZ + dZ;
        pCurrentPixelZ ++;
        pixel++;

    }
    
    
}
__attribute__((always_inline)) static inline void DrawTexHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
f32 aU,f32 aV,f32 bU,f32 bV, u32 lightFactor, fBuff_t *pFrameBuff,zBuff_t *pZbuff, B3L_texture_t *pTexture) {
    //printf("auv%.2f,%.2f,buv%.2f,%.2f\n",aU,aV,bU,bV);
    s32 intx = (s32)x,inty=y,intb=((s32)b);
    //s32 b = x + length -1;//correct
    f32 u=aU,v=aV;
    s32 clipL = 0;
    s32 clipR = RENDER_RESOLUTION_X ;
    f32 invlength = 1.0f/((f32)(intb-intx));
    intb = intb - 1;
    //printf("invlength %.3f\n",invlength);
    //length = Max_s32(length , 1) ;

    if ((intx>=clipR)||(b<clipL)){
        return;
    }
    f32 dZ = (bZ - aZ)*invlength;
    f32 du = (bU - aU)*invlength;
    f32 dv = (bV - aV)*invlength; 
    //printf("du %.3f, dv %.3f\n",du,dv);
    //s32 sU = du;
    //s32 sV = dv;
    f32 skip;
    u32 intu,intv;
    if (intx< clipL){
        skip = (f32)(clipL -intx);
        intx = clipL;
        u += (skip)*du;
        v += (skip)*dv; 
        aZ += (skip)*dZ;

    }

    if (intb>=clipR){
        intb = clipR-1;

    }


    s32 i = intb-intx;

    u32 shift = inty*RENDER_RESOLUTION_X  + intx;
    fBuff_t *pixel = pFrameBuff +shift;
    zBuff_t  *pCurrentPixelZ = pZbuff + shift;  
    u32 uvSize = pTexture->uvSize;
    u8  *uvData = pTexture->pData;
    texLUT_t *lut = pTexture->pLUT;


    u8  colorIdx = 0;
    fBuff_t color;
    u8 transColorIdx = pTexture->transColorIdx;
    zBuff_t compZ;
    switch(pTexture->type){
        case LUT16:
        for (;i>=0;i--){  //don't draw the most right pixel, so the b has already -1
            compZ = CalZbuffValue(aZ);
            if (compZ< *pCurrentPixelZ){           
                intu = (uint32_t)u;
                intv = (uint32_t)v;
                colorIdx = uvData[intv*(uvSize>>1) + (intu>>1)];
                if (intu & 0x01){
                    colorIdx = colorIdx>>4;
                }else{
                    colorIdx = colorIdx&0x0F;
                }
                if (colorIdx == transColorIdx){
                    continue;
                }                
                *pCurrentPixelZ = compZ;
                *pixel = GetColorValue(lut,colorIdx,lightFactor);          
            }
            u +=du;
            v +=dv;
            aZ = aZ + dZ;
            pCurrentPixelZ ++;
            pixel++;
        }
        break;
        case LUT256:
        for (;i>=0;i--){
            compZ = CalZbuffValue(aZ);
            if (compZ< *pCurrentPixelZ){          
                intu = (uint32_t)u;
                intv = (uint32_t)v;  
                colorIdx = uvData[intv*uvSize + intu];
                if (colorIdx == transColorIdx){
                    continue;
                }
                *pCurrentPixelZ = compZ;
                *pixel = GetColorValue(lut,colorIdx,lightFactor);           
            }
            u +=du;
            v +=dv;
            aZ +=dZ;
            pCurrentPixelZ++;
            pixel++;
        }
        break;
    }
    
    
}

__attribute__((always_inline)) static  inline void  DrawTriTexture(
                                                                        f32 x0,f32 y0,f32 u0,f32 v0,f32 z0,
                                                                        f32 x1,f32 y1,f32 u1,f32 v1,f32 z1,
                                                                        f32 x2,f32 y2,f32 u2,f32 v2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,B3L_texture_t *pTexture,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff){
//to calculate 0.5 pixel, if it works, then we will modified the project functions
    #ifndef _swap_f32_t
    #define _swap_f32_t(a, b) { f32 t = a; a = b; b = t; }
    #endif
 

    s32 y,last;
    
    if(y0 > y1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(u0,u1);
        _swap_f32_t(v0,v1); 
        _swap_f32_t(z0,z1); 
        //_swap_int32_t(inty0,inty1);  
    }
    if (y1 > y2) {
        //B3L_SWAP_DRAW_TRI_VECT(2,1);
        _swap_f32_t(y2,y1);
        _swap_f32_t(x2,x1);
        _swap_f32_t(u2,u1);
        _swap_f32_t(v2,v1);
        _swap_f32_t(z2,z1);  
        //_swap_int32_t(inty1,inty2);  
    }
    if(y0 > y1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(u0,u1);
        _swap_f32_t(v0,v1);
        _swap_f32_t(z0,z1); 
        //_swap_int32_t(inty0,inty1);   
    }
    s32 inty0 = (s32)y0,inty1 = (s32)y1,inty2 = (s32)y2;
    if(inty0 == inty2) { // Handle awkward all-on-same-line case as its own thing
        return;
    }
    f32 dy01 = 1.0f/(inty1 - inty0);
    f32 dy02 = 1.0f/(inty2 - inty0);
    f32 dy12 = 1.0f/(inty2 - inty1);

    f32 dx01 = (x1 - x0)*dy01;
    f32 dx02 = (x2 - x0)*dy02;
    f32 dx12 = (x2 - x1)*dy12;
    
    f32 du01 = (u1 - u0)*dy01;
    f32 dv01 = (v1 - v0)*dy01;
    f32 du02 = (u2 - u0)*dy02;
    f32 dv02 = (v2 - v0)*dy02;
    f32 du12 = (u2 - u1)*dy12;
    f32 dv12 = (v2 - v1)*dy12;
    f32 dz01 = (z1 - z0)*dy01;
    f32 dz02 = (z2 - z0)*dy02;
    f32 dz12 = (z2 - z1)*dy12;


    f32  aZ=z0;
    f32  bZ=z0;
    f32  a=x0;
    f32  b=x0;
    f32  aU=u0;
    f32  bU=u0;
    f32  aV=v0;
    f32  bV=v0;
    /*
    if(inty1 == inty2) last = inty1;   // Include y1 scanline
    else last = inty1-1; // Skip it
    */
    last = inty1-1;
    if (last>=RENDER_RESOLUTION_Y){
            last = RENDER_RESOLUTION_Y -1;
    }
    //printf("in first half\n");
    for(y=inty0; y<=last; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }
        
        if(!((y<0)||(((s32)a)==((s32)b)))){
        //include a, and b how many pixel
            if(a > b){
                DrawTexHLine(b,y,a,bZ,aZ,bU,bV,aU,aV,lightFactor,pFrameBuff,pZbuff,pTexture);
            }else{
                DrawTexHLine(a,y,b,aZ,bZ,aU,aV,bU,bV,lightFactor,pFrameBuff,pZbuff,pTexture);
            } 
        }
        a   += dx01;
        b   += dx02;

        aU +=du01;
        aV +=dv01;
        bU +=du02;
        bV +=dv02;
        aZ = aZ + dz01;
        bZ = bZ + dz02;
    }

    //s32 deltaY1 = y-inty1;
    s32 deltaY0 = y-inty0;
    a = x1;
    b = x0 + dx02*deltaY0;

    aZ = z1;
    aU = u1;
    aV = v1;
    bU = u0+du02*deltaY0;
    bV = v0+dv02*deltaY0;
    //s32 y2 = tri->y2;
    if (inty2>=RENDER_RESOLUTION_Y){
        inty2= RENDER_RESOLUTION_Y -1;
    }
    
    for(; y<inty2; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }

        if(!((y<0)||(((s32)a)==((s32)b)))){
        //include a, and b how many pixel
            if(a > b){
                DrawTexHLine(b,y,a,bZ,aZ,bU,bV,aU,aV,lightFactor,pFrameBuff,pZbuff,pTexture);
            }else{
                DrawTexHLine(a,y,b,aZ,bZ,aU,aV,bU,bV,lightFactor,pFrameBuff,pZbuff,pTexture);
            } 
        }
        a  += dx12;
        b  += dx02;
        aU += du12;
        aV += dv12;
        bU += du02;
        bV += dv02;
        aZ += dz12;
        bZ += dz02; 

    }



}

__attribute__((always_inline)) static  inline void  DrawTriColor(
                                                                        f32 x0,f32 y0,f32 z0,
                                                                        f32 x1,f32 y1,f32 z1,
                                                                        f32 x2,f32 y2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,fBuff_t color,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff){
//to calculate 0.5 pixel, if it works, then we will modified the project functions
    #ifndef _swap_f32_t
    #define _swap_f32_t(a, b) { f32 t = a; a = b; b = t; }
    #endif
    #ifndef _swap_int32_t
    #define _swap_int32_t(a, b) { int32_t t = a; a = b; b = t; }
    #endif


    #if FRAME_BUFF_COLOR_TYPE == 0
                
            fBuff_t  finalColor = (color&0x00FFFFFF)|(((u32)lightFactor)<<24);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1

            fBuff_t  finalColor  = (color&0x0FFF)|(((u16)lightFactor)<<12);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
            fBuff_t  finalColor  = (color&0x00FF)|(((u16)lightFactor)<<8);
    #endif

    s32 y,last;
    
    if(y0 > y1){
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(z0,z1); 
        //_swap_int32_t(inty0,inty1);  
    }
    if (y1 > y2) {
        _swap_f32_t(y2,y1);
        _swap_f32_t(x2,x1);
        _swap_f32_t(z2,z1);  
        //_swap_int32_t(inty1,inty2);  
    }
    if(y0 > y1){
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(z0,z1); 
        //_swap_int32_t(inty0,inty1);   
    }
    s32 inty0 = (s32)y0,inty1 = (s32)y1,inty2 = (s32)y2;
    if(inty0 == inty2) { // Handle awkward all-on-same-line case as its own thing
        return;
    }
    f32 dy01 = 1.0f/(inty1 - inty0);
    f32 dy02 = 1.0f/(inty2 - inty0);
    f32 dy12 = 1.0f/(inty2 - inty1);

    f32 dx01 = (x1 - x0)*dy01;
    f32 dx02 = (x2 - x0)*dy02;
    f32 dx12 = (x2 - x1)*dy12;

    f32 dz01 = (z1 - z0)*dy01;
    f32 dz02 = (z2 - z0)*dy02;
    f32 dz12 = (z2 - z1)*dy12;

    f32  aZ=z0;
    f32  bZ=z0;
    f32  a=x0;
    f32  b=x0;
    /*
    if(inty1 == inty2) last = inty1;   // Include y1 scanline
    else last = inty1-1; // Skip it
    */
    last = inty1-1;
    if (last>=RENDER_RESOLUTION_Y){
            last = RENDER_RESOLUTION_Y -1;
    }
    //printf("in first half\n");
    for(y=inty0; y<=last; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }
        
        if(!((y<0)||(((s32)a)==((s32)b)))){
        //include a, and b how many pixel
            if(a > b){
                DrawColorHLine(b,y,a,bZ,aZ,finalColor,pFrameBuff,pZbuff);
            }else{
                DrawColorHLine(a,y,b,aZ,bZ,finalColor,pFrameBuff,pZbuff);
            } 
        }
        a   += dx01;
        b   += dx02;
        aZ = aZ + dz01;
        bZ = bZ + dz02;
    }

    //s32 deltaY1 = y-inty1;
    s32 deltaY0 = y-inty0;
    a = x1;
    b = x0 + dx02*deltaY0;
    aZ = z1;

    if (inty2>=RENDER_RESOLUTION_Y){
        inty2= RENDER_RESOLUTION_Y -1;
    }
    //printf("aU,%.3f aV,%.3f bU,%.3f bV%.3f \n",aU,aV,bU,bV);    
    for(; y<inty2; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }
        if(!((y<0)||(((s32)a)==((s32)b)))){
        //include a, and b how many pixel
            if(a > b){
                DrawColorHLine(b,y,a,bZ,aZ,finalColor,pFrameBuff,pZbuff);
            }else{
                DrawColorHLine(a,y,b,aZ,bZ,finalColor,pFrameBuff,pZbuff);
            } 
        }
        a  += dx12;
        b  += dx02;
        aZ += dz12;
        bZ += dz02; 
    }
}

#include <stdio.h>
#include <stdbool.h>
#include "b3dlib.h"

#ifndef   __ASM
  #define __ASM                                  __asm
#endif

//config the ram position if necessary
screen3f_t      vectBuff[VECT_BUFF_SIZE]; //8KB
Z_buff_t       zbuff[BUFF_LENTH];        //75KB


#if Z_BUFF_LEVEL == 2
    #define Z_LIMIT_NUM      (1.0f)
#endif
#if Z_BUFF_LEVEL == 1
    #define Z_LIMIT_NUM      (65535u)
    #define Z_LIMIT_NUM_F    (65535.0f)
#endif
#if Z_BUFF_LEVEL == 0
    #define Z_LIMIT_NUM      (255u)
    #define Z_LIMIT_NUM_F    (255.0f)
#endif


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
const B3L_Mesh_t B3L_box = {.id      = 0,
                      .vectNum = 8,
                      .triNum  = 12,
                      .pVect   = (f32 *)B3L_boxVect,
                      .pTri    = (u16 *)B3L_boxTri,
                      .pUv     = (u8 *)B3L_boxUV,
                      .pNormal = (f32 *)B3L_boxNormal
                      };
const B3L_Mesh_NoTex_t B3L_box_noTex = {.id      = 0,
                      .vectNum = 8,
                      .triNum  = 12,
                      .pVect   = (f32 *)B3L_boxVect,
                      .pTri    = (u16 *)B3L_boxTri,
                      .pColorIdx  = (u8  *)B3L_boxColorIdx,
                      .pNormal = (f32 *)B3L_boxNormal
                      };



const B3L_texture_t B3L_boxTexture = { .id      = 0,
                                 .type    = LUT16,
                                 .uvSize  = 16,
                                 #if FRAME_BUFF_COLOR_TYPE == 0
                                 .pLUT    = (texLUTData_t *)B3L_boxLUT32bit,
                                 #endif
                                 #if FRAME_BUFF_COLOR_TYPE == 1
                                 .pLUT    =  (texLUTData_t *)B3L_boxLUT4444,
                                 #endif
                                 #if FRAME_BUFF_COLOR_TYPE == 2
                                 .pLUT    =  (texLUTData_t *)B3L_boxLUTL8Idx,
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
__attribute__((always_inline)) static  inline s32   B3L_VcvtF32ToS32_Fix(f32 in);
__attribute__((always_inline)) static  inline f32   B3L_VcvtS32ToF32_Fix(s32 in);
__attribute__((always_inline)) static  inline u32   B3L_SatToU8(u32 in);
__attribute__((always_inline)) static  inline u32   B3L_SatToU16(u32 in);

__attribute__((always_inline)) static  inline f32   B3L_nonZero  (f32 value);
__attribute__((always_inline)) static  inline f32   B3L_interp(f32 x1, f32 x2, f32 t);
__attribute__((always_inline)) static  inline s32   B3L_IntClamp(s32 v, s32 v1, s32 v2);
__attribute__((always_inline)) static  inline f32   B3L_FloatClamp(f32 v, f32 v1, f32 v2);
__attribute__((always_inline)) static  inline f32   B3L_min_f(f32 v1, f32 v2);
__attribute__((always_inline)) static  inline s32   B3L_IntMin(s32 v1, s32 v2);
__attribute__((always_inline)) static  inline u32   B3L_UintMin(u32 v1, u32 v2);
__attribute__((always_inline)) static  inline f32   B3L_max_f(f32 v1, f32 v2);
__attribute__((always_inline)) static  inline s32   B3L_IntMax(s32 v1, s32 v2);
__attribute__((always_inline)) static  inline void  B3L_MakeClipMatrix(f32 focalLength, f32 aspectRatio,mat4_t *mat);
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4(vect3_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest_float(vect3_t *pV, mat4_t *pMat, screen3f_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline bool  B3L_Vect4BoundTest(vect4_t *pV);
__attribute__((always_inline)) static  inline f32   B3L_FastInvertSqrt(f32 x);
__attribute__((always_inline)) static  inline void  B3L_UpdateLightVect(render_t *pRender);
__attribute__((always_inline)) static  inline u32   B3L_CalLightFactor(f32 normalDotLight, f32 lightFactor0,f32 lightFactor1);
__attribute__((always_inline)) static  inline bool  B3L_TriangleFaceToViewer(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2);
__attribute__((always_inline)) static  inline bool  B3L_TriangleFaceToViewer2(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);
__attribute__((always_inline)) static  inline bool  B3L_TriVisable(u32 r0,u32 r1,u32 r2);
__attribute__((always_inline)) static  inline void  B3L_DrawTriTexture(
                                                                        f32 x0,f32 y0,f32 u0,f32 v0,f32 z0,
                                                                        f32 x1,f32 y1,f32 u1,f32 v1,f32 z1,
                                                                        f32 x2,f32 y2,f32 u2,f32 v2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,B3L_texture_t *pTexture,
                                                                        frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff);
__attribute__((always_inline)) static   inline void  B3L_DrawTriColor(
                                                                        f32 x0,f32 y0,f32 z0,
                                                                        f32 x1,f32 y1,f32 z1,
                                                                        f32 x2,f32 y2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,frameBuffData_t color,
                                                                        frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff);
__attribute__((always_inline)) static   inline void DrawTexHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
                                                                f32 aU,f32 aV,f32 bU,f32 bV,u32 lightFactor, 
                                                                frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff,
                                                                B3L_texture_t *pTexture);
__attribute__((always_inline)) static   inline void DrawColorHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
                                                                frameBuffData_t finalColor, 
                                                                frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff); 
__attribute__((always_inline)) static   inline void B3L_DrawDepthLine(f32 Ax,f32 Ay,f32 Az,f32 Bx,f32 By,f32 Bz, texLUTData_t color);           

static void B3L_AddObjToList(B3LObj_t *pObj, B3LObj_t **pStart);

static void ClearFrameBuff(frameBuffData_t *pFramebuff,frameBuffData_t value,u32 length);
static void ClearZbuff(Z_buff_t *pZbuff,u32 length);

static void B3L_DrawObjs(render_t *pRender);
static void B3L_DrawMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel);
static void B3L_DrawMeshNoTexture(B3LMeshNoTexObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel);
static void B3L_DrawPolygon(B3LPolygonObj_t *pObj,render_t *pRender, mat4_t *pMat);



/*Private Fuction define ----------------------------------------------------*/
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
__attribute__((always_inline)) static  inline s32   B3L_VcvtF32ToS32_Fix(f32 in){
    s32 result;
    __ASM ("vcvt.s32.f32 %0,%1,#B3L_FIX_BITS" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline f32   B3L_VcvtS32ToF32_Fix(s32 in){
    s32 result;
    __ASM ("vcvt.f32.s32 %0,%1,#B3L_FIX_BITS" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline u32   B3L_SatToU8(u32 in){
    u32 result;
    __ASM ("usat %0,#8,%1" : "=t"(result) : "t"(in));
    return result; 
}

__attribute__((always_inline)) static  inline u32   B3L_SatToU16(u32 in){
    u32 result;
    __ASM ("usat %0,#16,%1" : "=t"(result) : "t"(in));
    return result; 
}

#else
__attribute__((always_inline)) static  inline s32   B3L_VcvtF32ToS32_Fix(f32 in){
    return ((s32)(in*((f32)(1<<B3L_FIX_BITS))));
}
__attribute__((always_inline)) static  inline f32   B3L_VcvtS32ToF32_Fix(s32 in){
    return ((f32)in)/((f32)(1<<B3L_FIX_BITS));
}
__attribute__((always_inline)) static  inline u32   B3L_SatToU8(u32 in){
    const uint32_t max = ((1U << 8) - 1U);
    if (in > max){
        return max;
    }else{
        return in;
    }
}
__attribute__((always_inline)) static  inline u32   B3L_SatToU16(u32 in){
    const uint32_t max = ((1U << 16) - 1U);
    if (in > max){
        return max;
    }else{
        return in;
    }
}

#endif

__attribute__((always_inline)) static  inline f32 B3L_FloatClamp(f32 v, f32 v1, f32 v2){
    return v > v1 ? (v < v2 ? v : v2) : v1;
}

__attribute__((always_inline)) static  inline s32 B3L_IntClamp(s32 v, s32 v1, s32 v2)
{
    return v >= v1 ? (v <= v2 ? v : v2) : v1;
}
__attribute__((always_inline)) static  inline u32 B3L_UintMin(u32 v1, u32 v2){
    return v1 >= v2 ? v2 : v1;
}
__attribute__((always_inline)) static  inline s32 B3L_IntMin(s32 v1, s32 v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static  inline s32 B3L_IntMax(s32 v1, s32 v2){
    return v1 >= v2 ? v1 : v2;
}
__attribute__((always_inline)) static inline f32 B3L_min_f(f32 v1, f32 v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static inline f32 B3L_max_f(f32 v1, f32 v2){
    return v1 >= v2 ? v1 : v2;
}


__attribute__((always_inline)) static inline bool B3L_TriangleFaceToViewer2(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2){
  f32 winding =
    (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) ;
    // ^ cross product for points with z == 0

  return winding >= 0.0f ? true : false;
}


__attribute__((always_inline)) static inline bool B3L_TriangleFaceToViewer(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2){
  int32_t winding =
    (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) ;
    // ^ cross product for points with z == 0

  return winding >= 0 ? true : false;
}
//inv sqrt black magic from quake 
__attribute__((always_inline)) static  inline f32 B3L_FastInvertSqrt(f32 x){
    f32 xhalf = 0.5f * x;
        int i = *(int*)&x;            // store floating-point bits in integer
        i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
        x = *(float*)&i;              // convert new bits into float
        x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
        return x;
}

__attribute__((always_inline)) static inline f32 B3L_nonZero  (f32 value){
    if (value == 0.0f){
        return value + 0.00001f;
    }else{
        return value;
    }
}

__attribute__((always_inline)) static inline f32 B3L_interp(f32 x1, f32 x2, f32 t){
    return x1 + (x2 - x1) * t;
}


__attribute__((always_inline)) static inline void B3L_MakeClipMatrix(f32 focalLength, f32 aspectRatio,mat4_t *mat){

    #define M(x,y) (mat)->m##x##y

    M(0,0) = focalLength; M(1,0) = 0.0f;   M(2,0) = 0.0f;   M(3,0) = 0.0f; 
    M(0,1) = 0.0f;   M(1,1) = focalLength*aspectRatio; M(2,1) = 0.0f;   M(3,1) = 0.0f; 
    M(0,2) = 0.0f;   M(1,2) = 0.0f;   M(2,2) = FAR_PLANE/(FAR_PLANE-NEAR_PLANE); M(3,2) = 1.0f; 
    M(0,3) = 0.0f;   M(1,3) = 0.0f;   M(2,3) =-1.0f*NEAR_PLANE*FAR_PLANE/(FAR_PLANE-NEAR_PLANE);   M(3,3) = 0.0f; 

    #undef M

}
/*
__attribute__((always_inline)) static inline void B3L_ClipToScreen(vect4Test_t *pV, screen4_t *pResult){
    f32 factor=1.0f / (pV->w);
    pResult->x = (int32_t)(HALF_RESOLUTION_X + pV->x *factor* HALF_RESOLUTION_X);
    pResult->y = (int32_t)(HALF_RESOLUTION_Y - pV->y *factor* HALF_RESOLUTION_Y);
    pResult->z = pV->z*factor;
}
*/
__attribute__((always_inline)) static inline void B3L_Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult){
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
__attribute__((always_inline)) static inline void B3L_Vect3Xmat4(vect3_t *pV, mat4_t *pMat, vect4_t *pResult){
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
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest_float(vect3_t *pV, mat4_t *pMat, screen3f_t *pResult){
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
    f32 intX = (HALF_RESOLUTION_X + rx *factor* HALF_RESOLUTION_X);
    f32 intY = (HALF_RESOLUTION_Y - ry *factor* HALF_RESOLUTION_Y);
    rz = rz*factor;

    pResult->x = intX;
    pResult->y = intY;
    pResult->z = rz;
    #undef dotCol

}




__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult){
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

__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult){
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
    factor = B3L_FastInvertSqrt(rx * rx + ry * ry + rz * rz);
    pResult->x = rx*factor;
    pResult->y = ry*factor;
    pResult->z = rz*factor;
    #undef dotCol 

}
__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4(vect3_t *pV, mat4_t *pMat, vect3_t *pResult){
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
__attribute__((always_inline)) static  inline bool B3L_Vect4BoundTest(vect4_t *pV){
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

__attribute__((always_inline)) static  inline bool B3L_TriVisable(u32 r0,u32 r1,u32 r2){
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
/*Fuction define ------------------------------------------------------------*/
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
    in = B3L_FloatClamp(in, -1.0f,1.0f);
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
    f32 factor = B3L_FastInvertSqrt(pV->x*pV->x+pV->y*pV->y+pV->z*pV->z);
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
    pVc->x = B3L_interp(pVa->x,pVb->x,t);
    pVc->y = B3L_interp(pVa->y,pVb->y,t);
    pVc->z = B3L_interp(pVa->z,pVb->z,t);
}

void B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult){
    pResult->x = pA->y * pB->z - pA->z * pB->y;
    pResult->y = pA->z * pB->x - pA->x * pB->z;
    pResult->z = pA->x * pB->y - pA->y * pB->x;  
}

f32 B3L_DotProductVect3(vect3_t *pA, vect3_t *pB){
    return (pA->x*pB->x+pA->y*pB->y+pA->z*pB->z);
}

void B3L_InitMat4(mat4_t *pMat){
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

__attribute__((always_inline)) static inline u32 B3L_CalLightFactor(f32 normalDotLight, f32 lightFactor0,f32 lightFactor1){
    s32 lightValue;
    normalDotLight += lightFactor0;
    lightValue =(s32) (normalDotLight*lightFactor1);
    lightValue = B3L_IntMax(lightValue,0);
    return lightValue;
}

__attribute__((always_inline)) static  inline void  B3L_UpdateLightVect(render_t *pRender){
    //if it is a point light type, then we need to calculate the current light position in camera space 
    if (B3L_TEST((pRender->light.state),LIGHT_TYPE_BIT)){
        B3L_Vect3Xmat4(&(pRender->light.lightVect), &(pRender->camera.camMat), &(pRender->light.pointLightVectInCamSpaceBuff));
    }else{
        B3L_Norm3Xmat4Normalize(&(pRender->light.lightVect), &(pRender->camera.camMat) , (vect3_t *)&(pRender->light.pointLightVectInCamSpaceBuff)); 
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
    B3L_SetCameraMatrix(pCam);
}

void B3L_SetCameraMatrix(camera_t *pCam){

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
    B3L_MakeClipMatrix(pCam->focalLength,pCam->aspectRate,&temp);
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

    dx = v.x / B3L_nonZero(l); // normalize

    pCam->transform.rotation.y = -1.0f * B3L_asin(dx);

    if (v.y < 0){
        pCam->transform.rotation.y = 0.5f - pCam->transform.rotation.y ;
    }
    

    v.x = pAt->y - pCam->transform.translation.y;
    v.y = l;
 
    l = B3L_Vec2Length((vect2_t *)(&v));
 
    dx = v.x / B3L_nonZero(l);

    pCam->transform.rotation.x = B3L_asin(dx);
    //printf("rotation y %.3f,rotation x %.3f\n",pCam->transform.rotation.y,pCam->transform.rotation.x);
}


/*-----------------------------------------------------------------------------
Matrix functions
-----------------------------------------------------------------------------*/
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
       
    B3L_MakeTranslationMat(pWorldTransform->translation.x,
                                pWorldTransform->translation.y,
                                pWorldTransform->translation.z,&temp);
    B3L_Mat4Xmat4(pMat,&temp);   
#ifdef B3L_DEBUG
printf("pWorldTransform %.3f,%.3f,%.3f,\n",pWorldTransform->rotation.x,pWorldTransform->rotation.y,pWorldTransform->rotation.z);
printf("In make world matrix, temp matrix:\n");
B3L_logMat4(temp);  
#endif  
              

}


/*-----------------------------------------------------------------------------
obj functions
-----------------------------------------------------------------------------*/



static void B3L_DrawObjs(render_t *pRender){
    
    mat4_t mat; //64 byte
    vect4_t boundBoxBuffVec; //128 byte
    int32_t i;
    bool inClipSpace;
    u32 state;
    u32 renderLevel;
    f32 distance;
//get the list enter point obj
    B3LObj_t *pCurrentObj = pRender->scene.pActiveObjs;
//printf("start draw obj\n");
    while(pCurrentObj != (B3LObj_t *)NULL){
      
        state = pCurrentObj->state;
        if (B3L_TEST(state ,OBJ_VISUALIZABLE)==0){  //obj visual is false

            pCurrentObj = pCurrentObj->next;
            continue;
        }

        //create the obj->clip matrix
        B3L_MakeWorldMatrix(&(pCurrentObj->transform), &mat);
        
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

        if (B3L_Vect4BoundTest( &boundBoxBuffVec)){
            inClipSpace = true;    
        }else{
            for (i=7;i>=0;i--){
                B3L_Vect3Xmat4((vect3_t *)&(pCurrentObj->pBoundBox[i*3]), &mat, &(boundBoxBuffVec));
                if (B3L_Vect4BoundTest( &boundBoxBuffVec)){
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
            if (distance < LEVEL_0_DISTANCE){
                renderLevel = 0;
            }else if (distance < LEVEL_1_DISTANCE){
                renderLevel = 1;
            }else{
                renderLevel = 2;
            }            
        }
        

        switch(state & OBJ_TYPE_MASK){
            case (1<<MESH_OBJ):
                B3L_DrawMesh((B3LMeshObj_t *)pCurrentObj,pRender,&mat,renderLevel);
                break;
            case (1<<POLYGON_OBJ):
                B3L_DrawPolygon((B3LPolygonObj_t *)pCurrentObj,pRender,&mat);
                break;
            case (1<<NOTEX_MESH_OBJ):                
                B3L_DrawMeshNoTexture((B3LMeshNoTexObj_t *)pCurrentObj,pRender,&mat,renderLevel);
                break;
            case (1<<PARTICLE_OBJ):
                break;
        }      
        //point to the next obj
        pCurrentObj = pCurrentObj->next;
    }

}

void B3L_RenderScence(render_t *pRender){

    //printf("start render\n");
    //set world to clip matrix
    B3L_SetCameraMatrix(&(pRender->camera));

    B3L_UpdateLightVect(pRender);

    B3L_DrawObjs(pRender);


}

void B3L_NewFrame(render_t *pRender){
    ClearFrameBuff(pRender->pFrameBuff,0xFF003423,BUFF_LENTH);
    ClearZbuff(pRender->pZBuff,BUFF_LENTH);
}    

void B3L_ResetScene(scene_t *pScene){
    u32 i;
    pScene->pInactiveObjs = pScene->objBuff;
    pScene->objBuff[0].privous = pScene->pInactiveObjs;
    pScene->objBuff[0].next= &(pScene->objBuff[1]);
    for (i = 1;i<OBJ_BUFF_SIZE;i++){
        pScene->objBuff[i].privous = &(pScene->objBuff[i-1]);
        pScene->objBuff[i].next = &(pScene->objBuff[i+1]);
    }
    pScene->objBuff[OBJ_BUFF_SIZE - 1].next = (B3LObj_t *)NULL;
}

static void B3L_AddObjToList(B3LObj_t *pObj, B3LObj_t **pStart){
    pObj->next = *pStart;
    *pStart = pObj;
    pObj->privous = pObj;
    if (pObj->next != (B3LObj_t *)NULL){
        pObj->next->privous = pObj;
        
    }
    
}

B3LObj_t * B3L_GetFreeObj(render_t *pRender){
    B3LObj_t *returnObj;
    if (pRender->scene.pInactiveObjs != (B3LObj_t *)NULL){
        returnObj = pRender->scene.pInactiveObjs;
        pRender->scene.pInactiveObjs = pRender->scene.pInactiveObjs->next;
        pRender->scene.pInactiveObjs->privous = pRender->scene.pInactiveObjs;
        //isolate the returned obj 
        returnObj->next = (B3LObj_t *)NULL;
        returnObj->privous = (B3LObj_t *)NULL;
        return returnObj;
    }else{
        return (B3LObj_t *)NULL;
    }
}



void B3L_AddObjToRenderList(B3LObj_t *pObj, render_t *pRender){
    B3L_AddObjToList(pObj, &(pRender->scene.pActiveObjs));
}



void B3L_PopObjFromRenderList(B3LObj_t *pObj, render_t *pRender){
    if (pObj->privous != pObj){
        if (pObj->next != (B3LObj_t *)NULL){
            pObj->next->privous = pObj->privous;
        }
        pObj->privous->next = pObj->next; 
    }else{
        pRender->scene.pActiveObjs = pObj->next;
        if (pObj->next != (B3LObj_t *)NULL){
            pObj->next->privous = pRender->scene.pActiveObjs;
        }
    }
    
    pObj->next = (B3LObj_t *)NULL;
    pObj->privous = (B3LObj_t *)NULL;
}

void B3L_ReturnObjToInactiveList(B3LObj_t *pObj,  render_t *pRender){
    if (pObj->privous != (B3LObj_t *)NULL){ //it is now in the active list
        B3L_PopObjFromRenderList(pObj,  pRender);
    }
    B3L_AddObjToList(pObj, &(pRender->scene.pInactiveObjs));
}



void B3L_RenderInit(render_t *pRender,frameBuffData_t *pFrameBuff){
    pRender->pFrameBuff = pFrameBuff;
    pRender->pZBuff = zbuff;
    pRender->pVectBuff = vectBuff;
    B3L_ResetScene(&(pRender->scene));
    B3L_InitCamera(&(pRender->camera));
    B3L_ResetLight(&(pRender->light));

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
    pObj->pLUT =  (texLUTData_t *)B3L_boxLUT32bit;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1
    pObj->pLUT =  (texLUTData_t *)B3L_boxLUT4444;
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
    pObj->pLUT =  (texLUTData_t *)B3L_boxLUTL8Idx;
    #endif
    B3L_SET(pObj->state,NOTEX_MESH_OBJ); 
    B3L_SET(pObj->state,OBJ_VISUALIZABLE);

    B3L_SET(pObj->state,OBJ_BACK_CULLING_CLOCK);

}


static void B3L_DrawMeshNoTexture(B3LMeshNoTexObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel){
#ifdef B3L_DEBUG
printf("Draw a no texture mesh");
#endif
    int32_t i;
    B3L_Mesh_NoTex_t *pMesh= pObj->pMesh;
    vect3_t *pVectSource = ((vect3_t *)(pMesh->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    u8  *pColorIdx = pMesh->pColorIdx;
    texLUTData_t       *pLUT = pObj->pLUT;
//process all the vectors
    for(i=pMesh->vectNum - 1;i>=0;i--){ 
        B3L_Vect3Xmat4WithTest_float(pVectSource+i, pMat, pVectTarget+i);
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
            normalFact = B3L_FastInvertSqrt(lightX*lightX+lightY*lightY+lightZ*lightZ);
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


    u32 cullingState = ((pObj->state)&OBJ_CILLING_MASK)>>OBJ_CILLING_SHIFT;

    
    u32 vect0Idx,vect1Idx,vect2Idx;
    vect3_t normalVect;
    f32   normalDotLight;
    frameBuffData_t color;
    //if the render level is not zero, then the lightValue would fix at 0xff
    u32 lightValue=B3L_LEVEL_1_DEFAULT_LIGHT;
//draw tri loop
    for (i=pMesh->triNum -1;i>=0;i--){
        //pTriRenderState[i]=0;
        vect0Idx = pTriIdx[i*3];
        vect1Idx = pTriIdx[i*3+1];
        vect2Idx = pTriIdx[i*3+2];
        u32 result0 = pVectTarget[vect0Idx].test;
        u32 result1 = pVectTarget[vect1Idx].test;
        u32 result2 = pVectTarget[vect2Idx].test;
        if(!B3L_TriVisable(result0,result1,result2)){
            continue;
        }
        //screen3_t *screenVect = pVectTarget;
        f32 x0 = pVectTarget[vect0Idx].x;
        f32 y0 = pVectTarget[vect0Idx].y;
        f32 x1 = pVectTarget[vect1Idx].x;
        f32 y1 = pVectTarget[vect1Idx].y;
        f32 x2 = pVectTarget[vect2Idx].x;
        f32 y2 = pVectTarget[vect2Idx].y;

        bool backFaceCullingResult = B3L_TriangleFaceToViewer2(x0, y0, x1, y1, x2, y2);
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
            B3L_Norm3Xmat4Normalize(pVectSource+i, pMat, &normalVect); 
            //dot multi light and normalvect to get the light factor
            normalDotLight = normalVect.x*lightX + normalVect.y*lightY + normalVect.z*lightZ;
            //normalDotLight is in the range -1.0f to 1.0f
            lightValue = B3L_CalLightFactor(normalDotLight,lightFactor0,lightFactor1);
        }
        //printf("renderLevel %d,lightValue %d\n",renderLevel,lightValue);
        #if (FRAME_BUFF_COLOR_TYPE  == 0) || (FRAME_BUFF_COLOR_TYPE  == 1)
        color = pLUT[pColorIdx[i]];
        #endif
        #if (FRAME_BUFF_COLOR_TYPE  == 2)
        color = pColorIdx[i];
        #endif
        B3L_DrawTriColor(
            x0,y0,pVectTarget[vect0Idx].z,
            x1,y1,pVectTarget[vect1Idx].z,
            x2,y2,pVectTarget[vect2Idx].z,
            renderLevel,lightValue,color,
            pRender->pFrameBuff,pRender->pZBuff);

    }      
    
}

static void B3L_DrawPolygon(B3LPolygonObj_t *pObj,render_t *pRender, mat4_t *pMat){
    int32_t i;
    B3L_Polygon_t *pPoly = pObj->pPolygon;
    texLUTData_t color= pObj->color;
    vect3_t *pVectSource = ((vect3_t *)(pPoly->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    //project all the vect into screen space
    for(i=pPoly->vectNum - 1;i>=0;i--){ 
        B3L_Vect3Xmat4WithTest_float(pVectSource+i, pMat, pVectTarget+i);
    }
    u8 *pLine = pPoly->pLine;
    u8 lineIdxA,lineIdxB;
    for (i=pPoly->lineNum -1;i>=0;i--){
        //draw every line
        lineIdxA = pLine[i*2];
        lineIdxB = pLine[i*2+1];
        f32 Ax = pVectTarget[lineIdxA].x;
        f32 Ay = pVectTarget[lineIdxA].y;
        f32 Az = pVectTarget[lineIdxA].z;
        f32 Bx = pVectTarget[lineIdxB].x;
        f32 By = pVectTarget[lineIdxB].y;
        f32 Bz = pVectTarget[lineIdxB].z;
        B3L_DrawDepthLine(Ax,Ay,Az,Bx,By,Bz,color);

    }


}


static void B3L_DrawMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat,u32 renderLevel){
#ifdef B3L_DEBUG
printf("Draw a mesh");
#endif
    int32_t i;
    B3L_Mesh_t *pMesh= pObj->pMesh;
    vect3_t *pVectSource = ((vect3_t *)(pMesh->pVect));
    screen3f_t *pVectTarget =pRender->pVectBuff;
    u8 * pUV = pObj->pMesh->pUv;
    B3L_texture_t *pTexture = pObj->pTexture;
//process all the vectors
    for(i=pMesh->vectNum - 1;i>=0;i--){ 
        B3L_Vect3Xmat4WithTest_float(pVectSource+i, pMat, pVectTarget+i);
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
            normalFact = B3L_FastInvertSqrt(lightX*lightX+lightY*lightY+lightZ*lightZ);
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


    u32 cullingState = ((pObj->state)&OBJ_CILLING_MASK)>>OBJ_CILLING_SHIFT;

    u32 vect0Idx,vect1Idx,vect2Idx;
    vect3_t normalVect;
    f32   normalDotLight;
    //if the render level is not zero, then the lightValue would fix at 0xff
    u32 lightValue=B3L_LEVEL_1_DEFAULT_LIGHT;
//draw tri loop
    for (i=pMesh->triNum -1;i>=0;i--){
        //pTriRenderState[i]=0;
        vect0Idx = pTriIdx[i*3];
        vect1Idx = pTriIdx[i*3+1];
        vect2Idx = pTriIdx[i*3+2];
        u32 result0 = pVectTarget[vect0Idx].test;
        u32 result1 = pVectTarget[vect1Idx].test;
        u32 result2 = pVectTarget[vect2Idx].test;
        if(!B3L_TriVisable(result0,result1,result2)){
            continue;
        }
        //screen3f_t *screenVect = pVectTarget;
        f32 x0 = pVectTarget[vect0Idx].x;
        f32 y0 = pVectTarget[vect0Idx].y;
        f32 x1 = pVectTarget[vect1Idx].x;
        f32 y1 = pVectTarget[vect1Idx].y;
        f32 x2 = pVectTarget[vect2Idx].x;
        f32 y2 = pVectTarget[vect2Idx].y;

        bool backFaceCullingResult = B3L_TriangleFaceToViewer2(x0, y0, x1, y1, x2, y2);
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
            B3L_Norm3Xmat4Normalize(pVectSource+i, pMat, &normalVect); 
            //dot multi light and normalvect to get the light factor
            normalDotLight = normalVect.x*lightX + normalVect.y*lightY + normalVect.z*lightZ;
            //normalDotLight is in the range -1.0f to 1.0f
            lightValue = B3L_CalLightFactor(normalDotLight,lightFactor0,lightFactor1);
        }


        B3L_DrawTriTexture(
            x0,y0,(f32)(pUV[i*6]),(f32)(pUV[i*6+1]),pVectTarget[vect0Idx].z,
            x1,y1,(f32)(pUV[i*6+2]),(f32)(pUV[i*6+3]),pVectTarget[vect1Idx].z,
            x2,y2,(f32)(pUV[i*6+4]),(f32)(pUV[i*6+5]),pVectTarget[vect2Idx].z,
            renderLevel,lightValue,pObj->pTexture,
            pRender->pFrameBuff,pRender->pZBuff);

    }      
    
}


static void ClearFrameBuff(frameBuffData_t *pFramebuff,frameBuffData_t value,u32 length){
//in stm32, we could use DMA to do this job   
    int32_t i;   
    #define Addr pFramebuff
    //frameBuffData_t value = 0;
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

static void ClearZbuff(Z_buff_t *pZbuff,u32 length){
//in stm32, we could use DMA to do this job   
    int32_t i;   
    #define Addr pZbuff
    Z_buff_t value = Z_LIMIT_NUM;
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


/*draw functions ------------------------------------------------------------*/
__attribute__((always_inline)) static inline void B3L_DrawDepthLine(f32 Ax,f32 Ay,f32 Az,f32 Bx,f32 By,f32 Bz, texLUTData_t color){
//todo here~
}



__attribute__((always_inline)) static inline void DrawColorHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
frameBuffData_t finalColor, frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff) {
    //printf("auv%.2f,%.2f,buv%.2f,%.2f\n",aU,aV,bU,bV);
    s32 intx = (s32)x,inty=y,intb=(s32)b;
    //s32 b = x + length -1;//correct
    s32 clipL = 0;
    s32 clipR = RENDER_RESOLUTION_X ;
    f32 invlength = 1.0f/((f32)(intb-intx));
    //printf("invlength %.3f\n",invlength);
    //length = B3L_IntMax(length , 1) ;

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
    frameBuffData_t *pixel = pFrameBuff +shift;
    Z_buff_t  *pCurrentPixelZ = pZbuff + shift;  


    for (;i>0;i--){ //don't draw the most right pixel
        #if  (Z_BUFF_LEVEL == 0)
        if (aZ< ((*pCurrentPixelZ)*0.0039215686f)){
        #endif
        #if (Z_BUFF_LEVEL == 1)
        if (aZ< ((*pCurrentPixelZ)*0.000015259021896f)){
        #endif
        #if (Z_BUFF_LEVEL == 2)
        if (aZ< *pCurrentPixelZ){
        #endif
            #if Z_BUFF_LEVEL == 0
                u32 compZ = (u32)(aZ*255.0f);
                compZ = B3L_SatToU8(compZ);
                //compZ = B3L_UintMin(compZ, 255);    //could be optimzed by cortex m dsp q ins
            #endif
            #if Z_BUFF_LEVEL == 1
                u32 compZ = (u32)(aZ*65535.0f);
                compZ = B3L_SatToU16(compZ);
                //compZ = B3L_UintMin(compZ, 65535);
            #endif
            #if Z_BUFF_LEVEL == 2
                f32 compZ = aZ;
            #endif
            //printf("u:%.3f,v:%.3f\n",u,v);
            //if(intu>15){intu=15;}
            //if(intv>15){intv=15;}
            
            *pCurrentPixelZ = compZ;
            *pixel =finalColor;
            
        }

            aZ = aZ + dZ;
            pCurrentPixelZ ++;
            pixel++;

    }
    
    
}
__attribute__((always_inline)) static inline void DrawTexHLine(f32 x,s32 y,f32 b, f32 aZ, f32 bZ,
f32 aU,f32 aV,f32 bU,f32 bV, u32 lightFactor, frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff, B3L_texture_t *pTexture) {
    //printf("auv%.2f,%.2f,buv%.2f,%.2f\n",aU,aV,bU,bV);
    s32 intx = (s32)x,inty=y,intb=(s32)b;
    //s32 b = x + length -1;//correct
    f32 u=aU,v=aV;
    s32 clipL = 0;
    s32 clipR = RENDER_RESOLUTION_X ;
    f32 invlength = 1.0f/((f32)(intb-intx));
    //printf("invlength %.3f\n",invlength);
    //length = B3L_IntMax(length , 1) ;

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
    frameBuffData_t *pixel = pFrameBuff +shift;
    Z_buff_t  *pCurrentPixelZ = pZbuff + shift;  
    u32 uvSize = pTexture->uvSize;
    u8  *uvData = pTexture->pData;
    texLUTData_t *lut = pTexture->pLUT;


    u8  colorIdx = 0;
    frameBuffData_t color;
    u8 transColorIdx = pTexture->transColorIdx;
    switch(pTexture->type){
        case LUT16:
        for (;i>0;i--){ //don't draw the most right pixel
            #if  (Z_BUFF_LEVEL == 0)
            if (aZ< ((*pCurrentPixelZ)*0.0039215686f)){
            #endif
            #if (Z_BUFF_LEVEL == 1)
            if (aZ< ((*pCurrentPixelZ)*0.000015259021896f)){
            #endif
            #if (Z_BUFF_LEVEL == 2)
            if (aZ< *pCurrentPixelZ){
            #endif
                #if Z_BUFF_LEVEL == 0
                    u32 compZ = (u32)(aZ*255.0f);
                    compZ = B3L_SatToU8(compZ);
                    //compZ = B3L_UintMin(compZ, 255);    //could be optimzed by cortex m dsp q ins
                #endif
                #if Z_BUFF_LEVEL == 1
                    u32 compZ = (u32)(aZ*65535.0f);
                    compZ = B3L_SatToU16(compZ);
                    //compZ = B3L_UintMin(compZ, 65535);
                #endif
                #if Z_BUFF_LEVEL == 2
                    f32 compZ = aZ;
                #endif
                intu = (uint32_t)u;
                intv = (uint32_t)v;
                //printf("u:%.3f,v:%.3f\n",u,v);
                //if(intu>15){intu=15;}
                //if(intv>15){intv=15;}
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
                *pixel = color;          
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
            #if  (Z_BUFF_LEVEL == 0)
            if (aZ< ((*pCurrentPixelZ)*0.0039215686f)){
            #endif
            #if (Z_BUFF_LEVEL == 1)
            if (aZ< ((*pCurrentPixelZ)*0.000015259021896f)){
            #endif
            #if (Z_BUFF_LEVEL == 2)
            if (aZ< *pCurrentPixelZ){
            #endif
                #if Z_BUFF_LEVEL == 0
                    u32 compZ = (u32)(aZ*255.0f);
                    compZ = B3L_SatToU8(compZ);
                    //compZ = B3L_UintMin(compZ, 255);    //could be optimzed by cortex m dsp q ins
                #endif
                #if Z_BUFF_LEVEL == 1
                    u32 compZ = (u32)(aZ*65535.0f);
                    compZ = B3L_SatToU16(compZ);
                    //compZ = B3L_UintMin(compZ, 65535);
                #endif
                #if Z_BUFF_LEVEL == 2
                    f32 compZ = aZ;
                #endif       
                intu = (uint32_t)u;
                intv = (uint32_t)v;  
                //if(intu>15){intu=15;}
                //if(intv>15){intv=15;} 
                colorIdx = uvData[intv*uvSize + intu];
                if (colorIdx == transColorIdx){
                    continue;
                }
                *pCurrentPixelZ = compZ;
                
                #if FRAME_BUFF_COLOR_TYPE == 0
                color = lut[colorIdx];
                color = (color&0x00FFFFFF)|((lightFactor)<<24);
                #endif
                #if FRAME_BUFF_COLOR_TYPE == 1
                color = lut[colorIdx];
                color = (color&0x0FFF)|(((u16)lightFactor)<<12);
                #endif
                #if FRAME_BUFF_COLOR_TYPE == 2
                color = ((u16)colorIdx)|(((u16)lightFactor)<<8);
                #endif
                *pixel = color;        
            }
            u +=du;
            v +=dv;
            aZ = aZ + dZ;
            pCurrentPixelZ ++;
            pixel++;
        }
        break;
    }
    
    
}


__attribute__((always_inline)) static  inline void  B3L_DrawTriTexture(
                                                                        f32 x0,f32 y0,f32 u0,f32 v0,f32 z0,
                                                                        f32 x1,f32 y1,f32 u1,f32 v1,f32 z1,
                                                                        f32 x2,f32 y2,f32 u2,f32 v2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,B3L_texture_t *pTexture,
                                                                        frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff){
//to calculate 0.5 pixel, if it works, then we will modified the project functions
    #ifndef _swap_f32_t
    #define _swap_f32_t(a, b) { f32 t = a; a = b; b = t; }
    #endif
    #ifndef _swap_int32_t
    #define _swap_int32_t(a, b) { int32_t t = a; a = b; b = t; }
    #endif
    #ifndef _swap_zbuff_t
    #define _swap_zbuff_t(a, b) { Z_buff_t t = a; a = b; b = t; }
    #endif

    #define B3L_SWAP_DRAW_TRI_VECT(a,b) _swap_int32_t(y##a,y##b);\
                                        _swap_int32_t(x##a,x##b);\
                                        _swap_int32_t(u##a,u##b);\
                                        _swap_int32_t(v##a,v##b);\
                                        _swap_zbuff_t(z##a,z##b);  



    s32 y,last;
    s32 inty0 = (s32)y0,inty1 = (s32)y1,inty2 = (s32)y2;
    if(inty0 > inty1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(u0,u1);
        _swap_f32_t(v0,v1); 
        _swap_zbuff_t(z0,z1); 
        _swap_int32_t(inty0,inty1);  
    }
    if (inty1 > inty2) {
        //B3L_SWAP_DRAW_TRI_VECT(2,1);
        _swap_f32_t(y2,y1);
        _swap_f32_t(x2,x1);
        _swap_f32_t(u2,u1);
        _swap_f32_t(v2,v1);
        _swap_zbuff_t(z2,z1);  
        _swap_int32_t(inty1,inty2);  
    }
    if(inty0 > inty1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_f32_t(u0,u1);
        _swap_f32_t(v0,v1);
        _swap_zbuff_t(z0,z1); 
        _swap_int32_t(inty0,inty1);   
    }
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
    //printf("\n",dy01,dy02,dy12);
    //printf("du12,%.3f dv12,%.3f du02,%.3f dv02,%.3f \n",du12,dv12,du02,dv02);
    if(inty1 == inty2) last = inty1;   // Include y1 scanline
    else last = inty1-1; // Skip it
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
    //printf("aU,%.3f aV,%.3f bU,%.3f bV%.3f \n",aU,aV,bU,bV);
    
    for(; y<=inty2; y++) {
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

__attribute__((always_inline)) static  inline void  B3L_DrawTriColor(
                                                                        f32 x0,f32 y0,f32 z0,
                                                                        f32 x1,f32 y1,f32 z1,
                                                                        f32 x2,f32 y2,f32 z2,
                                                                        u32 renderLevel,u32 lightFactor,frameBuffData_t color,
                                                                        frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff){
//to calculate 0.5 pixel, if it works, then we will modified the project functions
    #ifndef _swap_f32_t
    #define _swap_f32_t(a, b) { f32 t = a; a = b; b = t; }
    #endif
    #ifndef _swap_int32_t
    #define _swap_int32_t(a, b) { int32_t t = a; a = b; b = t; }
    #endif
    #ifndef _swap_zbuff_t
    #define _swap_zbuff_t(a, b) { Z_buff_t t = a; a = b; b = t; }
    #endif

    #if FRAME_BUFF_COLOR_TYPE == 0
                
            frameBuffData_t  finalColor = (color&0x00FFFFFF)|(((u32)lightFactor)<<24);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 1

            frameBuffData_t  finalColor  = (color&0x0FFF)|(((u16)lightFactor)<<12);
    #endif
    #if FRAME_BUFF_COLOR_TYPE == 2
            frameBuffData_t  finalColor  = (color&0x00FF)|(((u16)lightFactor)<<8);
    #endif

    s32 y,last;
    s32 inty0 = (s32)y0,inty1 = (s32)y1,inty2 = (s32)y2;
    if(inty0 > inty1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_zbuff_t(z0,z1); 
        _swap_int32_t(inty0,inty1);  
    }
    if (inty1 > inty2) {
        //B3L_SWAP_DRAW_TRI_VECT(2,1);
        _swap_f32_t(y2,y1);
        _swap_f32_t(x2,x1);
        _swap_zbuff_t(z2,z1);  
        _swap_int32_t(inty1,inty2);  
    }
    if(inty0 > inty1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_f32_t(y0,y1);
        _swap_f32_t(x0,x1);
        _swap_zbuff_t(z0,z1); 
        _swap_int32_t(inty0,inty1);   
    }
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
    //printf("\n",dy01,dy02,dy12);
    //printf("du12,%.3f dv12,%.3f du02,%.3f dv02,%.3f \n",du12,dv12,du02,dv02);
    if(inty1 == inty2) last = inty1;   // Include y1 scanline
    else last = inty1-1; // Skip it
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
    for(; y<=inty2; y++) {
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

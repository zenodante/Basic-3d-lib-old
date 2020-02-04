#include <stdio.h>
#include <stdbool.h>
#include "b3dlib.h"
//__attribute__((section(".fb")))

#if Z_BUFF_LEVEL == 2
    #define Z_LIMIT_NUM      (1.0f)
#endif
#if Z_BUFF_LEVEL == 1
    #define Z_LIMIT_NUM      (65535u)
    #define Z_LIMIT_NUM_F      (65535.0f)
#endif
#if Z_BUFF_LEVEL == 0
    #define Z_LIMIT_NUM      (255u)
    #define Z_LIMIT_NUM_F      (255.0f)
#endif

#define B3L_logVec3(v)\
  printf("Vec3: %.3f %.3f %.3f\n",((v).x),((v).y),((v).z))
#define B3L_logVec4(v)\
  printf("Vec4: %.3f %.3f %.3f %.3f\n",((v).x),((v).y),((v).z),((v).w))
#define B3L_logMat4(m)\
  printf("Mat4:\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n"\
   ,(m).m00,(m).m10,(m).m20,(m).m30,\
    (m).m01,(m).m11,(m).m21,(m).m31,\
    (m).m02,(m).m12,(m).m22,(m).m32,\
    (m).m03,(m).m13,(m).m23,(m).m33)

#define B3L_SET(PIN,N)  (PIN |=  (1u<<N))
#define B3L_CLR(PIN,N)  (PIN &= ~(1u<<N))
#define B3L_TEST(PIN,N) (PIN & (1u<<N))


screen3_t    vectBuff[VECT_BUFF_SIZE]; //10KB
Z_buff_t       zbuff[BUFF_LENTH];//~75KB

const f32_t   B3L_boxVect[24] = {
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

const uint16_t B3L_boxTri[36] ={
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
#define m 15
const uint8_t B3L_boxUV[72]={
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
const f32_t B3L_boxNormal[36]={
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
const uint32_t B3L_boxLUT[16]={
    0X000000FF,
    0X1D2B53FF,
    0X7E2553FF,
    0X008751FF,
    0XAB5236FF,
    0X5F574FFF,
    0XC2C3C7FF,
    0XFFF1E8FF,
    0XFF004DFF,
    0XFFA300FF,
    0XFFEC27FF,
    0X00E436FF,
    0X29ADFFFF,
    0X83769CFF,
    0XFF77A8FF,
    0XFFCCAAFF
};

const uint8_t B3L_boxTexData[128]={
	0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X44, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X49, 
	0X14, 0X73, 0X77, 0XFF, 0XFF, 0X22, 0XD2, 0X49, 0X14, 0XA3, 0XAA, 0XAA, 0XAA, 0XAA, 0XDA, 0X49, 
	0X14, 0XB3, 0XBB, 0XBB, 0XBB, 0XBB, 0XDB, 0X49, 0X14, 0XC3, 0XCC, 0XCC, 0XCC, 0XCC, 0XDC, 0X49, 
	0X14, 0X83, 0X88, 0X88, 0X88, 0X88, 0XD8, 0X49, 0X14, 0XE3, 0XEE, 0XEE, 0XEE, 0XEE, 0XDE, 0X49, 
	0X14, 0XED, 0XEE, 0XEE, 0XEE, 0XEE, 0X5E, 0X49, 0X14, 0X8D, 0X88, 0X88, 0X88, 0X88, 0X58, 0X49, 
	0X14, 0XCD, 0XCC, 0XCC, 0XCC, 0XCC, 0X5C, 0X49, 0X14, 0XBD, 0XBB, 0XBB, 0XBB, 0XBB, 0X5B, 0X49, 
	0X14, 0XAD, 0XAA, 0XAA, 0XAA, 0XAA, 0X5A, 0X49, 0X14, 0X2D, 0X22, 0XFF, 0XFF, 0X77, 0X57, 0X49, 
	0X14, 0X11, 0X11, 0X11, 0X11, 0X11, 0X11, 0X44, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 0X66, 
};
B3L_Mesh_t B3L_box = {.id      = 0,
                      .vectNum = 8,
                      .triNum  = 12,
                      .pVect   = (f32_t *)B3L_boxVect,
                      .pTri    = (uint16_t *)B3L_boxTri,
                      .pUv     = (uint8_t *)B3L_boxUV,
                      .pNormal = (f32_t *)B3L_boxNormal
                      };

B3L_texture_t B3L_boxTexture = { .id      = 0,
                                 .type    = LUT16,
                                 .uvSize  = 16,
                                 .pLUT    = (frameBuffData_t *)B3L_boxLUT,
                                 .pData   = (uint8_t *)B3L_boxTexData

};
/*Private Fuction declare ---------------------------------------------------*/ 
__attribute__((always_inline)) static  inline f32_t B3L_nonZero  (f32_t value);
__attribute__((always_inline)) static  inline f32_t B3L_interp(f32_t x1, f32_t x2, f32_t t);
__attribute__((always_inline)) static  inline s32 B3L_IntClamp(s32 v, s32 v1, s32 v2);
__attribute__((always_inline)) static  inline f32_t B3L_FloatClamp(f32_t v, f32_t v1, f32_t v2);
__attribute__((always_inline)) static  inline f32_t B3L_min(f32_t v1, f32_t v2);
__attribute__((always_inline)) static  inline s32 B3L_IntMin(s32 v1, s32 v2);
__attribute__((always_inline)) static  inline f32_t B3L_max(f32_t v1, f32_t v2);
__attribute__((always_inline)) static  inline s32 B3L_IntMax(s32 v1, s32 v2);
__attribute__((always_inline)) static inline void B3L_MakeClipMatrix(f32_t focalLength, f32_t aspectRatio,mat4_t *mat);
//__attribute__((always_inline)) static  inline void  B3L_ClipToScreen(vect4Test_t *pV, screen4_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4(vect3_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult);
__attribute__((always_inline)) static  inline void  B3L_Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult);
__attribute__((always_inline)) static  inline bool B3L_Vect4BoundTest(vect4_t *pV);
__attribute__((always_inline)) static  inline f32_t B3L_FastInvertSqrt(f32_t x);
__attribute__((always_inline)) static inline bool B3L_TriangleWinding(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2);
__attribute__((always_inline)) static  inline bool B3L_TriVisable(uint32_t r0,uint32_t r1,uint32_t r2);
__attribute__((always_inline)) static inline void B3L_DrawTriTexture(
s32 x0,s32 y0,s32 u0,s32 v0,f32_t z0,
s32 x1,s32 y1,s32 u1,s32 v1,f32_t z1,
s32 x2,s32 y2,s32 u2,s32 v2,f32_t z2,
uint32_t renderLevel,uint8_t lightFactor,B3L_texture_t *pTexture,
frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff);
__attribute__((always_inline)) static inline void DrawTexHLine(s32 x,s32 y,s32 length, f32_t aZ, f32_t bZ,
s32 aU,s32 aV,s32 bU,s32 bV,uint8_t lightFactor, frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff, B3L_texture_t *pTexture);
static void B3L_AddObjToList(B3LObj_t *pObj, B3LObj_t **pStart);
static void B3L_DrawMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat, uint32_t renderLevel);
static void ClearFrameBuff(frameBuffData_t *pFramebuff,frameBuffData_t value,uint32_t length);
static void ClearZbuff(Z_buff_t *pZbuff,uint32_t length);

/*Private Fuction define ----------------------------------------------------*/

__attribute__((always_inline)) static  inline f32_t B3L_FloatClamp(f32_t v, f32_t v1, f32_t v2){
    return v > v1 ? (v < v2 ? v : v2) : v1;
}

__attribute__((always_inline)) static  inline s32 B3L_IntClamp(s32 v, s32 v1, s32 v2)
{
    return v >= v1 ? (v <= v2 ? v : v2) : v1;
}

__attribute__((always_inline)) static  inline s32 B3L_IntMin(s32 v1, s32 v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static  inline s32 B3L_IntMax(s32 v1, s32 v2){
    return v1 >= v2 ? v1 : v2;
}

__attribute__((always_inline)) static inline bool B3L_TriangleWinding(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2){
  int32_t winding =
    (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0) ;
    // ^ cross product for points with z == 0

  return winding >= 0 ? true : false;
}
//inv sqrt black magic from quake 
__attribute__((always_inline)) static  inline f32_t B3L_FastInvertSqrt(f32_t x){
    f32_t xhalf = 0.5f * x;
        int i = *(int*)&x;            // store floating-point bits in integer
        i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
        x = *(float*)&i;              // convert new bits into float
        x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
        return x;
}

__attribute__((always_inline)) static inline f32_t B3L_nonZero  (f32_t value){
    if (value == 0.0f){
        return value + 0.00001f;
    }else{
        return value;
    }
}

__attribute__((always_inline)) static inline f32_t B3L_interp(f32_t x1, f32_t x2, f32_t t){
    return x1 + (x2 - x1) * t;
}
__attribute__((always_inline)) static inline f32_t B3L_min(f32_t v1, f32_t v2){
    return v1 >= v2 ? v2 : v1;
}

__attribute__((always_inline)) static inline f32_t B3L_max(f32_t v1, f32_t v2){
    return v1 >= v2 ? v1 : v2;
}

__attribute__((always_inline)) static inline void B3L_MakeClipMatrix(f32_t focalLength, f32_t aspectRatio,mat4_t *mat){

    #define M(x,y) (mat)->m##x##y

    M(0,0) = focalLength; M(1,0) = 0.0f;   M(2,0) = 0.0f;   M(3,0) = 0.0f; 
    M(0,1) = 0.0f;   M(1,1) = focalLength*aspectRatio; M(2,1) = 0.0f;   M(3,1) = 0.0f; 
    M(0,2) = 0.0f;   M(1,2) = 0.0f;   M(2,2) = FAR_PLANE/(FAR_PLANE-NEAR_PLANE); M(3,2) = 1.0f; 
    M(0,3) = 0.0f;   M(1,3) = 0.0f;   M(2,3) =-1.0f*NEAR_PLANE*FAR_PLANE/(FAR_PLANE-NEAR_PLANE);   M(3,3) = 0.0f; 

    #undef M

}
/*
__attribute__((always_inline)) static inline void B3L_ClipToScreen(vect4Test_t *pV, screen4_t *pResult){
    f32_t factor=1.0f / (pV->w);
    pResult->x = (int32_t)(HALF_RESOLUTION_X + pV->x *factor* HALF_RESOLUTION_X);
    pResult->y = (int32_t)(HALF_RESOLUTION_Y - pV->y *factor* HALF_RESOLUTION_Y);
    pResult->z = pV->z*factor;
}
*/
__attribute__((always_inline)) static inline void B3L_Vect4Xmat4(vect4_t *pV, mat4_t *pMat, vect4_t *pResult){
    f32_t x,y,z,w;
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
    f32_t x,y,z;
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

__attribute__((always_inline)) static  inline void  B3L_Vect3Xmat4WithTest(vect3_t *pV, mat4_t *pMat, screen3_t *pResult){
    f32_t x,y,z,rx,ry,rz,rw;
    x = pV->x;
    y = pV->y;
    z = pV->z;

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
        B3L_SET(pResult->test,B3L_NEAR_PLANE_CLIP);      
        return;
    }else{
        if((rx<=rw)&&(rx>=-rw)&&(ry<=rw)&&(ry>=-rw)&&(rz<=rw)){
            B3L_SET(pResult->test,B3L_IN_SPACE);
        }
    }

    f32_t factor=1.0f / (rw);//rw won't be zero due we returned already rz<0 (rz>0, rw must >0)
    s32 intX = (int32_t)(HALF_RESOLUTION_X + rx *factor* HALF_RESOLUTION_X);
    s32 intY = (int32_t)(HALF_RESOLUTION_Y - ry *factor* HALF_RESOLUTION_Y);
    rz = rz*factor;

    pResult->x = intX;
    pResult->y = intY;
    pResult->z = rz;
    #undef dotCol

}

__attribute__((always_inline)) static  inline void  B3L_Norm3Xmat4Normalize(vect3_t *pV, mat4_t *pMat, vect3_t *pResult){
    f32_t x,y,z,rx,ry,rz,factor;
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
    f32_t x,y,z,rx,ry,rz;
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
    f32_t x,y,z,w;

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

__attribute__((always_inline)) static  inline bool B3L_TriVisable(uint32_t r0,uint32_t r1,uint32_t r2){
    bool returnVal=true;
    //uint32_t nearPlaneCount = 0;
    //uint32_t inSpaceCount = 0;

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
//list function

//math functions
const f32_t sinTable_f32[B3L_MATH_TABLE_SIZE + 1] = {
   0.00000000f, 0.01227154f, 0.02454123f, 0.03680722f, 0.04906767f, 0.06132074f,
   0.07356456f, 0.08579731f, 0.09801714f, 0.11022221f, 0.12241068f, 0.13458071f,
   0.14673047f, 0.15885814f, 0.17096189f, 0.18303989f, 0.19509032f, 0.20711138f,
   0.21910124f, 0.23105811f, 0.24298018f, 0.25486566f, 0.26671276f, 0.27851969f,
   0.29028468f, 0.30200595f, 0.31368174f, 0.32531029f, 0.33688985f, 0.34841868f,
   0.35989504f, 0.37131719f, 0.38268343f, 0.39399204f, 0.40524131f, 0.41642956f,
   0.42755509f, 0.43861624f, 0.44961133f, 0.46053871f, 0.47139674f, 0.48218377f,
   0.49289819f, 0.50353838f, 0.51410274f, 0.52458968f, 0.53499762f, 0.54532499f,
   0.55557023f, 0.56573181f, 0.57580819f, 0.58579786f, 0.59569930f, 0.60551104f,
   0.61523159f, 0.62485949f, 0.63439328f, 0.64383154f, 0.65317284f, 0.66241578f,
   0.67155895f, 0.68060100f, 0.68954054f, 0.69837625f, 0.70710678f, 0.71573083f,
   0.72424708f, 0.73265427f, 0.74095113f, 0.74913639f, 0.75720885f, 0.76516727f,
   0.77301045f, 0.78073723f, 0.78834643f, 0.79583690f, 0.80320753f, 0.81045720f,
   0.81758481f, 0.82458930f, 0.83146961f, 0.83822471f, 0.84485357f, 0.85135519f,
   0.85772861f, 0.86397286f, 0.87008699f, 0.87607009f, 0.88192126f, 0.88763962f,
   0.89322430f, 0.89867447f, 0.90398929f, 0.90916798f, 0.91420976f, 0.91911385f,
   0.92387953f, 0.92850608f, 0.93299280f, 0.93733901f, 0.94154407f, 0.94560733f,
   0.94952818f, 0.95330604f, 0.95694034f, 0.96043052f, 0.96377607f, 0.96697647f,
   0.97003125f, 0.97293995f, 0.97570213f, 0.97831737f, 0.98078528f, 0.98310549f,
   0.98527764f, 0.98730142f, 0.98917651f, 0.99090264f, 0.99247953f, 0.99390697f,
   0.99518473f, 0.99631261f, 0.99729046f, 0.99811811f, 0.99879546f, 0.99932238f,
   0.99969882f, 0.99992470f, 1.00000000f, 
};
f32_t B3L_sin(f32_t in){
    f32_t sinVal, fract;                   /* Temporary input, output variables */
    uint16_t index;                                /* Index variable */
    f32_t a, b;                                /* Two nearest output values */
    int32_t n;
    f32_t findex;
    f32_t positive = 1.0f;

    /* Calculation of floor value of input */
    n = (int32_t) in;

    /* Make negative values towards -infinity */
    if (in < 0.0f)
    {
        n--;
    }

    /* Map input value to [0 0.25] */
    in = in - (f32_t) n;

    if (in < 0.25f){
        //do nothing
    }else if(in < 0.5f){
        //90degree to 180 degree
        in = 0.5f - in;

    }else if(in <0.75f){
        in = in - 0.5f;
        positive = -1.0f;
        
    }else{
        in = 1.0f - in;
        positive = -1.0f;

    }



    /* Calculation of index of the table */
    findex = ((f32_t)(B3L_MATH_TABLE_SIZE<<2)) * in;
    index = (uint16_t)findex;

    /* when "in" is exactly 1, we need to rotate the index down to 0 */
    /*
    if (index >= B3L_MATH_TABLE_SIZE) {
        index = 0;
        findex -= (B3L_t)B3L_MATH_TABLE_SIZE;
    }
    */
    /* fractional value calculation */
    fract = findex - (f32_t) index;

    /* Read two nearest values of input value from the sin table */

        a = sinTable_f32[index];

        b = sinTable_f32[index+1];  


    /* Linear interpolation process */
    sinVal = ((1.0f - fract) * a + fract * b)*positive;
    //sinVal = a*positive;
    /* Return output value */
    return (sinVal);

 }
f32_t B3L_cos(f32_t in){
    return B3L_sin(in + 0.25f);
}

f32_t B3L_asin(f32_t in){
    f32_t sign = 1.0f;
    f32_t result;
    
    in = B3L_FloatClamp(in, -1.0f,1.0f);

    if (in < 0.0f){
        sign = -1.0f;
        in = -1.0f*in;
    }

    uint32_t low = 0;
    uint32_t high = B3L_MATH_TABLE_SIZE;
    uint32_t middle;
    f32_t v;
    while(low<high){
        middle =(low+high)/2;       
        v = sinTable_f32[middle];
        if (v>in){
            high = middle-1;
        }else{
            if(v<in){
                low = middle+1;
            }else{
                low = middle;
                break;
            }    
        } 
        
    }
    
    result = ((f32_t)low)/((f32_t)(B3L_MATH_TABLE_SIZE*4));
    printf("low is %d, high is %d\n",low,high);
    return result*sign;

}

void B3L_NormalizeVec3(vect3_t *pV){
    f32_t factor = B3L_FastInvertSqrt(pV->x*pV->x+pV->y*pV->y+pV->z*pV->z);
    pV->x = (pV->x*factor);
    pV->y = (pV->y*factor);
    pV->z = (pV->z*factor);
}

f32_t B3L_Vec2Length(vect2_t *pV){
    return sqrtf(pV->x*pV->x+pV->y*pV->y);
}

f32_t B3L_Vec3Length(vect3_t *pV){
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);  
}

void B3L_Vec3Add(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc){
    pVc->x = pVa->x + pVb->x;
    pVc->y = pVa->y + pVb->y;
    pVc->z = pVa->z + pVb->z;
}

void B3L_VecInterp(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc,f32_t t){
    pVc->x = B3L_interp(pVa->x,pVb->x,t);
    pVc->y = B3L_interp(pVa->y,pVb->y,t);
    pVc->z = B3L_interp(pVa->z,pVb->z,t);
}

void B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult){
    pResult->x = pA->y * pB->z - pA->z * pB->y;
    pResult->y = pA->z * pB->x - pA->x * pB->z;
    pResult->z = pA->x * pB->y - pA->y * pB->x;  
}

f32_t B3L_DotProductVect3(vect3_t *pA, vect3_t *pB){
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
    f32_t temp;
    temp = ((f32_t *)pMat)[1];
    ((f32_t *)pMat)[1]=((f32_t *)pMat)[4];
    ((f32_t *)pMat)[4] = temp;

    temp = ((f32_t *)pMat)[2];
    ((f32_t *)pMat)[2]=((f32_t *)pMat)[8];
    ((f32_t *)pMat)[8] = temp;

    temp = ((f32_t *)pMat)[6];
    ((f32_t *)pMat)[6]=((f32_t *)pMat)[9];
    ((f32_t *)pMat)[9] = temp;
    
    temp = ((f32_t *)pMat)[3];
    ((f32_t *)pMat)[3]=((f32_t *)pMat)[12];
    ((f32_t *)pMat)[12] = temp;

    temp = ((f32_t *)pMat)[7];
    ((f32_t *)pMat)[7]=((f32_t *)pMat)[13];
    ((f32_t *)pMat)[13] = temp;

    temp = ((f32_t *)pMat)[11];
    ((f32_t *)pMat)[11]=((f32_t *)pMat)[14];
    ((f32_t *)pMat)[14] = temp;
}




//mat1 * mat2 -> mat1
void B3L_Mat4Xmat4(mat4_t *pMat1, mat4_t *pMat2){
    f32_t t0,t1,t2,t3;
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
    B3L_MakeRotationMatrixZXY(pCam->transform.rotation.x,
                              pCam->transform.rotation.y,
                              pCam->transform.rotation.z,&(pCam->camMat));
    B3L_TransposeMat4(&(pCam->camMat)); 
    pCam->camMat.m03 = -1.0f * pCam->transform.translation.x;
    pCam->camMat.m13 = -1.0f * pCam->transform.translation.y;
    pCam->camMat.m23 = -1.0f * pCam->transform.translation.z;       
    pCam->camMat.m33 = 1.0f;
    mat4_t temp;
    B3L_MakeClipMatrix(pCam->focalLength,pCam->aspectRate,&temp);
    B3L_Mat4Xmat4(&(pCam->camMat),&temp);                
}

void B3L_CameraLookAt(camera_t *pCam, vect3_t *pAt){
    vect3_t v;

    v.x = pAt->x - pCam->transform.translation.x;
    v.y = pAt->z - pCam->transform.translation.z;

    f32_t dx = v.x;
    f32_t l = B3L_Vec2Length((vect2_t *)(&v));

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
}


/*-----------------------------------------------------------------------------
Matrix functions
-----------------------------------------------------------------------------*/
void B3L_MakeRotationMatrixZXY(f32_t byX,f32_t byY,f32_t byZ,mat4_t *pMat){
    byX *= -1.0f;
    byY *= -1.0f;
    byZ *= -1.0f;

    f32_t sx = B3L_sin(byX);
    f32_t sy = B3L_sin(byY);
    f32_t sz = B3L_sin(byZ);

    f32_t cx = B3L_cos(byX);
    f32_t cy = B3L_cos(byY);
    f32_t cz = B3L_cos(byZ);
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

void B3L_MakeScaleMatrix(f32_t scaleX,f32_t scaleY,f32_t scaleZ,mat4_t *pMat){
    #define M(x,y) (pMat)->m##x##y

    M(0,0) = scaleX; M(1,0) = 0.0f;   M(2,0) = 0.0f;   M(3,0) = 0.0f; 
    M(0,1) = 0.0f;   M(1,1) = scaleY; M(2,1) = 0.0f;   M(3,1) = 0.0f; 
    M(0,2) = 0.0f;   M(1,2) = 0.0f;   M(2,2) = scaleZ; M(3,2) = 0.0f; 
    M(0,3) = 0.0f;   M(1,3) = 0.0f;   M(2,3) = 0.0f;   M(3,3) = 1.0f; 

    #undef M
}

void B3L_MakeTranslationMat(f32_t offsetX,f32_t offsetY,f32_t offsetZ,mat4_t *pMat){
    #define M(x,y) (pMat)->m##x##y
    f32_t one = 1.0f;
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



void B3L_DrawObjs(render_t *pRender){
    
    mat4_t mat; //64 byte
    vect4_t boundBoxBuffVec; //128 byte
    int32_t i;
    bool inClipSpace;
    uint32_t state;
    uint32_t renderLevel;
    f32_t distance;
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
        f32_t x = mat.m03;
        f32_t y = mat.m13;
        f32_t z = mat.m23;
        distance = sqrtf(x*x+y*y+z*z);
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
            case (1<<TEXTURE2D_OBJ):
                break;
            case (1<<POLYGON_OBJ):
                break;
            case (1<<NOTEX_MESH_OBJ):
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

    //draw background
    

    B3L_DrawObjs(pRender);


}

void B3L_NewFrame(render_t *pRender){
    ClearFrameBuff(pRender->pFrameBuff,0x000000FF,BUFF_LENTH);
    ClearZbuff(pRender->pZBuff,BUFF_LENTH);
}    

void B3L_ResetScene(scene_t *pScene){
    uint32_t i;
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

void B3L_ResetLight(light_t *pLight){
    pLight->strongth = 0.5f;
    pLight->pointToLight.x = 0.0f;
    pLight->pointToLight.y = 1.0f;
    pLight->pointToLight.z = 0.0f;
    #if B3L_ALPHA_CHANNEL_SHIFT == 0
    pLight->color = 0x000000FF;
    #endif
    #if B3L_ALPHA_CHANNEL_SHIFT == 3
    pLight->color = 0xFF000000;
    #endif
    pLight->factor_0 = 1.01f; //make sure it is larger than 0, now the range is 0.01~2.01
    pLight->factor_1 =126.7f;
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
void B3L_InitBoxObj(B3LMeshObj_t *pObj,f32_t size){
    uint32_t i;
    pObj->privous = (B3LObj_t *)NULL;
    pObj->next = (B3LObj_t *)NULL;
    pObj->pMesh = &B3L_box;
    pObj->pTexture = &B3L_boxTexture;
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
    B3L_SET(pObj->state,OBJ_BACK_CULLING);
}

static void B3L_DrawMesh(B3LMeshObj_t *pObj,render_t *pRender, mat4_t *pMat,uint32_t renderLevel){
#ifdef B3L_DEBUG
printf("Draw a mesh");
#endif
    int32_t i;
    B3L_Mesh_t *pMesh= pObj->pMesh;
    vect3_t *pVectSource = ((vect3_t *)(pMesh->pVect));
    screen3_t *pVectTarget =pRender->pVectBuff;
    uint8_t * pUV = pObj->pMesh->pUv;
    B3L_texture_t *pTexture = pObj->pTexture;
//process all the vectors
    for(i=pMesh->vectNum - 1;i>=0;i--){ 
        B3L_Vect3Xmat4WithTest(pVectSource+i, pMat, pVectTarget+i);
    }
    vect3_t light;
    f32_t   lightFactor0;
    f32_t   lightFactor1;
    if (renderLevel==0){//light calculation is needed, so normalized the normal

        pVectSource = ((vect3_t *)(pMesh->pNormal));// now the vectsource point to the normal vect
        //calculate the toLight vect in clip space;       
        B3L_Norm3Xmat4Normalize(&(pRender->light.pointToLight), &(pRender->camera.camMat) , &light); 

        lightFactor0 = pRender->light.factor_0;
        lightFactor1 = pRender->light.factor_1;
    }               

    uint16_t *pTriIdx = pMesh->pTri;
    uint32_t cullingState = B3L_TEST(pObj->state,OBJ_BACK_CULLING);
    uint32_t vect0Idx,vect1Idx,vect2Idx;
    vect3_t normalVect;
    f32_t   normalDotLight;
    uint8_t lightValue=0xFF;
//draw tri loop
    for (i=pMesh->triNum -1;i>=0;i--){
        //pTriRenderState[i]=0;
        vect0Idx = pTriIdx[i*3];
        vect1Idx = pTriIdx[i*3+1];
        vect2Idx = pTriIdx[i*3+2];
        uint32_t result0 = pVectTarget[vect0Idx].test;
        uint32_t result1 = pVectTarget[vect1Idx].test;
        uint32_t result2 = pVectTarget[vect2Idx].test;
        if(!B3L_TriVisable(result0,result1,result2)){
            continue;
        }
        //screen3_t *screenVect = pVectTarget;
        s32 x0 = pVectTarget[vect0Idx].x;
        s32 y0 = pVectTarget[vect0Idx].y;
        s32 x1 = pVectTarget[vect1Idx].x;
        s32 y1 = pVectTarget[vect1Idx].y;
        s32 x2 = pVectTarget[vect2Idx].x;
        s32 y2 = pVectTarget[vect2Idx].y;

        bool backFaceCullingResult = B3L_TriangleWinding(x0, y0, x1, y1, x2, y2);
#ifdef B3L_DEBUG
        printf("backFaceCullingResult = %d\n",backFaceCullingResult);
#endif
        if (cullingState && backFaceCullingResult){     
            continue;
        }

        if (renderLevel==0){
            B3L_Norm3Xmat4Normalize(pVectSource+i, pMat, &normalVect); 
            //dot multi light and normalvect to get the light factor
            normalDotLight = normalVect.x*light.x + normalVect.y*light.y + normalVect.z*light.z;
            //normalDotLight is in the range -1.0f to 1.0f
            normalDotLight += lightFactor0;
            lightValue = (uint8_t)(normalDotLight*lightFactor1);
        }



        //draw it
        B3L_DrawTriTexture(
            x0,y0,pUV[i*6],pUV[i*6+1],pVectTarget[vect0Idx].z,
            x1,y1,pUV[i*6+2],pUV[i*6+3],pVectTarget[vect1Idx].z,
            x2,y2,pUV[i*6+4],pUV[i*6+5],pVectTarget[vect2Idx].z,
            renderLevel,lightValue,pObj->pTexture,
            pRender->pFrameBuff,pRender->pZBuff);

    }      
    
}

static void ClearFrameBuff(frameBuffData_t *pFramebuff,frameBuffData_t value,uint32_t length){
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

    switch(length-(length>>4)){
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

static void ClearZbuff(Z_buff_t *pZbuff,uint32_t length){
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

    switch(length-(length>>4)){
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


__attribute__((always_inline)) static inline void DrawTexHLine(s32 x,s32 y,s32 length, f32_t aZ, f32_t bZ,
s32 aU,s32 aV,s32 bU,s32 bV, uint8_t lightFactor, frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff, B3L_texture_t *pTexture) {

    s32 b = x + length -1;
    s32 u,v;
    s32 clipL = 0;
    s32 clipR = RENDER_RESOLUTION_X ;
    length = B3L_IntMax(length -1, 1) ;

    if ((x>=clipR)||(b<clipL)){
        return;
    }
    /*
    if ((aZ>1.0f) && (bZ>1.0f)){
        return;
    }
    */
    f32_t dZ = (bZ -aZ)/(f32_t)length;
    s32 du = bU - aU;
    s32 dv = bV - aV; 
    s32 sU = 0;
    s32 sV = 0;
    s32 skip;
    
    if (x< clipL){
        skip = clipL -x;
        x = clipL;
        sU = skip*du;
        sV = skip*dv; 
        aZ = aZ+ ((f32_t)skip)*dZ;

    }
    if (b>=clipR){
        b = clipR-1;

    }
    s32 i = b-x;
    uint32_t shift = y*RENDER_RESOLUTION_X  + x;
    frameBuffData_t *pixel = pFrameBuff +shift;
    Z_buff_t  *pCurrentPixelZ = pZbuff + shift;
    
    uint32_t uvSize = pTexture->uvSize;
    uint8_t  *uvData = pTexture->pData;
    frameBuffData_t *lut = pTexture->pLUT;
    uint8_t  colorIdx = 0;
    uint32_t color;
    switch(pTexture->type){
        case LUT16:
        for (;i>0;i--){ //don't draw the most right pixel
            u= aU+sU/length ;
            v= aV+sV/length ;
            sU += du ;
            sV += dv ;
            #if Z_BUFF_LEVEL == 0
                int32_t compZ = (int32_t)(aZ*255.0f);
                compZ = B3L_IntClamp(compZ, 0, 255);    //could be optimzed by cortex m dsp q ins
            #endif
            #if Z_BUFF_LEVEL == 1
                int32_t compZ = (int32_t)(aZ*65535.0f);
                compZ = B3L_IntClamp(compZ, 0, 65535);
            #endif
            #if Z_BUFF_LEVEL == 2
                f32_t compZ = aZ;
            #endif
            if (aZ< *pCurrentPixelZ){
                *pCurrentPixelZ = aZ;
                colorIdx = uvData[v*(uvSize>>1) + (u>>1)];
                if (u & 0x01){
                    color = lut[colorIdx>>4];
                }else{
                    color = lut[colorIdx&0x0F];
                }
                #if B3L_ALPHA_CHANNEL_SHIFT == 0
                color = (color&0xFFFFFF00)|((uint32_t)lightFactor);
                #endif
                #if B3L_ALPHA_CHANNEL_SHIFT == 3
                color = (color&0x00FFFFFF)|(((uint32_t)lightFactor)<<24);
                #endif
                *pixel = color;          
            }
            aZ = aZ + dZ;
            pCurrentPixelZ ++;
            pixel++;
        }
        break;
        case LUT256:
        for (;i>=0;i--){
            u= aU+sU/length ;
            v= aV+sV/length ;
            sU += du ;
            sV += dv ;
            #if Z_BUFF_LEVEL == 0
                int32_t compZ = (int32_t)(aZ*255.0f);
                compZ = B3L_IntClamp(compZ, 0, 255);    //could be optimzed by cortex m dsp q ins
            #endif
            #if Z_BUFF_LEVEL == 1
                int32_t compZ = (int32_t)(aZ*65535.0f);
                compZ = B3L_IntClamp(compZ, 0, 65535);
            #endif
            #if Z_BUFF_LEVEL == 2
                f32_t compZ = aZ;
            #endif
            if (aZ< *pCurrentPixelZ){
                *pCurrentPixelZ = aZ;
                colorIdx = uvData[v*uvSize + u];
                color = lut[colorIdx];
                #if B3L_ALPHA_CHANNEL_SHIFT == 0
                color = (color&0xFFFFFF00)|((uint32_t)lightFactor);
                #endif    
                #if B3L_ALPHA_CHANNEL_SHIFT == 3
                color = (color&0x00FFFFFF)|(((uint32_t)lightFactor)<<24);
                #endif
                *pixel = color;        
            }
            aZ = aZ + dZ;
            pCurrentPixelZ ++;
            pixel++;
        }
        break;
    }
    
    
}

__attribute__((always_inline)) static inline void B3L_DrawTriTexture(
s32 x0,s32 y0,s32 u0,s32 v0,f32_t z0,
s32 x1,s32 y1,s32 u1,s32 v1,f32_t z1,
s32 x2,s32 y2,s32 u2,s32 v2,f32_t z2,
uint32_t renderLevel,uint8_t lightFactor,B3L_texture_t *pTexture,
frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff){
    
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
    s32 a,b,y,last;
    if(y0 > y1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_int32_t(y0,y1);
        _swap_int32_t(x0,x1);
        _swap_int32_t(u0,u1);
        _swap_int32_t(v0,v1); 
        _swap_zbuff_t(z0,z1);   
    }
    if (y1 > y2) {
        //B3L_SWAP_DRAW_TRI_VECT(2,1);
        _swap_int32_t(y2,y1);
        _swap_int32_t(x2,x1);
        _swap_int32_t(u2,u1);
        _swap_int32_t(v2,v1);
        _swap_zbuff_t(z2,z1);  
    }
    if(y0 > y1){
        //B3L_SWAP_DRAW_TRI_VECT(0,1);
        _swap_int32_t(y0,y1);
        _swap_int32_t(x0,x1);
        _swap_int32_t(u0,u1);
        _swap_int32_t(v0,v1);
        _swap_zbuff_t(z0,z1);  
    }
        if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        /*
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        DrawHLine(a, y0, b-a+1, color);
        */
        return;
    }

    s32
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;

    s32 
    du01 = u1 - u0,
    dv01 = v1 - v0,
    du02 = u2 - u0,
    dv02 = v2 - v0,
    du12 = u2 - u1,
    dv12 = v2 - v1;

    s32
    sa = 0,
    sb = 0,
    saU = 0,
    saV = 0,
    sbU = 0,
    sbV = 0;

    s32
    aU,aV,bU,bV;

    f32_t 
    dz01 = (z1 - z0)/(f32_t)(dy01),
    dz02 = (z2 - z0)/(f32_t)(dy02),
    dz12 = (z2 - z1)/(f32_t)(dy12);
    f32_t aZ=z0,bZ=z0;



    if(y1 == y2) last = y1;   // Include y1 scanline
    else last = y1-1; // Skip it
    if (last>=RENDER_RESOLUTION_Y){
            last = RENDER_RESOLUTION_Y -1;
    }

    for(y=y0; y<=last; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;


        aU = u0 +saU/dy01;
        aV = v0 +saV/dy01;
        bU = u0 +sbU/dy02;
        bV = v0 +sbV/dy02;

        sa += dx01;
        sb += dx02;

        saU +=du01;
        saV +=dv01;
        sbU +=du02;
        sbV +=dv02;

        if(y<0){
            continue;
        }
        //printf("aZ:%.4f,bZ:%.4f\n",aZ,bZ);
        if(a > b){
            DrawTexHLine(b,y,a-b+1,bZ,aZ,bU,bV,aU,aV,lightFactor,pFrameBuff,pZbuff,pTexture);
        }else{
            DrawTexHLine(a,y,b-a+1,aZ,bZ,aU,aV,bU,bV,lightFactor,pFrameBuff,pZbuff,pTexture);
        } 

        aZ = aZ + dz01;
        bZ = bZ + dz02;


    }
    s32 deltaY1 = y - y1;
    s32 deltaY0 = y - y0;
    sa = dx12 * deltaY1;
    sb = dx02 * deltaY0;
    saU = du12 * deltaY1;
    saV = dv12 * deltaY1;
    sbU = du02 * deltaY0;
    sbV = dv02 * deltaY0;


    aZ = z1;

    //s32 y2 = tri->y2;
    if (y2>=RENDER_RESOLUTION_Y){
        y2= RENDER_RESOLUTION_Y -1;
    }
    for(; y<=y2; y++) {
        if ((aZ>1.0f) && (bZ>1.0f)){
            continue;
        }
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;

        aU= u1 + saU/dy12;
        aV= v1 + saV/dy12;
        bU= u0 + sbU/dy02;
        bV= v0 + sbV/dy02;
        sa += dx12;
        sb += dx02;
        saU +=du12;
        saV +=dv12;
        sbU +=du02;
        sbV +=dv02;
        if(y<0){
            continue;
        }

        if(a > b){
            DrawTexHLine(b,y,a-b+1,bZ,aZ,bU,bV,aU,aV,lightFactor,pFrameBuff,pZbuff,pTexture);
        }else{
            DrawTexHLine(a,y,b-a+1,aZ,bZ,aU,aV,bU,bV,lightFactor,pFrameBuff,pZbuff,pTexture);
        }

        aZ += dz12;
        bZ += dz02; 

    }

}



#ifndef __B3DLIB_H__
#define __B3DLIB_H__


#include <stdint.h>
#include <math.h>

//#define  B3L_ARM



//#define B3L_DEBUG
/*Config area----------------------------------------------------------------*/
//not used fix math 
#define B3L_FIX_BITS            10
//vect buff is limited the max vectors in single obj
#define VECT_BUFF_SIZE          512
#define OBJ_BUFF_SIZE           64
//Zbuffer level 2: f32, 1:u16, 0: u8
#define Z_BUFF_LEVEL            2
/*
Type 0: 32bit 8:8:8:8 ARGB  
type 1: 16bit 4:4:4:4 ARGB
type 2: 16bit 8:8     AL
*/
//current only type 0 tested
#define FRAME_BUFF_COLOR_TYPE   0

#define RENDER_RESOLUTION_X     160
#define RENDER_RESOLUTION_Y     120
#define VIDEO_BUFF_LENTH              ((RENDER_RESOLUTION_X)*(RENDER_RESOLUTION_Y))

#define HALF_RESOLUTION_X       79.5f
#define HALF_RESOLUTION_Y       59.5f
#define DEFAULT_ASPECT_RATIO    ((4.0f)/(3.0f))
//1.0f == 90 degree fov,smaller is larger fov
#define DEFAULT_FOCUS_LENGTH    (1.0f)

//maybe all here parameter could be dynamic set in render struct to balance render load?
#define FAR_PLANE                    800.0f
#define NEAR_PLANE                   0.1f
#define LEVEL_0_DEFAULT_DISTANCE     400.0f
#define LEVEL_1_DEFAULT_DISTANCE     800.0f
#define LEVEL_1_DEFAULT_LIGHT        0xA0
//level 0, calculate light, texture
//level 1, calculate texture



#define B3L_USING_PARTICLE   
#ifdef B3L_USING_PARTICLE
#define B3L_PARTICLE_BUFF_DEPTH    512
#endif


  


//the obj buff at least has 1 slot
#if OBJ_BUFF_SIZE<=0
#undef OBJ_BUFF_SIZE
#define OBJ_BUFF_SIZE            1
#endif


/*Type defines---------------------------------------------------------------*/

typedef float    f32;
typedef int32_t  s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint32_t q32;

#define B3L_FIX_0_5        (1<<(B3L_FIX_BITS-1))
#define B3L_FIX_1          (1<<B3L_FIX_BITS)

#if Z_BUFF_LEVEL == 0
typedef  u8 zBuff_t;
#endif


#if Z_BUFF_LEVEL == 1
typedef u16 zBuff_t; 
#endif

#if Z_BUFF_LEVEL == 2
typedef f32 zBuff_t;
#endif

#if (FRAME_BUFF_COLOR_TYPE  == 0)
typedef  u32 fBuff_t;
typedef  u32 texLUT_t;
#define LIGHT_BIT           8
#endif
#if (FRAME_BUFF_COLOR_TYPE  == 1)
typedef  u16 fBuff_t;
typedef  u16 texLUT_t;
#define LIGHT_BIT           4
#endif
#if (FRAME_BUFF_COLOR_TYPE  == 2)
typedef  u16 fBuff_t;
typedef  u8  texLUT_t;
#define LIGHT_BIT           8
#endif


typedef struct{
    f32                 x;
    f32                 y;   
}vect2_t;

typedef struct{
    f32                 x;
    f32                 y;  
    f32                 z; 
}vect3_t;

//screen3_t is for 2d screen drawing step, it has same length as vect4_t
#define B3L_IN_SPACE             (0u)
#define B3L_NEAR_PLANE_CLIP      (1u)
typedef struct{
    int32_t             x;
    int32_t             y; 
    f32                 z;
    u32                 test;
}screen3_t;

typedef struct{
    f32                 x;
    f32                 y; 
    f32                 z;
    u32                 test;
}screen3f_t;

typedef struct{
    f32                 x;
    f32                 y; 
    f32                 z;
    f32                 w;  
}vect4_t;

//in column first order, mxy -- x is column num, y is the row number
typedef struct{
    //row0  row1    row2    row3    
    f32 m00;f32 m01;f32 m02;f32 m03;//column 0
    f32 m10;f32 m11;f32 m12;f32 m13;//column 1
    f32 m20;f32 m21;f32 m22;f32 m23;//column 2
    f32 m30;f32 m31;f32 m32;f32 m33;//column 3
}mat4_t;

typedef struct{
    vect3_t             rotation;
    vect3_t             scale;
    vect3_t             translation;
}transform3D_t;

typedef struct{
    f32                 aspectRate;
    f32                 focalLength;
    transform3D_t       transform;
    mat4_t              camMat;
}camera_t;

typedef struct {
    u16                 vectNum;
    u16                 lineNum;
    f32                 *pVect;
    u8                  *pLine;  
}B3L_Polygon_t;

typedef struct {
    u16                 vectNum;
    u16                 triNum;
    f32                 *pVect;
    u16                 *pTri;
    u8                  *pColorIdx;
    f32                 *pNormal;
}B3L_Mesh_NoTex_t;

typedef struct {
    u16                 vectNum;
    u16                 triNum;
    f32                 *pVect;
    u16                 *pTri;
    u8                  *pUv;
    f32                 *pNormal;
}B3L_Mesh_t;

//36 byte for single particle
typedef struct B3L_PARTICLE{
    struct B3L_PARTICLE *next;
    uint32_t            state;
    uint32_t            life;
    vect3_t             position;
    vect3_t             delta;
}B3L_Particle_t;



#define LUT4         0
#define LUT16        1
#define LUT256       2
typedef struct{
    u16                 type;
    u16                 uvSize;
    texLUT_t            *pLUT;
    u8                  *pData;
    u8                  transColorIdx; 
}B3L_texture_t ;

/*
B3LObj_t state
   31     2423     1615      87
   ------------------------------------
31|        |      II|    HGFE|   DCB A|0
  ------------------------------------
  A-- mesh obj with texture
  B-- polygon obj
  C-- mesh obj without texture
  D-- particle obj
  E-- obj visualization
  F-- Back face culling clock wise
  G-- Back face culling anti-clock wise
  H-- fix render level switch
  I-- fix render level number
*/
#define OBJ_TYPE_MASK            0x000000FF
#define MESH_OBJ                    (0)
//the texture 2d obj should be seperated and sorted before draw, then we could use
//alpha mix correctly
//so the new plan would be:draw all 3d objs, then 2d objs overthen with z buff check
#define POLYGON_OBJ                 (1)
#define NOTEX_MESH_OBJ              (2)
#define PARTICLE_OBJ                (3)
#define BITMAP_OBJ                  (4)
//obj visualizable control
#define OBJ_VISUALIZABLE            (8)

#define OBJ_BACK_CULLING_CLOCK            (9)
#define OBJ_BACK_CULLING_ANTICLOCK        (10)
#define OBJ_CILLING_MASK             0x00000600  
#define OBJ_CILLING_SHIFT                  (9)
#define OBJ_IGNORE_RENDER_LEVEL            (11)
//render stage information
#define OBJ_RENDER_LEVEL_MASK        0x00030000
#define OBJ_FIX_RENDER_LEVEL_SHIFT         (16)

//all different obj types's size is <= sizeof(B3LObj_t)
typedef struct B3LOBJ{
    struct B3LOBJ       *privous;
    struct B3LOBJ       *next;
    u32                 state;
    f32                 *pBoundBox;
    transform3D_t       transform;  
    #ifdef B3L_ARM      
    u32                 dummy[2];
    #else    
    u32                 dummy[5];
    #endif
}B3LObj_t;//15 words on ARM32, 20words on win64

typedef struct{
    B3LObj_t            *privous;
    B3LObj_t            *next;
    u32                 state;
    f32                 *pBoundBox;
    transform3D_t       transform; 
    B3L_Mesh_t          *pMesh;
    B3L_texture_t       *pTexture;   
}B3LMeshObj_t;//15 on ARM32 20 on win64


typedef struct{
    B3LObj_t            *privous;
    B3LObj_t            *next;
    u32                 state;
    f32                 *pBoundBox;
    transform3D_t       transform; 
    B3L_Mesh_NoTex_t    *pMesh; 
    texLUT_t            *pLUT;
}B3LMeshNoTexObj_t;//15 on ARM32 20 on win64

typedef struct{
    B3LObj_t            *privous;
    B3LObj_t            *next;
    u32                 state;
    f32                 *pBoundBox;
    transform3D_t       transform; 
    B3L_Polygon_t       *pPolygon; 
    texLUT_t            color;
}B3LPolygonObj_t;//15 on ARM32 19 on win64

typedef struct{
    B3LObj_t            *privous;
    B3LObj_t            *next;
    u32                 state;
}B3LBitmapObj_t;
/*
B3LParticleGenObj_t state
   31     2423     1615      87
   ------------------------------------
31|        |      II|    H FE|   DCB A|0
  ------------------------------------
  A-- mesh obj with texture
  B-- polygon obj
  C-- mesh obj without texture
  D-- particle obj
  E-- obj visualization
  F-- obj Active
  
  H-- fix render level switch
  I-- fix render level number
*/
#define OBJ_PARTICLE_ACTIVE            (9)
//typedef void (*B3L_PtlUpdFunc_t)(u32, mat4_t *, B3L_Particle_t *);
//typedef void (*B3L_DrawFunc_t)(B3L_Particle_t *, screen3_t *,fBuff_t *,zBuff_t *);
//user need to provide 2 functions to update particle state and draw methods
typedef struct PARTICLEGENOBJ{
    B3LObj_t            *privous;
    B3LObj_t            *next;
    u32                 state;
    B3LObj_t            *mother;//option, which could help to bound generater to other obj
    vect3_t             translation;//for particle generate 
    vect3_t             rotation;//for particle generate 
    u32                 lastTime;
    u32                 particleNum;
    B3L_Particle_t      *pParticleActive;   
    void      (*DrawFunc)(B3L_Particle_t *, screen3_t *,fBuff_t *,zBuff_t *);
    void      (*PtlUpdFunc)(u32,struct PARTICLEGENOBJ *,mat4_t *,u32 *,B3L_Particle_t *);   
    //time, self, obj->world matrix,free particle num pointer,free particle pool  
}B3LParticleGenObj_t; //15 words on ARM32 21 words on win64


typedef struct{
    B3LObj_t            objBuff[OBJ_BUFF_SIZE];
    u32                 freeObjNum; 
    B3LObj_t            *pFreeObjs;   
    B3LObj_t            *pActiveMeshObjs;
#ifdef B3L_USING_PARTICLE
    B3LObj_t            *pActiveParticleGenObjs;
    u32                 freeParticleNum;
    B3L_Particle_t      *pfreeParticles;
#endif
}scene_t;

//state
typedef enum {   
    parallelLight = 0,
    dotLight = 1,
}lightType_e;
/*
light_t state
   31     2423     1615      87
   ------------------------------------
31|        |        |        |       A|0
  ------------------------------------
A-- 1, point light
    0, parallel light
*/
#define LIGHT_TYPE_BIT         (0)
//POINT_LIGHT            (1)
//PARALLEL_LIGHT         (0)
typedef struct{
    u32                 state;
    vect3_t             lightVect;
    vect4_t             pointLightVectInCamSpaceBuff;
    u32                 color;
    f32                 factor_0;
    f32                 factor_1;
}light_t;

typedef struct{   
    fBuff_t             *pFrameBuff;
    zBuff_t             *pZBuff;
    screen3f_t          *pVectBuff;
    camera_t            camera;
    light_t             light;    
    scene_t             scene;
    f32                 lvl0Distance;
    f32                 lvl1Distance;
    u8                  lvl1Light;
}render_t;   

/*Useful macros--------------------------------------------------------------*/
#define B3L_SET(PIN,N)  (PIN |=  (1u<<N))
#define B3L_CLR(PIN,N)  (PIN &= ~(1u<<N))
#define B3L_TEST(PIN,N) (PIN & (1u<<N))

#define B3L_logVec2(v)\
  printf("Vector 2: %.3f %.3f\n",((v).x),((v).y))
#define B3L_logVec3(v)\
  printf("Vector 3: %.3f %.3f %.3f\n",((v).x),((v).y),((v).z))
#define B3L_logVec4(v)\
  printf("Vector 4: %.3f %.3f %.3f %.3f\n",((v).x),((v).y),((v).z),((v).w))
#define B3L_logMat4(m)\
  printf("Matix4:\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n"\
   ,(m).m00,(m).m10,(m).m20,(m).m30,\
    (m).m01,(m).m11,(m).m21,(m).m31,\
    (m).m02,(m).m12,(m).m22,(m).m32,\
    (m).m03,(m).m13,(m).m23,(m).m33)

/*Function declear-----------------------------------------------------------*/
//math functions
extern f32      B3L_sin(f32 in);
extern f32      B3L_cos(f32 in);
extern f32      B3L_asin(f32 in);
extern void     B3L_NormalizeVec3(vect3_t *pV);
extern vect2_t  B3L_Vect2(f32 x,f32 y);
extern vect3_t  B3L_Vect3(f32 x,f32 y,f32 z);
extern vect4_t  B3L_Vect4(f32 x,f32 y,f32 z,f32 w);
extern f32      B3L_Vec2Length(vect2_t *pV);
extern f32      B3L_Vec3Length(vect3_t *pV);
extern void     B3L_Vec3Add(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc);
extern void     B3L_VecInterp(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc,f32 t);
extern void     B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult);
extern f32      B3L_DotProductVect3(vect3_t *pA, vect3_t *pB);
extern void     B3L_InitMat4One(mat4_t *pMat);
extern void     B3L_TransposeMat4(mat4_t *pMat);
extern void     B3L_Mat4Xmat4(mat4_t *pMat1, mat4_t *pMat2);
extern void     B3L_MakeRotationMatrixZXY(f32 byX,f32 byY,f32 byZ,mat4_t *pMat);
extern void     B3L_MakeScaleMatrix(f32 scaleX,f32 scaleY,f32 scaleZ,mat4_t *pMat);
extern void     B3L_MakeTranslationMat(f32 offsetX,f32 offsetY,f32 offsetZ,mat4_t *pMat);
extern void     B3L_MakeWorldMatrix(transform3D_t *pWorldTransform, mat4_t *pMat);
//camera functions
extern void     B3L_InitCamera(camera_t *pCam);
extern void     B3L_CameraMoveTo(vect3_t position,camera_t *pCam);
extern void     B3L_CameraLookAt(camera_t *pCam, vect3_t *pAt);
//extern void B3L_CameraUpDirection(camera_t *pCam, vect3_t *pUp);
//render functions
extern void     B3L_RenderInit(render_t *pRender,fBuff_t *pFrameBuff);
extern void     B3L_NewRenderStart(render_t *pRender);
extern void     B3L_Update(render_t *pRender,u32 time);
extern void     B3L_RenderScence(render_t *pRender);
extern void     B3L_ResetScene(scene_t *pScene);
//light functions
extern void     B3L_ResetLight(light_t *pLight);
extern void     B3L_SetLightType(render_t *pRender,lightType_e type);
extern void     B3L_SetLightVect(render_t *pRender, f32 x,f32 y,f32 z);
//render obj functions
extern u32      B3L_GetFreeObjNum(render_t *pRender);
extern u32      B3L_GetFreeParticleNum(render_t *pRender);
extern B3LObj_t *B3L_GetFreeObj(render_t *pRender);
extern void     B3L_AddObjToRenderList(B3LObj_t *pObj, render_t *pRender);
extern void     B3L_PopObjFromRenderList(B3LObj_t *pObj, render_t *pRender);
extern void     B3L_ReturnObjToInactiveList(B3LObj_t *pObj,  render_t *pRender);
//helper function for testing objs
extern void     B3L_InitBoxObj(B3LMeshObj_t *pObj,f32 size);
extern void     B3L_InitBoxObjNoTexture(B3LMeshNoTexObj_t *pObj,f32 size);
extern void     B3L_InitBoxObjPolygon(B3LPolygonObj_t *pObj,f32 size);
//particle function
extern void     B3L_DefaultParticleDrawFunc(B3L_Particle_t *pParticle, screen3f_t *pScreenVect,fBuff_t *pFBuff,zBuff_t *pZBuff);

//extern void RenderMeshObjs(render_t *pRender);
/*
void DrawTriTexture(
s32 x0,s32 y0,s32 u0,s32 v0,f32 z0,
s32 x1,s32 y1,s32 u1,s32 v1,f32 z1,
s32 x2,s32 y2,s32 u2,s32 v2,f32 z2,
u32 renderLevel,u16 lightFactor,B3L_texture_t *pTexture,
fBuff_t *pFrameBuff,zBuff_t *pZbuff);
*/
/*
extern void  DrawTriColor(
                                                                        f32 x0,f32 y0,f32 z0,
                                                                        f32 x1,f32 y1,f32 z1,
                                                                        f32 x2,f32 y2,f32 z2,
                                                                        u32 renderLevel,u8 lightFactor,fBuff_t color,
                                                                        fBuff_t *pFrameBuff,zBuff_t *pZbuff);
*/
#endif
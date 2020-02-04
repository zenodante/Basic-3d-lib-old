#ifndef __B3DLIB_H__
#define __B3DLIB_H__


#include <stdint.h>
#include <math.h>

//#define B3L_DEBUG
/*Config area----------------------------------------------------------------*/
//z buff level 0: uint8_t, level 1: uint16_t, level 2: f32_t
//RGBA -> at 0
//ARGB -> at 3
#define B3L_ALPHA_CHANNEL_SHIFT   0

#define B3L_MATH_TABLE_SIZE     128
#define VECT_BUFF_SIZE          512
#define OBJ_BUFF_SIZE           64
#define Z_BUFF_LEVEL            2


#define RENDER_RESOLUTION_X     160
#define RENDER_RESOLUTION_Y     120
#define BUFF_LENTH              (160*120)

#define HALF_RESOLUTION_X       79.5f
#define HALF_RESOLUTION_Y       59.5f
#define DEFAULT_ASPECT_RATIO    ((4.0f)/(3.0f))
//1.0f == 90 degree fov
#define DEFAULT_FOCUS_LENGTH    (1.0f)
#define FAR_PLANE               500.0f
#define NEAR_PLANE              5.0f
#define LEVEL_0_DISTANCE        300.0f
#define LEVEL_1_DISTANCE        500.0f
//level 0, calculate light, texture
//level 1, calculate texture
//level 2, calculate color only
#define USING_LIGHT             1
  

#define B3L_IN_SPACE             (0u)
#define B3L_NEAR_PLANE_CLIP      (1u)
/*Type defines---------------------------------------------------------------*/

typedef float f32_t;
typedef  int32_t s32;
#if Z_BUFF_LEVEL == 0
#define Z_buff_t uint8_t
#endif


#if Z_BUFF_LEVEL == 1
#define Z_buff_t uint16_t
#endif

#if Z_BUFF_LEVEL == 2
typedef f32_t Z_buff_t;
#endif


typedef  uint32_t frameBuffData_t;

typedef struct{
    f32_t x;
    f32_t y;   
}vect2_t;

typedef struct{
    f32_t x;
    f32_t y;  
    f32_t z; 
}vect3_t;


typedef struct{
    int32_t x;
    int32_t y; 
    f32_t z;
    uint32_t test;
}screen3_t;


typedef struct{
    f32_t x;
    f32_t y; 
    f32_t z;
    f32_t w;  
}vect4_t;




typedef struct{
    f32_t m00;
    f32_t m01;
    f32_t m02;
    f32_t m03;
    f32_t m10;
    f32_t m11;
    f32_t m12;
    f32_t m13;
    f32_t m20;
    f32_t m21;
    f32_t m22;
    f32_t m23;
    f32_t m30;
    f32_t m31;
    f32_t m32;
    f32_t m33;
}mat4_t;
typedef struct{
    vect3_t rotation;
    vect3_t scale;
    vect3_t translation;
}transform3D_t;

typedef struct{
    f32_t          aspectRate;
    f32_t          focalLength;
    transform3D_t  transform;
    mat4_t         camMat;
}camera_t;

typedef struct {
    uint32_t        id;
    uint16_t        vectNum;
    uint16_t        triNum;
    f32_t           *pVect;
    uint16_t        *pTri;
    uint8_t         *pUv;
    f32_t           *pNormal;
}B3L_Mesh_t;


#define LUT4         0
#define LUT16        1
#define LUT256       2
#define RGB565       3
#define RGBX32       4
typedef struct{
    uint32_t        id;
    uint16_t        type;
    uint16_t        uvSize;
    frameBuffData_t *pLUT;
    uint8_t         *pData;
}B3L_texture_t ;

//obj type information
#define OBJ_TYPE_MASK            0x000000FF
#define MESH_OBJ                    (0)
#define TEXTURE2D_OBJ               (1)
#define POLYGON_OBJ                 (2)
#define NOTEX_MESH_OBJ              (3)
//obj visualizable information
#define OBJ_VISUALIZABLE            (8)
#define OBJ_BACK_CULLING            (9)
#define OBJ_IGNORE_RENDER_LEVEL     (10)
//render stage information
#define OBJ_RENDER_LEVEL_MASK    0x00030000
#define OBJ_FIX_RENDER_LEVEL_SHIFT  (16)


typedef struct B3LOBJ{
    struct B3LOBJ   *privous;
    struct B3LOBJ   *next;
    uint32_t        state;
    f32_t           *pBoundBox;
    transform3D_t   transform;    
    uint32_t        dummy[4];
}B3LObj_t;

typedef struct{
    B3LObj_t        *privous;
    B3LObj_t        *next;
    uint32_t        state;
    f32_t           *pBoundBox;
    transform3D_t   transform; 
    B3L_Mesh_t      *pMesh;
    B3L_texture_t   *pTexture;   
}B3LMeshObj_t;

typedef struct{
    B3LObj_t       objBuff[OBJ_BUFF_SIZE];    
    B3LObj_t       *pActiveObjs;
    //B3LObj_t       *pViewableObjs;
    B3LObj_t       *pInactiveObjs;

}scene_t;

typedef struct{
    vect3_t      pointToLight;
    f32_t        strongth;
    uint32_t     color;
    f32_t        factor_0;
    f32_t        factor_1;
}light_t;

#define NOT_DRAW_TRI            (0u)
#define TRI_NEAR_PLANE_CLIP     (1u)
#define TRI_NEAR_PLANE_1_POINT  (2u)
#define TRI_NEAR_PLANE_2_POINT  (3u)
typedef struct{
    
    frameBuffData_t  *pFrameBuff;
    Z_buff_t         *pZBuff;
    camera_t         camera;
    light_t          light;    
    scene_t          scene;
    screen3_t        *pVectBuff;
}render_t;

/*Function declear-----------------------------------------------------------*/
extern f32_t B3L_sin(f32_t in);
extern f32_t B3L_cos(f32_t in);
extern f32_t B3L_asin(f32_t in);
extern void B3L_NormalizeVec3(vect3_t *pV);
extern f32_t B3L_Vec2Length(vect2_t *pV);
extern f32_t B3L_Vec3Length(vect3_t *pV);
extern void B3L_Vec3Add(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc);
extern void B3L_VecInterp(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc,f32_t t);
extern void B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult);
extern f32_t B3L_DotProductVect3(vect3_t *pA, vect3_t *pB);
extern void B3L_InitMat4(mat4_t *pMat);
extern void B3L_TransposeMat4(mat4_t *pMat);
extern void B3L_Mat4Xmat4(mat4_t *pMat1, mat4_t *pMat2);

extern void B3L_InitCamera(camera_t *pCam);
extern void B3L_SetCameraMatrix(camera_t *pCam);
extern void B3L_CameraLookAt(camera_t *pCam, vect3_t *pAt);

extern void B3L_MakeRotationMatrixZXY(f32_t byX,f32_t byY,f32_t byZ,mat4_t *pMat);
extern void B3L_MakeScaleMatrix(f32_t scaleX,f32_t scaleY,f32_t scaleZ,mat4_t *pMat);
extern void B3L_MakeTranslationMat(f32_t offsetX,f32_t offsetY,f32_t offsetZ,mat4_t *pMat);
extern void B3L_MakeWorldMatrix(transform3D_t *pWorldTransform, mat4_t *pMat);




extern void B3L_RenderInit(render_t *pRender,frameBuffData_t *pFrameBuff);
extern void B3L_NewFrame(render_t *pRender);

//render obj functions
B3LObj_t * B3L_GetFreeObj(render_t *pRender);
extern void B3L_AddObjToRenderList(B3LObj_t *pObj, render_t *pRender);
extern void B3L_PopObjFromRenderList(B3LObj_t *pObj, render_t *pRender);
extern void B3L_ReturnObjToInactiveList(B3LObj_t *pObj,  render_t *pRender);
extern void B3L_DrawObjs(render_t *pRender);
extern void B3L_InitBoxObj(B3LMeshObj_t *pObj,f32_t size);
extern void B3L_ResetLight(light_t *pLight);
extern void B3L_ResetScene(scene_t *pScene);
extern void B3L_RenderScence(render_t *pRender);
/*
void B3L_DrawTriTexture(
s32 x0,s32 y0,s32 u0,s32 v0,f32_t z0,
s32 x1,s32 y1,s32 u1,s32 v1,f32_t z1,
s32 x2,s32 y2,s32 u2,s32 v2,f32_t z2,
uint32_t renderLevel,uint16_t lightFactor,B3L_texture_t *pTexture,
frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff);
*/



#endif
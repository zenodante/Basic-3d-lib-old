#ifndef __B3DLIB_H__
#define __B3DLIB_H__


#include <stdint.h>
#include <math.h>

//#define ARM_CORTEX_M
//#define B3L_ARM_GCC
//#define B3L_ARM_IAR


//#define B3L_DEBUG
/*Config area----------------------------------------------------------------*/
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
#define BUFF_LENTH              ((RENDER_RESOLUTION_X)*(RENDER_RESOLUTION_Y))

#define HALF_RESOLUTION_X       79.5f
#define HALF_RESOLUTION_Y       59.5f
#define DEFAULT_ASPECT_RATIO    ((4.0f)/(3.0f))
//1.0f == 90 degree fov,smaller is larger fov
#define DEFAULT_FOCUS_LENGTH    (1.0f)

#define FAR_PLANE               500.0f
#define NEAR_PLANE              1.0f
#define LEVEL_0_DISTANCE        300.0f
#define LEVEL_1_DISTANCE        500.0f
//level 0, calculate light, texture
//level 1, calculate texture
//level 2, calculate color only


  
/*bit defines and setting area-----------------------------------------------*/

//the obj buff at least has 1 slot
#if OBJ_BUFF_SIZE<=0
#undef OBJ_BUFF_SIZE
#define OBJ_BUFF_SIZE            1
#endif

#define B3L_IN_SPACE             (0u)
#define B3L_NEAR_PLANE_CLIP      (1u)

#define B3L_MATH_TABLE_SIZE      128
/*Type defines---------------------------------------------------------------*/

typedef float    f32;
typedef int32_t  s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t  u8;
typedef int8_t   s8;

#if Z_BUFF_LEVEL == 0
#define Z_buff_t u8
#endif


#if Z_BUFF_LEVEL == 1
#define Z_buff_t u16
#endif

#if Z_BUFF_LEVEL == 2
typedef f32 Z_buff_t;
#endif

#if (FRAME_BUFF_COLOR_TYPE  == 0)
typedef  u32 frameBuffData_t;
typedef  u32 texLUTData_t;
#define LIGHT_BIT           8
#endif
#if (FRAME_BUFF_COLOR_TYPE  == 1)
typedef  u16 frameBuffData_t;
typedef  u16 texLUTData_t;
#define LIGHT_BIT           4
#endif
#if (FRAME_BUFF_COLOR_TYPE  == 2)
typedef  u16 frameBuffData_t;
typedef  u8  texLUTData_t;
#define LIGHT_BIT           8
#endif


typedef struct{
    f32 x;
    f32 y;   
}vect2_t;

typedef struct{
    f32 x;
    f32 y;  
    f32 z; 
}vect3_t;

//screen3_t is for 2d screen drawing step, it has same length as vect4_t

typedef struct{
    int32_t x;
    int32_t y; 
    f32 z;
    u32 test;
}screen3_t;


typedef struct{
    f32 x;
    f32 y; 
    f32 z;
    f32 w;  
}vect4_t;

//in column first order, mxy -- x is column num, y is the row number
typedef struct{
    f32 m00;f32 m01;f32 m02;f32 m03;
    f32 m10;f32 m11;f32 m12;f32 m13;
    f32 m20;f32 m21;f32 m22;f32 m23;
    f32 m30;f32 m31;f32 m32;f32 m33;
}mat4_t;

typedef struct{
    vect3_t rotation;
    vect3_t scale;
    vect3_t translation;
}transform3D_t;

typedef struct{
    f32            aspectRate;
    f32            focalLength;
    transform3D_t  transform;
    mat4_t         camMat;
}camera_t;

typedef struct {
    u32        id;
    u16        vectNum;
    u16        triNum;
    f32        *pVect;
    u16        *pTri;
    u8    *pUv;
    f32        *pNormal;
}B3L_Mesh_t;


#define LUT4         0
#define LUT16        1
#define LUT256       2
typedef struct{
    u32            id;
    u16            type;
    u16            uvSize;
    texLUTData_t   *pLUT;
    u8        *pData;
    u8        transColorIdx;
}B3L_texture_t ;

//obj->state config bits
//obj type information
#define OBJ_TYPE_MASK            0x000000FF
#define MESH_OBJ                    (0)
#define TEXTURE2D_OBJ               (1)
#define POLYGON_OBJ                 (2)
#define NOTEX_MESH_OBJ              (3)
//obj visualizable control
#define OBJ_VISUALIZABLE            (8)
#define OBJ_BACK_CULLING            (9)
#define OBJ_IGNORE_RENDER_LEVEL     (10)
//render stage information
#define OBJ_RENDER_LEVEL_MASK    0x00030000
#define OBJ_FIX_RENDER_LEVEL_SHIFT  (16)

//all different obj types's size is <= sizeof(B3LObj_t)
typedef struct B3LOBJ{
    struct B3LOBJ   *privous;
    struct B3LOBJ   *next;
    u32             state;
    f32             *pBoundBox;
    transform3D_t   transform;    
    u32             dummy[4];
}B3LObj_t;

typedef struct{
    B3LObj_t        *privous;
    B3LObj_t        *next;
    u32             state;
    f32             *pBoundBox;
    transform3D_t   transform; 
    B3L_Mesh_t      *pMesh;
    B3L_texture_t   *pTexture;   
}B3LMeshObj_t;

typedef struct{
    B3LObj_t       objBuff[OBJ_BUFF_SIZE];    
    B3LObj_t       *pActiveObjs;
    B3LObj_t       *pInactiveObjs;
}scene_t;


//color value is the 0xFFFFFF - light color
typedef struct{
    vect3_t      pointToLight;
    f32          strongth;
    u32          color;
    f32          factor_0;
    f32          factor_1;
}light_t;

typedef struct{   
    frameBuffData_t  *pFrameBuff;
    Z_buff_t         *pZBuff;
    camera_t         camera;
    light_t          light;    
    scene_t          scene;
    screen3_t        *pVectBuff;
}render_t;
/*Useful macros--------------------------------------------------------------*/
#define B3L_SET(PIN,N)  (PIN |=  (1u<<N))
#define B3L_CLR(PIN,N)  (PIN &= ~(1u<<N))
#define B3L_TEST(PIN,N) (PIN & (1u<<N))
#define B3L_logVec2(v)\
  printf("Vec3: %.3f %.3f\n",((v).x),((v).y))
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

/*Function declear-----------------------------------------------------------*/
//math functions
extern f32 B3L_sin(f32 in);
extern f32 B3L_cos(f32 in);
extern f32 B3L_asin(f32 in);
extern void B3L_NormalizeVec3(vect3_t *pV);
extern vect2_t B3L_Vect2(f32 x,f32 y);
extern vect3_t B3L_Vect3(f32 x,f32 y,f32 z);
extern vect4_t B3L_Vect4(f32 x,f32 y,f32 z,f32 w);
extern f32 B3L_Vec2Length(vect2_t *pV);
extern f32 B3L_Vec3Length(vect3_t *pV);
extern void B3L_Vec3Add(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc);
extern void B3L_VecInterp(vect3_t *pVa,vect3_t *pVb,vect3_t *pVc,f32 t);
extern void B3L_CrossProductVect3(vect3_t *pA, vect3_t *pB, vect3_t *pResult);
extern f32 B3L_DotProductVect3(vect3_t *pA, vect3_t *pB);
extern void B3L_InitMat4(mat4_t *pMat);
extern void B3L_TransposeMat4(mat4_t *pMat);
extern void B3L_Mat4Xmat4(mat4_t *pMat1, mat4_t *pMat2);
extern void B3L_MakeRotationMatrixZXY(f32 byX,f32 byY,f32 byZ,mat4_t *pMat);
extern void B3L_MakeScaleMatrix(f32 scaleX,f32 scaleY,f32 scaleZ,mat4_t *pMat);
extern void B3L_MakeTranslationMat(f32 offsetX,f32 offsetY,f32 offsetZ,mat4_t *pMat);
extern void B3L_MakeWorldMatrix(transform3D_t *pWorldTransform, mat4_t *pMat);
//camera functions
extern void B3L_InitCamera(camera_t *pCam);
extern void B3L_CameraMoveTo(vect3_t position,camera_t *pCam);
extern void B3L_SetCameraMatrix(camera_t *pCam);
extern void B3L_CameraLookAt(camera_t *pCam, vect3_t *pAt);
//extern void B3L_CameraUpDirection(camera_t *pCam, vect3_t *pUp);
//render functions
extern void B3L_RenderInit(render_t *pRender,frameBuffData_t *pFrameBuff);
extern void B3L_NewFrame(render_t *pRender);
extern void B3L_RenderScence(render_t *pRender);
extern void B3L_ResetScene(scene_t *pScene);
//light functions
extern void B3L_ResetLight(light_t *pLight);
//extern void B3L_SetLightDirction(light_t *pLight, vect3_t *To);
//render obj functions
extern B3LObj_t *B3L_GetFreeObj(render_t *pRender);
extern void B3L_AddObjToRenderList(B3LObj_t *pObj, render_t *pRender);
extern void B3L_PopObjFromRenderList(B3LObj_t *pObj, render_t *pRender);
extern void B3L_ReturnObjToInactiveList(B3LObj_t *pObj,  render_t *pRender);
extern void B3L_InitBoxObj(B3LMeshObj_t *pObj,f32 size);

//extern void B3L_DrawObjs(render_t *pRender);
/*
void B3L_DrawTriTexture(
s32 x0,s32 y0,s32 u0,s32 v0,f32 z0,
s32 x1,s32 y1,s32 u1,s32 v1,f32 z1,
s32 x2,s32 y2,s32 u2,s32 v2,f32 z2,
u32 renderLevel,u16 lightFactor,B3L_texture_t *pTexture,
frameBuffData_t *pFrameBuff,Z_buff_t *pZbuff);
*/
#endif
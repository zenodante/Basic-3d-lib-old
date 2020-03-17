#include "32blit.hpp"

extern "C" {
#include "b3dlib.h"
#include "b3dSupport.h"
#include "b3dTestObj.h"
#include "cat.h"
#include "ship2_72tri.h"
#include "b3dDust.h"
}
using namespace blit;


render_t B3Lrender;
B3LParticleGenObj_t  *pParticleGen;
fBuff_t *renderBuff;
B3L_timeTweenCtl_t ttCtl;
B3L_tween_t tCatZ;
B3LMeshObj_t *pShip;
B3LMeshObj_t *pBox;
B3LMeshNoTexObj_t *pColorBox;
vect3_t at={0.0f,0.0f,0.0f};
vect3_t up ={0.0f,1.0f,0.0f};
f32 distance = 200.0f;
vect3_t angle={0.125f,0.0f,0.0f};
u16 frame4[320*240];
void init() {
    blit::set_screen_mode(ScreenMode::hires);
    //init rendom seed
    B3L_SetSeed(0x31415926);
    //init time and tween system
    B3L_TimerTweenCtlInit(&ttCtl);
    B3L_TweenInit(&ttCtl,&tCatZ,B3L_tween_linear,0.0f, 1.0f, 4000, -1);
    //init the main render
    fBuff_t *renderBuff=B3L_3dRenderAreaShiftCal(((fBuff_t *)frame4),0, 0);   
    B3L_RenderInit(&B3Lrender,renderBuff);
    //init particle generator
    pBox = B3L_GetFreeMeshObj(&B3Lrender);
    pColorBox = B3L_GetFreeMeshNoTexObj(&B3Lrender);
    B3L_InitBoxObj(pBox,5.0f);
    B3L_InitBoxObjNoTexture(pColorBox,5.0f);
    B3L_AddObjToRenderList((B3LObj_t *)pBox, &B3Lrender);
    B3L_AddObjToRenderList((B3LObj_t *)pColorBox, &B3Lrender);
    pBox->transform.translation.z = -10.0f;
    pColorBox->transform.translation.x = -10.0f;
    //pParticleGen = B3L_GetFreeParticleGeneratorObj(&B3Lrender);
    //B3L_InitDemoParticleGenObj(pParticleGen);
    //pParticleGen->transform.translation.x = 0.0f;
    //pParticleGen->transform.translation.y = 0.0f;
    //B3L_AddObjToRenderList((B3LObj_t *)pParticleGen, &B3Lrender);
    //init light
    B3L_SetLightType(&B3Lrender,parallelLight);
    //B3L_SetLightVect();
    //B3Lrender.light.factor_0=0.2f;
    //init ship
    pShip = (B3LMeshObj_t *)B3L_GetFreeMeshObj(&B3Lrender);
    
    InitShipObj(pShip,1.5f);
    //pShip->transform.translation.z = 20.0f;
    //pShip->transform.rotation.y=0.5f;
    B3L_AddObjToRenderList((B3LObj_t *)pShip, &B3Lrender);
    //init the camera
    
    B3Lrender.camera.transform.translation = {0.0f,0.0f,-20.0f};
    //B3L_SetOrthographicProject(&(B3Lrender));
    //B3L_CameraLookAt(&(B3Lrender.camera), &at,&up);
    
    //vect3_t eulerAngle;

    B3L_TweenStart(&tCatZ,0);

    //printf("obj %d,particle %d\n",sizeof(B3LObj_t),sizeof(B3LParticleGenObj_t));
    B3L_CamInitTrack(&(B3Lrender.camera),(B3LObj_t *)pShip,0.0f,10.0f,-20.0f,0.0f,0.0f,20.0f);
    B3L_CamStartTrack(&(B3Lrender.camera));

    DustInit();
}

void update(uint32_t time){
    
    
    B3L_Update(&B3Lrender,time);
    B3L_TimerTweenUpdate(&ttCtl,time);



    if (pressed(DPAD_UP)){
        //angle.x -=0.002f;
        //B3Lrender.light.lightVect.z += 0.01f;
        //pShip->transform.rotation.x -= B3L_cos(0.5f-pShip->transform.rotation.z)*0.002f;
        //pShip->transform.rotation.y -= B3L_sin(0.5f-pShip->transform.rotation.z)*0.002f;
        //pShip->transform.rotation.y +=0.002f;
        //pShip->transform.rotation.x +=0.002f;
        //pShip->transform.rotation.x -= (f32)((s32)(pShip->transform.rotation.x));
        //B3Lrender.camera.transform.rotation.x += 0.002f;
        //B3L_RotateObjInOX((B3LObj_t *)pShip,-0.002f);
        ROTATE_IN_BODY_X(pShip,-0.002f);
        //B3L_RotateCamInOX(&(B3Lrender.camera),-0.002f);
        //ROTATE_IN_BODY_X(&(B3Lrender.camera),-0.002f);
        //pShip->transform.translation.z +=0.5f;
        //B3Lrender.camera.transform.translation.y +=1.0f;
    }
    if (pressed(DPAD_DOWN)){
        //pShip->transform.rotation.y -=0.002f;
        //angle.x +=0.002f; 
        //B3Lrender.light.lightVect.z -= 0.01f;
        //pShip->transform.rotation.x -=0.002f;
        //pShip->transform.rotation.y += B3L_sin(0.5f-pShip->transform.rotation.z)*0.002f;
        //pShip->transform.rotation.x += B3L_cos(0.5f-pShip->transform.rotation.z)*0.002f;
        //pShip->transform.rotation.x -= (f32)((s32)(pShip->transform.rotation.x)); 
        //B3Lrender.camera.transform.rotation.x -= 0.002f;
        ROTATE_IN_BODY_X(pShip,0.002f);
        //B3L_RotateCamInOX(&(B3Lrender.camera),0.002f);
        //ROTATE_IN_BODY_X(&(B3Lrender.camera),0.002f);
        //B3Lrender.camera.transform.translation.y -=1.0f;
        //pShip->transform.translation.z -=0.5f;
    }
    if (pressed(DPAD_LEFT)){
        //angle.y +=0.002f;
        //B3Lrender.light.lightVect.x -= 0.01f; 
        //pShip->transform.rotation.z -=0.002f;
        //pShip->transform.rotation.y -= (f32)((s32)(pShip->transform.rotation.y));   
        //B3Lrender.camera.transform.rotation.z -= 0.002f;
        ROTATE_IN_BODY_Y(pShip,-0.002f);
        //B3L_RotateCamInOY(&(B3Lrender.camera),-0.002f);
        //ROTATE_IN_BODY_Y(&(B3Lrender.camera),-0.002f);
        //B3Lrender.camera.transform.translation.x -=1.0f;
    }
    if (pressed(DPAD_RIGHT)){
         //angle.y -=0.002f;  
         //B3Lrender.light.lightVect.x += 0.01f;
         //pShip->transform.rotation.z +=0.002f;
        //pShip->transform.rotation.y -= (f32)((s32)(pShip->transform.rotation.y));
        //B3Lrender.camera.transform.rotation.z += 0.002f;
        ROTATE_IN_BODY_Y(pShip,0.002f);
        //B3L_RotateCamInOY(&(B3Lrender.camera),0.002f);
        //ROTATE_IN_BODY_Y(&(B3Lrender.camera),0.002f);
        //B3Lrender.camera.transform.translation.x +=1.0f;
    }
    if (pressed(A)){
        //B3Lrender.camera.trackDistance += 1.0f;
        //distance += 1.0f;
        ROTATE_IN_BODY_Z(pShip,-0.002f);
    }
    if (pressed(B)){
        //B3Lrender.camera.trackDistance -= 1.0f;
        //distance -= 1.0f;
        ROTATE_IN_BODY_Z(pShip,0.002f);
    }
    SYNC_ROTATION_STATE_NOW(pShip);
    vect3_t frontDirect;
    frontDirect.x = pShip->mat.m02*0.1f;
    frontDirect.y = pShip->mat.m12*0.1f;
    frontDirect.z = pShip->mat.m22*0.1f;
    B3L_Vect3Add(&(pShip->transform.translation),&frontDirect,&(pShip->transform.translation));

    /*
    vect3_t result;
    
    CalTargetPositonAngle(&(pShip->transform.rotation),
                                   &angle, &result);
     

    CameraTrackPoint(&(B3Lrender.camera), &at, &result, distance);
    */
    //CameraTrackPoint(&(B3Lrender.camera), &at, &angle, distance);

    //vect3_t up ={2.0f,3.0f,0.0f};
    //B3L_SetCameraUpDirection(&(B3Lrender.camera),&up);
    //B3L_CameraLookAt(&(B3Lrender.camera), &at,&up);
    
}


void render(uint32_t time) {
    
    B3L_NewRenderStart( &B3Lrender,0xF111);
    B3L_RenderScence(&B3Lrender);
    //B3L_AppliedLightFromAlpha(&B3Lrender);
    DustUpdateAndRender(&B3Lrender,(B3LObj_t *)pShip,time);
    B3L_AppliedLightFromAlpha4444To8888(&B3Lrender,(u32 *)(screen.data));
   /* 
    zBuff_t *pB = B3Lrender.pZBuff;
    u32 *pS = (u32 *)(screen.data);
    u32 i;
    u16 max=0;
    for (i = 0 ;i<320*240;i++){
        u16 zV = pB[i];
        if (zV>max){
            max = zV;
        }
        zV = zV>>8;
        pS[i] = 0xFF<<24|zV<<16|zV<<8|zV;
    }
    
    printf("%d\n",max);
*/
/*
    uint8_t *buff = (uint8_t *)(((fBuff_t *)screen.data)+160*15);
    //uint32_t *u4buff = (uint32_t *)screen.data;
    uint32_t *u4buff =(((fBuff_t *)screen.data)+160*15);
    uint32_t backColor = B3Lrender.light.color;
    uint8_t r,g,b;
    uint32_t i;
    uint32_t color;
    
    for (i=0;i<VIDEO_BUFF_LENTH;i++){
    color = u4buff[i];
    color = color>>24;
    
    color = (color<<24)|(color<<16)|(color<<8)|0xff;

    u4buff[i] = color;
    }
  
*/ 

    
    screen.watermark();
     
}

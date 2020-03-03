#include "32blit.hpp"

extern "C" {
#include "b3dlib.h"
#include "B3L_support.h"
#include "cat.h"
#include "ship2_72tri.h"
}
using namespace blit;


render_t B3Lrender;
B3LParticleGenObj_t  *pParticleGen;
fBuff_t *renderBuff;
B3L_timeTweenCtl_t ttCtl;
B3L_tween_t tCatZ;
B3LMeshObj_t *pShip;
vect3_t at={0.0f,0.0f,0.0f};
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
    pParticleGen = B3L_GetFreeParticleGeneratorObj(&B3Lrender);
    B3L_InitDemoParticleGenObj(pParticleGen);
    pParticleGen->translation.x = 0.0f;
    pParticleGen->translation.y = 0.0f;
    //B3L_AddObjToRenderList((B3LObj_t *)pParticleGen, &B3Lrender);
    //init light
    B3L_SetLightType(&B3Lrender,parallelLight);
    //B3L_SetLightVect();
    //B3Lrender.light.factor_0=0.2f;
    //init ship
    pShip = (B3LMeshObj_t *)B3L_GetFreeMeshObj(&B3Lrender);
    
    InitShipObj(pShip,15.0f);
    //pShip->transform.rotation.y=0.5f;
    B3L_AddObjToRenderList((B3LObj_t *)pShip, &B3Lrender);
    //init the camera
    
    //B3Lrender.camera.transform.translation = {0.0f,0.0f,-200.0f};
    //B3L_CameraLookAt(&(B3Lrender.camera), &at);

    
    B3L_TweenStart(&tCatZ,0);
    B3L_CamSetTrack(&(B3Lrender.camera), (B3LObj_t  *)pShip,200.0f, 0.003f,0.20f, 0.0f, 0.0f);
    B3L_CamStartTrack(&(B3Lrender.camera));

}

void update(uint32_t time){
    
    
    B3L_Update(&B3Lrender,time);
    B3L_TimerTweenUpdate(&ttCtl,time);



    if (pressed(DPAD_UP)){
        //angle.x -=0.002f;
        //B3Lrender.light.lightVect.z += 0.01f;
        pShip->transform.rotation.x +=0.002f;
        pShip->transform.rotation.x -= (f32)((s32)(pShip->transform.rotation.x));
    }
    if (pressed(DPAD_DOWN)){
        //angle.x +=0.002f; 
        //B3Lrender.light.lightVect.z -= 0.01f;
        pShip->transform.rotation.x -=0.002f;
        pShip->transform.rotation.x -= (f32)((s32)(pShip->transform.rotation.x)); 
    }
    if (pressed(DPAD_LEFT)){
        //angle.y +=0.002f;
        //B3Lrender.light.lightVect.x -= 0.01f; 
        pShip->transform.rotation.y +=0.002f;
        pShip->transform.rotation.y -= (f32)((s32)(pShip->transform.rotation.y));   
    }
    if (pressed(DPAD_RIGHT)){
         //angle.y -=0.002f;  
         //B3Lrender.light.lightVect.x += 0.01f;
         pShip->transform.rotation.y -=0.002f;
        pShip->transform.rotation.y -= (f32)((s32)(pShip->transform.rotation.y));
    }
    if (pressed(A)){
        B3Lrender.camera.trackDistance += 1.0f;
        //distance += 1.0f;
    }
    if (pressed(B)){
        B3Lrender.camera.trackDistance -= 1.0f;
        //distance -= 1.0f;
    }
    /*
    vect3_t result;
    
    CalTargetPositonAngle(&(pShip->transform.rotation),
                                   &angle, &result);
     

    CameraTrackPoint(&(B3Lrender.camera), &at, &result, distance);
    */
    //CameraTrackPoint(&(B3Lrender.camera), &at, &angle, distance);


    //CameraTrackPoint(&(B3Lrender.camera), &at, &angle, distance);
    //B3L_CameraLookAt(&(B3Lrender.camera), &at);
    //vect3_t up ={2.0f,3.0f,0.0f};
    //B3L_SetCameraUpDirection(&(B3Lrender.camera),&up);

}


void render(uint32_t time) {
    
    B3L_NewRenderStart( &B3Lrender,0xF111);
    B3L_RenderScence(&B3Lrender);
    //B3L_AppliedLightFromAlpha(&B3Lrender);
    B3L_AppliedLightFromAlpha4444To8888(&B3Lrender,(u32 *)screen.data);


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

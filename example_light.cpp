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
B3LMeshObj_t *pBox[60];
vect3_t at={0.0f,0.0f,0.0f};
void init() {
    blit::set_screen_mode(ScreenMode::lores);
    //init rendom seed
    B3L_SetSeed(0x31415926);
    //init time and tween system
    B3L_TimerTweenCtlInit(&ttCtl);
    B3L_TweenInit(&ttCtl,&tCatZ,B3L_tween_linear,0.0f, 1.0f, 4000, -1);
    //init the main render
    fBuff_t *renderBuff=B3L_3dRenderAreaShiftCal(((fBuff_t *)screen.data),0, 0);   
    B3L_RenderInit(&B3Lrender,renderBuff);
    //init particle generator
    pParticleGen = B3L_GetFreeParticleGeneratorObj(&B3Lrender);
    B3L_InitDemoParticleGenObj(pParticleGen);
    pParticleGen->translation.x = 0.0f;
    pParticleGen->translation.y = 0.0f;
    B3L_AddObjToRenderList((B3LObj_t *)pParticleGen, &B3Lrender);
    //init light
    B3L_SetLightType(&B3Lrender,dotLight);
    //B3Lrender.light.factor_0=0.2f;
    //
    int i = 20;
    for(i=0;i<20;i++){
        f32 degree = ((f32)i)/20.0f;
        f32 x = 120.0f*B3L_sin(degree);
        f32 z = 120.0f*B3L_cos(degree);
        pBox[i] = (B3LMeshObj_t *)(B3L_GetFreeMeshObj(&B3Lrender));
        B3L_InitBoxObj(pBox[i],20.0f);
        B3L_AddObjToRenderList((B3LObj_t *)(pBox[i]), &B3Lrender);
        //add to middle of the ball
        pBox[i]->transform.translation={x,0.0,z};

    }
    for(i=0;i<10;i++){
        f32 degree = ((f32)i)/10.0f;
        f32 x = 60.0f*B3L_sin(degree);
        f32 z = 60.0f*B3L_cos(degree);
        pBox[20+2*i] = (B3LMeshObj_t *)(B3L_GetFreeMeshObj(&B3Lrender));
        B3L_InitBoxObj(pBox[20+2*i],20.0f);
        B3L_AddObjToRenderList((B3LObj_t *)(pBox[20+2*i]), &B3Lrender);
        pBox[20+2*i]->transform.translation={x,60.0f,z};

        pBox[20+2*i+1] = (B3LMeshObj_t *)(B3L_GetFreeMeshObj(&B3Lrender));
        B3L_InitBoxObj(pBox[20+2*i+1],20.0f);
        B3L_AddObjToRenderList((B3LObj_t *)(pBox[20+2*i+1]), &B3Lrender);
        pBox[20+2*i+1]->transform.translation={x,-60.0f,z};


    }
    //init the camera
    
    B3Lrender.camera.transform.translation = {0.0f,0.0f,-220.0f};
    B3L_CameraLookAt(&(B3Lrender.camera), &at);

    
    B3L_TweenStart(&tCatZ,0);

}

void update(uint32_t time){
    
    
    B3L_Update(&B3Lrender,time);
    B3L_TimerTweenUpdate(&ttCtl,time);
    u32 i;
    for (i=0;i<20;i++){
        pBox[2*i]->transform.rotation.x = tCatZ.value;
        pBox[2*i]->transform.rotation.y = tCatZ.value;
        pBox[2*i+1]->transform.rotation.x = -tCatZ.value;
        pBox[2*i+1]->transform.rotation.y = -tCatZ.value;
    }

    if (pressed(DPAD_UP)){
        B3Lrender.camera.transform.translation.y +=1.01f;

    }
    if (pressed(DPAD_DOWN)){
        B3Lrender.camera.transform.translation.y -=1.01f; 
         
    }
    if (pressed(DPAD_LEFT)){
        B3Lrender.camera.transform.translation.x += 1.0f;    
    }
    if (pressed(DPAD_RIGHT)){
        B3Lrender.camera.transform.translation.x -= 1.0f;    
    }
    if (pressed(A)){

    }
    if (pressed(B)){

    }


    //B3L_SetLightVect(&B3Lrender,200.0f*tCatZ.value,200.0f*tCatZ.value,200.0f*tCatZ.value);    


    B3L_CameraLookAt(&(B3Lrender.camera), &at);
    //vect3_t up ={2.0f,3.0f,0.0f};
    //B3L_SetCameraUpDirection(&(B3Lrender.camera),&up);

}


void render(uint32_t time) {
    
    B3L_NewRenderStart( &B3Lrender,0xFF000000);
    B3L_RenderScence(&B3Lrender);
    B3L_AppliedLightFromAlpha(&B3Lrender);


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

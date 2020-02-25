#include "32blit.hpp"

extern "C" {
#include "b3dlib.h"
#include "B3L_support.h"
#include "cat.h"
}
using namespace blit;

B3LPolygonObj_t *pBox2;
B3LMeshObj_t *pBox;
render_t B3Lrender;
B3LMeshObj_t box;
vect3_t at ;
f32 i = 0.0f;
f32 lightY = 0.0f;
/* setup */
B3LParticleGenObj_t  *pParticleGen;
fBuff_t *renderBuff;
B3L_timeTweenCtl_t ttCtl;
B3L_tween_t tCatZ;
void init() {
    B3L_TimerTweenCtlInit(&ttCtl);
    B3L_TweenInit(&ttCtl,&tCatZ,B3L_tween_ease_in_out_quad,0.0f, -100.0f, 2000, -1);
    at.x=0.0f;at.y=0.0f;at.z=0.0f;
    B3L_SetSeed(0x31415926);
    blit::set_screen_mode(ScreenMode::lores);
    //you need a 32bit framebuff here!
    fBuff_t *renderBuff=B3L_3dRenderAreaShiftCal(((fBuff_t *)screen.data),0, 0);   
    B3L_RenderInit(&B3Lrender,renderBuff);
    
    pParticleGen = B3L_GetFreeParticleGeneratorObj(&B3Lrender);
    B3L_InitDemoParticleGenObj(pParticleGen);
    pParticleGen->translation.x = 100.0f;
    pParticleGen->translation.y = 50.0f;
    B3L_AddObjToRenderList((B3LObj_t *)pParticleGen, &B3Lrender);

    pBox2 = (B3LPolygonObj_t *)B3L_GetFreeObj(&B3Lrender);
    pBox = (B3LMeshObj_t *)B3L_GetFreeObj(&B3Lrender);
    B3L_InitBoxObj(&box,50.0f);
    //B3L_InitBoxObjNoTexture(pBox2,50.0f); //create a size 10 box   
    B3L_InitBoxObjPolygon(pBox2,50.0f);
    InitCatObj(pBox,2.0f); //create a size 10 box  
    B3L_AddObjToRenderList((B3LObj_t *)&box, &B3Lrender);
    B3L_AddObjToRenderList((B3LObj_t *)pBox, &B3Lrender);
    B3L_AddObjToRenderList((B3LObj_t *)pBox2, &B3Lrender);
    B3L_SetLightType(&B3Lrender,dotLight);
    B3Lrender.light.factor_0=0.2f;
    box.transform.translation.y = 0.0f;
    box.transform.translation.x = 50.0f;
    box.transform.translation.z = 0.0f;
    pBox2->transform.translation.y = 0.0f;
    pBox2->transform.translation.x = -50.0f;
    pBox2->transform.translation.z = 0.0f;
    pBox->transform.translation.y = 0.0f;
    pBox->transform.translation.x = 100.0f;
    pBox->transform.translation.z = -50.0f;
    B3Lrender.camera.transform.translation.x = 0.0f;
    B3Lrender.camera.transform.translation.y =  70.0f;
    B3Lrender.camera.transform.translation.z =  -200.0f;
    //B3Lrender.camera.transform.rotation.x = -0.125f;
    //B3Lrender.camera.transform.rotation.y = 0.25f;
    //B3Lrender.camera.transform.rotation.z = 0.0f;
    B3L_CameraLookAt(&(B3Lrender.camera), &at);
    //printf("init done\n");
    B3L_TweenStart(&tCatZ,0);

}

void update(uint32_t time){
    
    
    B3L_Update(&B3Lrender,time);
    B3L_TimerTweenUpdate(&ttCtl,time);
    pBox->transform.translation.z = tCatZ.value;
    //box.transform.rotation.x += 0.001f;
    //box.transform.rotation.y += 0.001f;
    //pBox->transform.rotation.x -= 0.001f;
    //pBox->transform.rotation.z -= 0.001f;
    //pBox2->transform.rotation.z -= 0.001f;
    //pBox2->transform.rotation.y -= 0.001f;
    if (pressed(DPAD_UP)){
        //B3Lrender.camera.transform.translation.z +=1.01f;
        lightY += 1.0f;
    }
    if (pressed(DPAD_DOWN)){
        //B3Lrender.camera.transform.translation.z -=1.01f; 
         lightY -= 1.0f; 
    }
    if (pressed(DPAD_LEFT)){
        B3Lrender.camera.transform.translation.x += 1.0f;    
    }
    if (pressed(DPAD_RIGHT)){
        B3Lrender.camera.transform.translation.x -= 1.0f;    
    }
    if (pressed(A)){
        pBox->transform.rotation.y +=0.01f;
    }
    if (pressed(B)){
        pBox->transform.rotation.y -=0.01f;
    }
    if (i>1.0f){
        i=0.0f;
    }
    i +=0.002f;

    B3L_SetLightVect(&B3Lrender, 0.0f,lightY,0.0f);    
    B3Lrender.camera.transform.translation.z =  B3L_sin(i)*200.0f;
    
    B3Lrender.camera.transform.translation.x =  B3L_cos(i)*200.0f;

    B3L_CameraLookAt(&(B3Lrender.camera), &at);
    //vect3_t up ={2.0f,3.0f,0.0f};
    //B3L_SetCameraUpDirection(&(B3Lrender.camera),&up);

}


void render(uint32_t time) {
    
    B3L_NewRenderStart( &B3Lrender,0xFF003423);
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

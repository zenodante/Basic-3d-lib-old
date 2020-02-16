#include "game.hpp"

extern "C" {
#include "b3dlib.h"
#include "cat.h"
}
using namespace blit;

B3LPolygonObj_t *pBox2;
B3LMeshObj_t *pBox;
render_t B3Lrender;
B3LMeshObj_t box;
vect3_t at ={.x=0.0f,.y=0.0f,.z=0.0f};
f32 i = 0.0f;
f32 lightY = 0.0f;
/* setup */
void init() {
    set_screen_mode(screen_mode::lores);
    //you need a 32bit framebuff here!
    B3L_RenderInit(&B3Lrender,(frameBuffData_t *)fb.data);

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


}

void update(uint32_t time){
    
    
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
    //B3Lrender.camera.transform.translation.z =  B3L_sin(i)*200.0f;
    
    //B3Lrender.camera.transform.translation.x =  B3L_cos(i)*200.0f;

    //B3L_CameraLookAt(&(B3Lrender.camera), &at);
}

#define RGB_BLEND(sr, sg, sb, dr, dg, db, a) \
    uint8_t r = (sr * a) >> 8; \
    uint8_t g = (sg * a) >> 8; \
    uint8_t b = (sb * a) >> 8; \
    uint16_t ia = 256 - a; \
    dr = (r + ((dr * ia) >> 8)); \
    dg = (g + ((dg * ia) >> 8)); \
    db = (b + ((db * ia) >> 8)); \

void render(uint32_t time) {
    B3L_NewFrame( &B3Lrender);
    B3L_RenderScence(&B3Lrender,time);
/*
B3L_DrawTriColor(
                                                                        0.0f,0.0f,0.1f,
                                                                        12.0f,12.0f,0.1f,
                                                                        0.0f,24.0f,0.1f,
                                                                        1,0xff,0XFF7E2553,
                                                                        (frameBuffData_t *)fb.data,B3Lrender.pZBuff);

*/
    //printf("result is %d\n",result);
    uint8_t *buff = (uint8_t *)fb.data;
    uint32_t *u4buff = (uint32_t *)fb.data;
    uint32_t backColor = B3Lrender.light.color;
    uint8_t r,g,b;
    uint32_t i;
    uint32_t color;
/*    
    for (i=0;i<BUFF_LENTH;i++){
    color = u4buff[i];
    color = color>>24;
    
    color = (color<<24)|(color<<16)|(color<<8)|0xff;

    u4buff[i] = color;
    }
  
*/ 
//on h750 you could use DMA2D mode DMA2D_CR/mode = 0b101 to do this transform
    for (i=0;i<BUFF_LENTH;i++){
        r =  (backColor&(0x00FF0000))>>16;
        g =  (backColor &(0x0000FF00))>>8;
        b = (backColor &(0x000000FF));
        RGB_BLEND(buff[i*4+2],buff[i*4+1],buff[i*4],r,g,b,buff[i*4+3])
        u4buff[i]=(r<<24)|(g<<16)|(b<<8);
    }
    
    fb.watermark();
     
}

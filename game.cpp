#include "game.hpp"

extern "C" {
#include "b3dlib.h"
}
using namespace blit;

B3LMeshObj_t *pBox;
B3LMeshObj_t *pBox2;
render_t B3Lrender;
B3LMeshObj_t box;
vect3_t at ={.x=0.0f,.y=0.0f,.z=0.0f};
f32_t i = 0.0f;
/* setup */
void init() {
    set_screen_mode(screen_mode::lores);
    B3L_RenderInit(&B3Lrender,(frameBuffData_t *)fb.data);

    pBox = (B3LMeshObj_t *)B3L_GetFreeObj(&B3Lrender);
    pBox2 = (B3LMeshObj_t *)B3L_GetFreeObj(&B3Lrender);
    B3L_InitBoxObj(&box,50.0f);
    B3L_InitBoxObj(pBox,30.0f); //create a size 10 box   
    B3L_InitBoxObj(pBox2,50.0f); //create a size 10 box  
    B3L_AddObjToRenderList((B3LObj_t *)&box, &B3Lrender);
    B3L_AddObjToRenderList((B3LObj_t *)pBox, &B3Lrender);
    B3L_AddObjToRenderList((B3LObj_t *)pBox2, &B3Lrender);

    box.transform.translation.y = 70.0f;
    box.transform.translation.x = 50.0f;
    box.transform.translation.z = -50.0f;
    box.transform.rotation.x = 0.13f;
    box.transform.rotation.y = 0.13f;
    pBox2->transform.translation.y = 80.0f;
    pBox2->transform.translation.x = 10.0f;
    pBox2->transform.translation.z = -70.0f;

    B3Lrender.camera.transform.translation.x =  50.0f;
    B3Lrender.camera.transform.translation.y =  70.f;
    B3Lrender.camera.transform.translation.z =  -200.0f;

}

void update(uint32_t time){
    B3L_NewFrame( &B3Lrender);
    box.transform.rotation.x += 0.003f;
    box.transform.rotation.y += 0.003f;
    pBox->transform.rotation.x -= 0.003f;
    pBox->transform.rotation.y -= 0.003f;
    pBox2->transform.rotation.z -= 0.003f;
    pBox2->transform.rotation.y -= 0.003f;

    
    if (i>1.0f){
        i=0.0f;
    }
    i +=0.002f;


    //B3Lrender.camera.transform.translation.z =  B3L_sin(i)*200.0f;
    
    //B3Lrender.camera.transform.translation.x =  B3L_cos(i)*200.0f;
    //printf("z:%.3f,x:%.3f\n",B3L_sin(i)*200.0f, B3L_cos(i)*200.0f);
    //B3L_CameraLookAt(&(B3Lrender.camera), &at);
}


void render(uint32_t time) {
    B3L_RenderScence(&B3Lrender);
    //printf("result is %d\n",result);
   
    fb.watermark();
    
}

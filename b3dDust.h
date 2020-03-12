#ifndef  __B3D_DUST_H__
#define  __B3D_DUST_H__

#include "b3dlib.h"

#define LIFE_SHIFT              18
#define DUSTNUM                 128
#define DUSTRANGE               300.0f
#define DUSTRANGE_DOUBLE        600.0f
#define DUSTRESETDISTANCE_SQ    160000.0f
#define INV65535                1.52590218967e-5f

typedef struct{
    vect3_t    position;
    s32        life;
}dust_t;



void DustInit(void);
void DustUpdateAndRender(render_t *pRender,B3LObj_t *pObj,u32 time);
#endif
#ifndef __ship2_72tri_H__
#define __ship2_72tri_H__
#include "b3dlib.h"
extern const float ship2_72tri_vect[123];
extern const float ship2_72tri_normal[234];
extern const uint16_t ship2_72tri_triIdx[234];
extern const uint8_t ship2_72tri_uv[468];
extern const B3L_Mesh_t ship2_72tri_mesh;
extern const float ship2_72tribound[24];
extern const uint32_t ship2_lut[16];
extern const uint16_t ship2_lut4444[16];
extern const uint8_t ship2_uvmap[8192];
extern const B3L_texture_t ship_texture;
extern void InitShipObj(B3LMeshObj_t *pObj,f32 size);
#endif 

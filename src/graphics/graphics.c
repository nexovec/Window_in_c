#include "graphics/graphics.h"
#include "graphics/primitives.h"
#include "utils/utils.h"
#include "glad/glad.h"
#include "assert.h"
#include "stdlib.h"
#include "stdio.h"

VBLayout *VBLayout_init(VBLayout *vbl)
{
    vbl->attrCount = 0;
    vbl->stride = 0;
    vbl->offsets[0] = 0;
    return vbl;
}
// FIXME: only works for float
VBLayout *VBLayout_addAttr(VBLayout *vbl, uint32_t attrName, uint32_t compCount, uint32_t type)
{
    // TODO: guards for sizes in debug
    uint32_t i = vbl->attrCount++;
    vbl->attrNames[i] = attrName;
    vbl->compCounts[i] = compCount;
    vbl->types[i] = type;
    if (i)
        vbl->offsets[i] = vbl->offsets[i - 1] + vbl->compCounts[i - 1] * sizeof(float);
    if (i == 1)
        assert(vbl->offsets[i] == 12);
    vbl->stride += compCount * sizeof(float);
    return vbl;
}
VBO *VBO_init(VBO *vbo, VBLayout *layout)
{
    vbo->layout = layout;
    return vbo;
}
// FIXME: depends on VA state creep and pre-enabled attribute names
VBO *VBO_uploadBuffer(VBO *vbo, size_t vCount)
{
    glad_glGenBuffers(1, &vbo->id);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    glad_glBufferData(GL_ARRAY_BUFFER, vCount * vbo->layout->stride, vbo->data, GL_DYNAMIC_DRAW);
    for (uint32_t i = 0; i < vbo->layout->attrCount; i++)
    {
        glad_glEnableVertexAttribArray(vbo->layout->attrNames[i]);
        glad_glVertexAttribPointer(vbo->layout->attrNames[i], vbo->layout->compCounts[i], vbo->layout->types[i], GL_FALSE, vbo->layout->stride, (const void *)vbo->layout->offsets[i]);
    }
    return vbo;
}

MeshArray *MeshArray_initMeshArray(MeshArray *ma, VBO *vbo, size_t maxMeshes)
{
    ma->vbo = vbo;
    ma->meshCount = 0;
    ma->maxMeshes = maxMeshes;
    //FIXME: leaks
    ma->meshes = calloc(sizeof(Mesh), ma->maxMeshes);
    return ma;
}

MeshArray *MeshArray_registerMesh(MeshArray *ma, Mesh *mesh)
{
    if (ma->maxMeshes == ma->meshCount - 1)
    {
        printf("MeshArray is full");
        return ma;
    }
    ma->meshes[ma->meshCount++] = mesh;
    return ma;
}
MeshArray *MeshArray_packVBO(MeshArray *ma)
{
    // TODO: lol, not like this... need to make multiple meshes supported
    ma->vbo->data = concatFloatArrays(ma->meshes[0]->pointer, 6 * ma->meshes[0]->vCount, ma->meshes[1]->pointer, 6 * ma->meshes[1]->vCount);
    ma->vbo->vCount = getMeshArrayVCount(ma);
    // printf("\n%d | %d | %d\n", ma->meshes[0]->vCount, ma->meshes[1]->vCount, ma->vCount);
    VBO_uploadBuffer(ma->vbo, ma->vbo->vCount);
    return ma;
}

//temporary
MeshArray *makeBasicMeshArray(uint32_t pos_loc, uint32_t color_loc)
{
    uint32_t vao;
    glad_glGenVertexArrays(1, &vao);
    glad_glBindVertexArray(vao);
    // FIXME: leaks
    VBLayout *vbl = malloc(sizeof(VBLayout));
    VBLayout_init(vbl);
    VBLayout_addAttr(vbl, pos_loc, 3, GL_FLOAT);
    VBLayout_addAttr(vbl, color_loc, 3, GL_FLOAT);
    VBO *vbo;
    // FIXME: leaks
    vbo = malloc(sizeof(VBO));
    VBO_init(vbo, vbl);
    MeshArray *ma = malloc(sizeof(MeshArray));
    MeshArray_initMeshArray(ma, vbo, 1000);

    size_t meshCount = 2;
    // NOTE: copying data
    Mesh **meshes = malloc(meshCount * sizeof(Mesh *));
    meshes[0] = makeSimpleQuadMesh();
    meshes[1] = makeQuadMesh(&(Vec3f){420, 100, 0}, &(Vec3f){100, 100, 0}, COLOR_WHITE);
    for (int i = 0; i < meshCount; i++)
        MeshArray_registerMesh(ma, meshes[i]);
    MeshArray_packVBO(ma);
    return ma;
}
uint32_t getMeshArrayVCount(MeshArray *arr)
{
    uint32_t res = 0;
    for (int i = 0; i < arr->meshCount; i++)
    {
        if (arr->meshes[i] == 0)
            break;
        res += arr->meshes[i]->vCount;
    }
    return res;
}
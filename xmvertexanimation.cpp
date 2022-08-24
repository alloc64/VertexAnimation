#include "main.h"

//#define _info

extern xmTimer timer;
extern xmTexture texture;
extern xmLevelShaders xmlLighting;

xmVertexAnimation::xmVertexAnimation() {
    iCurrFrame = 0;
    iFirstFrame = 0;
    dt = 0.0;
}

xmVertexAnimation::~xmVertexAnimation() {
    for (int o = 0; o < iNumObjects; o++) {

        if (object[o].face) delete[] object[o].face;
        if (object[o].texcoord) delete[] object[o].texcoord;
        if (object[o].animVertices) delete[] object[o].animVertices;

        for (int f = 0; f < iLastFrame; f++) {
            if (object[o].frame[f].vertex) delete[] object[o].frame[f].vertex;

            if (object[o].frame[f].uNormalVBO) glDeleteBuffersARB(1, &object[o].frame[f].uNormalVBO);
            if (object[o].frame[f].uTangentVBO) glDeleteBuffersARB(1, &object[o].frame[f].uTangentVBO);
        }

        if (object[o].uVertexVBO) glDeleteBuffersARB(1, &object[o].uVertexVBO);
        if (object[o].uTexCoordVBO) glDeleteBuffersARB(1, &object[o].uTexCoordVBO);
        if (object[o].uFaceVBO) glDeleteBuffersARB(1, &object[o].uFaceVBO);

        if (object[o].frame) delete[] object[o].frame;

        if (object[o].uDiffuseMap > 0) glDeleteTextures(1, &object[o].uDiffuseMap);
        if (object[o].uNormalMap > 0) glDeleteTextures(1, &object[o].uNormalMap);
        if (object[o].uSpecularMap > 0) glDeleteTextures(1, &object[o].uSpecularMap);
    }

    if (object) delete[] object;
}

bool xmVertexAnimation::Load(char *path) {
    float startTime = GetTickCount();

    FILE *pFile = fopen(path, "rb");
    if (!pFile) {
        printf("error: could not open file %s!", path);
        return false;
    }

    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);

    if (strcmp(header, "LXMA24")) {
        printf("error: file version %s is not valid ::xmVertexAnimation file!\n", path);
        return false;
    }

    fread(&iNumObjects, 1, sizeof(short int), pFile);
    fread(&iLastFrame, 1, sizeof(short int), pFile);
    fread(&iFramerate, 1, sizeof(short int), pFile);

#ifdef _info
    printf("%d %d\n", iNumObjects, iLastFrame);
#endif

    if (!iNumObjects) return false;
    object = new xmaPerVertexObject[iNumObjects];
    if (!object) return false;

    for (int o = 0; o < iNumObjects; o++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, size, pFile);

        fread(&object[o].iNumVertices, 1, sizeof(short int), pFile);
        fread(&object[o].iNumFaces, 1, sizeof(short int), pFile);

        if (object[o].iNumVertices <= 0 || object[o].iNumFaces <= 0) {
            printf("warning: empty object %d\n", o);
            iNumObjects--;
            continue;
        }

#ifdef _info
        printf("%s %d %d %d\n", object[o].name, size, object[o].iNumVertices, object[o].iNumFaces);
#endif

        object[o].face = new xmFace[object[o].iNumFaces];
        object[o].frame = new xmaFrame[iLastFrame];
        object[o].texcoord = new vec2[object[o].iNumVertices];
        object[o].animVertices = new vec3[object[o].iNumVertices];

        fread(object[o].texcoord, object[o].iNumVertices, sizeof(vec2), pFile);
        fread(object[o].face, object[o].iNumFaces, sizeof(xmFace), pFile);
        //for(int i=0; i < object[o].iNumFaces; i++)
        //{
        //    printf("%d %d %d\n", object[o].face[i].a, object[o].face[i].b, object[o].face[i].c);    
        //}

        min = max = vec3(0, 0, 0);

        for (int f = 0; f < iLastFrame; f++) {
            fread(&object[o].frame[f].center, 1, sizeof(vec3), pFile);
            object[o].frame[f].vertex = new vec3[object[o].iNumVertices];
            object[o].frame[f].normal = new vec3[object[o].iNumVertices];
            object[o].frame[f].tangent = new vec3[object[o].iNumVertices];

            fread(object[o].frame[f].vertex, object[o].iNumVertices, sizeof(vec3), pFile);

            //for(int i=0; i < object[o].iNumVertices; i++)
            //{
            //    printf("%f %f %f\n", object[o].frame[f].vertex[i].x, 
            //                         object[o].frame[f].vertex[i].y,
            //                         object[o].frame[f].vertex[i].z);    
            //}

            for (int i = 0; i < object[o].iNumFaces; i++) {
                int ind[3];
                ind[0] = object[o].face[i].a;
                ind[1] = object[o].face[i].b;
                ind[2] = object[o].face[i].c;

                for (int v = 0; v < 3; v++) {
                    if (object[o].frame[f].vertex[ind[v]].x < min.x) min.x = object[o].frame[f].vertex[ind[v]].x - 2.0f;
                    if (object[o].frame[f].vertex[ind[v]].y < min.y) min.y = object[o].frame[f].vertex[ind[v]].y - 2.0f;
                    if (object[o].frame[f].vertex[ind[v]].z < min.z) min.z = object[o].frame[f].vertex[ind[v]].z - 2.0f;

                    if (object[o].frame[f].vertex[ind[v]].x > max.x) max.x = object[o].frame[f].vertex[ind[v]].x + 2.0f;
                    if (object[o].frame[f].vertex[ind[v]].y > max.y) max.y = object[o].frame[f].vertex[ind[v]].y + 2.0f;
                    if (object[o].frame[f].vertex[ind[v]].z > max.z) max.z = object[o].frame[f].vertex[ind[v]].z + 2.0f;
                }

                vec3 Edge1 =
                        object[o].frame[f].vertex[object[o].face[i].b] - object[o].frame[f].vertex[object[o].face[i].a];
                vec3 Edge2 =
                        object[o].frame[f].vertex[object[o].face[i].c] - object[o].frame[f].vertex[object[o].face[i].a];
                vec2 Edge1uvs = object[o].texcoord[object[o].face[i].b] - object[o].texcoord[object[o].face[i].a];
                vec2 Edge2uvs = object[o].texcoord[object[o].face[i].c] - object[o].texcoord[object[o].face[i].a];

                float cp = Edge1uvs.y * Edge2uvs.x - Edge1uvs.x * Edge2uvs.y;
                vec3 normal = Normalize(Cross(Edge1, Edge2));
                vec3 tangent = vec3(0);
                (fabs(cp) < 1e-6f) ? tangent.x = 1.0f : (cp != 0.0f) ? tangent = Normalize(
                        (Edge1 * -Edge2uvs.y + Edge2 * Edge1uvs.y) * (1.0f / cp)) : tangent = vec3(0);

                object[o].frame[f].normal[object[o].face[i].a] = normal;
                object[o].frame[f].normal[object[o].face[i].b] = normal;
                object[o].frame[f].normal[object[o].face[i].c] = normal;

                object[o].frame[f].tangent[object[o].face[i].a] = tangent;
                object[o].frame[f].tangent[object[o].face[i].b] = tangent;
                object[o].frame[f].tangent[object[o].face[i].c] = tangent;

            }

            glGenBuffersARB(1, &object[o].frame[f].uNormalVBO);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].frame[f].uNormalVBO);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].frame[f].normal,
                            GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glGenBuffersARB(1, &object[o].frame[f].uTangentVBO);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].frame[f].uTangentVBO);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].frame[f].tangent,
                            GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            if (object[o].frame[f].normal) delete[] object[o].frame[f].normal;
            if (object[o].frame[f].tangent) delete[] object[o].frame[f].tangent;
        }

        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, size, pFile);
        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);
        //printf("%d %s %d\n", size, object[o].texturePath, object[o].textureFilter);

        object[o].uDiffuseMap = texture.xmLoadTexture((char *) object[o].texturePath, o, (int) object[o].textureFilter);

        char *name = strtok(object[o].texturePath, ".");
        if (name) {
            char path[1024];
            sprintf(path, "%s_normal.dds", name);

            object[o].uNormalMap = texture.xmLoadTexture((char *) path, o, (int) object[o].textureFilter);

            sprintf(path, "%s_specular.dds", name);
            object[o].uSpecularMap = texture.xmLoadTexture((char *) path, o, (int) object[o].textureFilter);
        }

        Animate(o);

        glGenBuffersARB(1, &object[o].uVertexVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].animVertices,
                        GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uTexCoordVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 2 * sizeof(float), object[o].texcoord,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uFaceVBO);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].iNumFaces * 3 * sizeof(unsigned int), object[o].face,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    for (int o = 0; o < iNumObjects; o++) {
        for (int i = 0; i < object[o].iNumVertices; i++) {
            int frame = 0;
            if (object[o].frame[frame].vertex[i].x < min.x) min.x = object[o].frame[frame].vertex[i].x - 2.0f;
            if (object[o].frame[frame].vertex[i].y < min.y) min.y = object[o].frame[frame].vertex[i].y - 2.0f;
            if (object[o].frame[frame].vertex[i].z < min.z) min.z = object[o].frame[frame].vertex[i].z - 2.0f;
            if (object[o].frame[frame].vertex[i].x > max.x) max.x = object[o].frame[frame].vertex[i].x + 2.0f;
            if (object[o].frame[frame].vertex[i].y > max.y) max.y = object[o].frame[frame].vertex[i].y + 2.0f;
            if (object[o].frame[frame].vertex[i].z > max.z) max.z = object[o].frame[frame].vertex[i].z + 2.0f;
        }
        object[o].min = min;
        object[o].max = max;
        object[o].radius = Distance(max, (max + min) / 2);
    }
    center = ((max + min) / 2);
    radius = Distance(max, center);

    BoxVertices[0] = vec3(min.x, min.y, min.z);
    BoxVertices[1] = vec3(min.x, max.y, min.z);
    BoxVertices[2] = vec3(max.x, max.y, min.z);

    BoxVertices[3] = vec3(min.x, min.y, min.z);
    BoxVertices[4] = vec3(max.x, max.y, min.z);
    BoxVertices[5] = vec3(max.x, min.y, min.z);

    BoxVertices[6] = vec3(min.x, min.y, max.z);
    BoxVertices[7] = vec3(max.x, max.y, max.z);
    BoxVertices[8] = vec3(min.x, max.y, max.z);

    BoxVertices[9] = vec3(min.x, min.y, max.z);
    BoxVertices[10] = vec3(max.x, min.y, max.z);
    BoxVertices[11] = vec3(max.x, max.y, max.z);

    BoxVertices[12] = vec3(min.x, min.y, min.z);
    BoxVertices[13] = vec3(max.x, min.y, max.z);
    BoxVertices[14] = vec3(min.x, min.y, max.z);

    BoxVertices[15] = vec3(min.x, min.y, min.z);
    BoxVertices[16] = vec3(max.x, min.y, min.z);
    BoxVertices[17] = vec3(max.x, min.y, max.z);

    BoxVertices[18] = vec3(min.x, max.y, min.z);
    BoxVertices[19] = vec3(min.x, max.y, max.z);
    BoxVertices[20] = vec3(max.x, max.y, max.z);

    BoxVertices[21] = vec3(min.x, max.y, min.z);
    BoxVertices[22] = vec3(max.x, max.y, max.z);
    BoxVertices[23] = vec3(max.x, max.y, min.z);

    BoxVertices[24] = vec3(min.x, min.y, min.z);
    BoxVertices[25] = vec3(min.x, min.y, max.z);
    BoxVertices[26] = vec3(min.x, max.y, max.z);

    BoxVertices[27] = vec3(min.x, min.y, min.z);
    BoxVertices[28] = vec3(min.x, max.y, max.z);
    BoxVertices[29] = vec3(min.x, max.y, min.z);

    BoxVertices[30] = vec3(max.x, min.y, min.z);
    BoxVertices[31] = vec3(max.x, max.y, max.z);
    BoxVertices[32] = vec3(max.x, min.y, max.z);

    BoxVertices[33] = vec3(max.x, min.y, min.z);
    BoxVertices[34] = vec3(max.x, max.y, min.z);
    BoxVertices[35] = vec3(max.x, max.y, max.z);

    fclose(pFile);
    printf("ok: ended, xmVertexAnimation:: load time %.1f ms!\n", GetTickCount() - startTime);
    return true;
}


bool xmVertexAnimation::SetDefaultSequence(bool animate) {
    if (animate) {
        if (iCurrFrame >= iLastFrame - 2) iCurrFrame = iFirstFrame < 1 ? iFirstFrame + 1 : iFirstFrame;
        bAnim = true;
    }

    if (bAnim) {
        dt += timer.fFrameInterval * iFramerate;
        if (dt > 1.0f) {
            dt = 0.0f;
            iCurrFrame++;
            if (iCurrFrame >= iLastFrame - 2) bAnim = false;
        }
    }

    return bAnim;
}


bool xmVertexAnimation::SetSequence(short int start, short int end, bool animate) {
    if (animate) {
        if (iCurrFrame >= end) iCurrFrame = start < 1 ? start + 1 : start;
        bAnim = true;
    }

    if (bAnim) {
        dt += timer.fFrameInterval * iFramerate;
        if (dt > 1.0f) {
            dt = 0.0f;
            iCurrFrame++;
            if (iCurrFrame >= end) bAnim = false;
        }
    }

    return bAnim;
}


void xmVertexAnimation::Animate(int o) {
    //if(iCurrFrame < 1) return;

    for (int i = 0; i < object[o].iNumVertices; i++) {
        object[o].animVertices[i] = (object[o].frame[iCurrFrame].vertex[i] + dt * (object[o].frame[iCurrFrame +
                                                                                                   1].vertex[i] -
                                                                                   object[o].frame[iCurrFrame].vertex[i]));
    }
    //glPushMatrix();
    //glTranslatef(object[o].frame[iCurrFrame].center.x,object[o].frame[iCurrFrame].center.y,object[o].frame[iCurrFrame].center.z);
    //glutWireSphere(10, 12, 12);
    //glPopMatrix();
}


void xmVertexAnimation::Draw() {
    if (!object) return;
    for (int o = 0; o < iNumObjects; o++) {
        //drawbox(object[o].min, object[o].max);  
        xmlLighting.BindTextures(object[o].uDiffuseMap, object[o].uNormalMap, object[o].uSpecularMap);

        glEnableVertexAttribArrayARB(xmlLighting.iUniformTangent);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].frame[iCurrFrame].uTangentVBO);
        glVertexAttribPointerARB(xmlLighting.iUniformTangent, 3, GL_FLOAT, 0, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        Animate(o);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        float *ptr = (float *) glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
        if (ptr) {
            memcpy(ptr, (float *) object[o].animVertices, object[o].iNumVertices * sizeof(vec3));
            glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        }
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].frame[iCurrFrame].uNormalVBO);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glDrawElements(GL_TRIANGLES, object[o].iNumFaces * 3, GL_UNSIGNED_INT, NULL);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

        glDisableVertexAttribArrayARB(xmlLighting.iUniformTangent);

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

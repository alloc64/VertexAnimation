/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "mat4.h"
#include "vector.h"

#define degtorad (180/3.14159265358979323846)

mat4 mat4::operator*(const mat4 &m2) {
    mat4 mt;
    float *m1 = mat;
    mt(0, m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12]);
    mt(1, m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13]);
    mt(2, m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14]);
    mt(3, m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15]);

    mt(4, m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12]);
    mt(5, m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13]);
    mt(6, m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14]);
    mt(7, m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15]);

    mt(8, m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12]);
    mt(9, m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13]);
    mt(10, m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14]);
    mt(11, m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15]);

    mt(12, m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12]);
    mt(13, m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13]);
    mt(14, m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14]);
    mt(15, m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15]);

    return mt;
}

void mat4::Inverse() {
    float minor[16];
    float *m = mat;

    minor[0] = m[5] * m[10] * m[15] + m[6] * m[11] * m[13] + m[7] * m[9] * m[14] - m[7] * m[10] * m[13] -
               m[6] * m[9] * m[15] - m[5] * m[11] * m[14];
    minor[4] = -(m[4] * m[10] * m[15] + m[6] * m[11] * m[12] + m[7] * m[8] * m[14] - m[7] * m[10] * m[12] -
                 m[6] * m[8] * m[15] - m[4] * m[11] * m[14]);
    minor[8] = m[4] * m[9] * m[15] + m[5] * m[11] * m[12] + m[7] * m[8] * m[13] - m[7] * m[9] * m[12] -
               m[5] * m[8] * m[15] - m[4] * m[11] * m[13];
    minor[12] = -(m[4] * m[9] * m[14] + m[5] * m[10] * m[12] + m[6] * m[8] * m[13] - m[6] * m[9] * m[12] -
                  m[5] * m[8] * m[14] - m[4] * m[10] * m[13]);

    minor[1] = -(m[1] * m[10] * m[15] + m[2] * m[11] * m[13] + m[3] * m[9] * m[14] - m[3] * m[10] * m[13] -
                 m[2] * m[9] * m[15] - m[1] * m[11] * m[14]);
    minor[5] = m[0] * m[10] * m[15] + m[2] * m[11] * m[12] + m[3] * m[8] * m[14] - m[3] * m[10] * m[12] -
               m[2] * m[8] * m[15] - m[0] * m[11] * m[14];
    minor[9] = -(m[0] * m[9] * m[15] + m[1] * m[11] * m[12] + m[3] * m[8] * m[13] - m[3] * m[9] * m[12] -
                 m[1] * m[8] * m[15] - m[0] * m[11] * m[13]);
    minor[13] = m[0] * m[9] * m[14] + m[1] * m[10] * m[12] + m[2] * m[8] * m[13] - m[2] * m[9] * m[12] -
                m[1] * m[8] * m[14] - m[0] * m[10] * m[13];

    minor[2] = m[1] * m[6] * m[15] + m[2] * m[7] * m[13] + m[3] * m[5] * m[14] - m[3] * m[6] * m[13] -
               m[2] * m[5] * m[15] - m[1] * m[7] * m[14];
    minor[6] = -(m[0] * m[6] * m[15] + m[2] * m[7] * m[12] + m[3] * m[4] * m[14] - m[3] * m[6] * m[12] -
                 m[2] * m[4] * m[15] - m[0] * m[7] * m[14]);
    minor[10] = m[0] * m[5] * m[15] + m[1] * m[7] * m[12] + m[3] * m[4] * m[13] - m[3] * m[5] * m[12] -
                m[1] * m[4] * m[15] - m[0] * m[7] * m[13];
    minor[14] = -(m[0] * m[5] * m[14] + m[1] * m[6] * m[12] + m[2] * m[4] * m[13] - m[2] * m[5] * m[12] -
                  m[1] * m[4] * m[14] - m[0] * m[6] * m[13]);

    minor[3] = -(m[1] * m[6] * m[11] + m[2] * m[7] * m[9] + m[3] * m[5] * m[10] - m[3] * m[6] * m[9] -
                 m[2] * m[5] * m[11] - m[1] * m[7] * m[10]);
    minor[7] =
            m[0] * m[6] * m[11] + m[2] * m[7] * m[8] + m[3] * m[4] * m[10] - m[3] * m[6] * m[8] - m[2] * m[4] * m[11] -
            m[0] * m[7] * m[10];
    minor[11] = -(m[0] * m[5] * m[11] + m[1] * m[7] * m[8] + m[3] * m[4] * m[9] - m[3] * m[5] * m[8] -
                  m[1] * m[4] * m[11] - m[0] * m[7] * m[9]);
    minor[15] =
            m[0] * m[5] * m[10] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[2] * m[5] * m[8] - m[1] * m[4] * m[10] -
            m[0] * m[6] * m[9];

    float det = Determinant();
    if (det) for (int i = 0; i < 16; i++) mat[i] = minor[i] / det;
}

float mat4::Determinant() {
    float a1 = mat[10] * mat[15] - mat[11] * mat[14];
    float a2 = mat[9] * mat[15] - mat[11] * mat[13];
    float a3 = mat[9] * mat[14] - mat[10] * mat[13];
    float a4 = mat[7] * mat[2] - mat[6] * mat[3];
    float a5 = mat[7] * mat[1] - mat[5] * mat[3];
    float a6 = mat[6] * mat[1] - mat[5] * mat[2];

    return mat[0] * (mat[5] * a1 - mat[6] * a2 + mat[7] * a3) -
           mat[4] * (mat[1] * a1 - mat[2] * a2 + mat[3] * a3) +
           mat[8] * (mat[13] * a4 - mat[14] * a5 + mat[15] * a6) -
           mat[12] * (mat[9] * a4 - mat[10] * a5 + mat[11] * a6);
}

vec3 mat4::operator*(vec3 vector) {
    vec3 b;
    b.x = vector.x * mat[0] + vector.y * mat[4] + vector.z * mat[8] + mat[12];
    b.y = vector.x * mat[1] + vector.y * mat[5] + vector.z * mat[9] + mat[13];
    b.z = vector.x * mat[2] + vector.y * mat[6] + vector.z * mat[10] + mat[14];
    return b;
}

void mat4::Rotate(float x, float y, float z) {
    glPushMatrix();
    glLoadMatrixf(mat);
    if (z)
        glRotatef(z * degtorad, 0, 0, 1);
    if (y)
        glRotatef(y * degtorad, 0, 1, 0);
    if (x)
        glRotatef(x * degtorad, 1, 0, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, mat);
    glPopMatrix();
}

void mat4::Rotate(vec3 rot) {
    glPushMatrix();
    glLoadMatrixf(mat);
    if (rot.z)
        glRotatef(rot.z * degtorad, 0, 0, 1);
    if (rot.y)
        glRotatef(rot.y * degtorad, 0, 1, 0);
    if (rot.x)
        glRotatef(rot.x * degtorad, 1, 0, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, mat);
    glPopMatrix();
}

vec3 TransformVec3ByMat4(vec3 vector, mat4 matrix) {
    vec3 v;

    v.x = vector.x * matrix[0] + vector.y * matrix[4] + vector.z * matrix[8];// + matrix[12];
    v.y = vector.x * matrix[1] + vector.y * matrix[5] + vector.z * matrix[9];// + matrix[13];
    v.z = vector.x * matrix[2] + vector.y * matrix[6] + vector.z * matrix[10];// + matrix[14];

    return v;
}

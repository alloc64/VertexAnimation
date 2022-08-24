/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "./textures.h"
#include "directx/ddraw.h"

//#define _texdebug

int xmTexture::xmLoadTexture(const char *xmFileName, GLuint xmTexIndex, GLuint xmFiltering) {
    GLuint xmTextureID = 0;
    xmTexData *pTexData = NULL;
    int xmMipMapOffset = 0;
    for (int i = 0; i < 3000; i++) if (strstr(usedTexture[i].name, xmFileName)) return usedTexture[i].usedIndex;

    if (strstr(xmFileName, ".jpg")) TEXTURE_TYPE = 0;
    else if (strstr(xmFileName, ".xmt"))
        TEXTURE_TYPE = 1;
    else if (strstr(xmFileName, ".dds")) TEXTURE_TYPE = 2; else TEXTURE_TYPE = -1;

    switch (xmFiltering) {
        case 0:
            xmMinFilter = GL_NEAREST;
            xmMaxFilter = GL_LINEAR;
#ifdef _texdebug
            printf("warning: setting texture filter GL_NEAREST;\n");
#endif
            break;

        case 1:
            xmMinFilter = GL_LINEAR;
            xmMaxFilter = GL_LINEAR_MIPMAP_NEAREST;
#ifdef _texdebug
            printf("warning: setting texture filter GL_LINEAR;\n");
#endif
            break;

        case 2:
            xmMinFilter = GL_LINEAR;
            xmMaxFilter = GL_LINEAR_MIPMAP_LINEAR;
#ifdef _texdebug
            printf("warning: setting texture filter GL_LINEAR_MIPMAP_LINEAR;\n");
#endif
            break;

        default:
            printf("error: bad filter type!\n");
            return -1;
            break;
    }

    switch (TEXTURE_TYPE) {
        case 0: //JPG texture
            pTexData = xmLoadJPG(xmFileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", xmFileName);
                return -1;
            }
            glGenTextures(1, &xmTextureID);
            glBindTexture(GL_TEXTURE_2D, xmTextureID);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, xmMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, xmMaxFilter);
#if 0
            glTexImage2D (GL_TEXTURE_2D, 0, pTexData->internalFormat, pTexData->width, pTexData->height, 0, pTexData->format, GL_UNSIGNED_BYTE, pTexData->texels);
#else
            printf("warning: creating mipmaps is very slow!(JPEG) \n");
            gluBuild2DMipmaps(GL_TEXTURE_2D, pTexData->internalFormat, pTexData->width, pTexData->height,
                              pTexData->format, GL_UNSIGNED_BYTE, pTexData->texels);
#endif

            break;

        case 1:    //XMT texture
            pTexData = xmLoadXMT(xmFileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", xmFileName);
                return -1;
            }
            glGenTextures(1, &xmTextureID);
            glBindTexture(GL_TEXTURE_2D, xmTextureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, xmMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, xmMaxFilter);
            glTexImage2D(GL_TEXTURE_2D, 0, pTexData->internalFormat, pTexData->width, pTexData->height, 0,
                         pTexData->format, GL_UNSIGNED_BYTE, &pTexData->texels);
            break;

        case 2:    //DDS texture
            pTexData = xmLoadDDS(xmFileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", xmFileName);
                return -1;
            }
            glGenTextures(1, &xmTextureID);
            glBindTexture(GL_TEXTURE_2D, xmTextureID);

            if (pTexData->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
                pTexData->mipFactor = 8;
            else pTexData->mipFactor = 16;
            for (int i = 0; i < pTexData->totalMipMaps; i++) {
                int xmMipMapSize = ((pTexData->width + 3) / 4) * ((pTexData->height + 3) / 4) * pTexData->mipFactor;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, xmMinFilter);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, xmMaxFilter);

                glCompressedTexImage2DARB(GL_TEXTURE_2D, i, pTexData->format, pTexData->width, pTexData->height, 0,
                                          xmMipMapSize, pTexData->texels + xmMipMapOffset);

                pTexData->width = pTexData->width >> 1;
                pTexData->height = pTexData->height >> 1;

                xmMipMapOffset += xmMipMapSize;
            }
            break;

        default:
            printf("error: texture isn't supported! only JPG,DDS,XMT are supported!\n");
            return -1;
            break;
    }

    if (pTexData) {
        if (pTexData->texels) delete[] pTexData->texels;
        delete[] pTexData;
    }

    strcpy(usedTexture[xmTexIndex].name, xmFileName);
    usedTexture[xmTexIndex].usedIndex = xmTextureID;
    printf("ok: texture %s loaded sucessfully!\n", xmFileName);

    return xmTextureID;
}

xmTexData *xmTexture::xmLoadJPG(const char *xmFileName) {
    xmTexData *pData = NULL;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW j;

    FILE *pFile = fopen(xmFileName, "rb");
    if (!pFile) {
        printf("error: couldn't open %s!\n", xmFileName);
        return NULL;
    }

    jpeg_create_decompress (&cinfo);
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_stdio_src(&cinfo, pFile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    pData = (xmTexData *) malloc(sizeof(xmTexData));

    pData->width = cinfo.image_width;
    pData->height = cinfo.image_height;
    pData->internalFormat = cinfo.num_components;

    if (cinfo.num_components == 1) {
        pData->format = GL_LUMINANCE;
    } else {
        pData->format = GL_RGB;
    }

    pData->texels = (GLubyte *) malloc(sizeof(GLubyte) * pData->width * pData->height * pData->internalFormat);

    for (int i = 0; i < pData->height; ++i) {
        j = &pData->texels[pData->width * i * pData->internalFormat];
        jpeg_read_scanlines(&cinfo, &j, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(pFile);
    return pData;


}

xmTexData *xmTexture::xmLoadXMT(const char *xmFileName) {

    int width = 0;
    int height = 0;
    int channels = 0;
    int size = 0;
    char header[3];

    FILE *xmTexFile = fopen(xmFileName, "rb");
    if (!xmTexFile) printf("error: file %s not found!\n", xmFileName);

    fread(&header, 3, 1, xmTexFile);
    if (header[0] != 'X' || header[1] != 'M' || header[2] != 'T') {
        printf("error: header isn't XMT! file %s has bad format!\n", xmFileName);
        return nullptr;
    }

    fread(&width, sizeof(int), 1, xmTexFile);
    fread(&height, sizeof(int), 1, xmTexFile);
    fread(&channels, sizeof(int), 1, xmTexFile);

    size = width * height * channels;
    xmTexData *pTexData = new xmTexData[size];

    if (!pTexData) {
        printf("error: cannot allocate memory for XMT texture!\n");
        return nullptr;
    }

    fread(&pTexData->texels, size, 1, xmTexFile);

    pTexData->width = width;
    pTexData->height = height;
    pTexData->internalFormat = channels;
    pTexData->format = GL_RGB;

    return pTexData;
}

xmTexData *xmTexture::xmLoadDDS(const char *xmFileName) {
    FILE *fp;
    xmTexData *pTexData = NULL;
    DDSURFACEDESC2 ddsd;
    char imageID[4];
    int totalSize = 0;
    int imageSize = 0;

    fp = fopen(xmFileName, "rb");
    if (!fp) {
        printf("error: couldn't open %s!\n", xmFileName);
        return NULL;
    }

    fread(imageID, 1, 4, fp);

    if (strncmp(imageID, "DDS ", 4) != 0) {
        fclose(fp);
        return NULL;
    }

    fread(&ddsd, sizeof(DDSURFACEDESC2), 1, fp);

    if (ddsd.dwLinearSize != 0) {
        imageSize = ddsd.dwLinearSize;
    } else {
        imageSize = ddsd.dwHeight * ddsd.dwWidth;
    }

    if (imageSize <= 0) {
        printf("error: %s is empty!\n", xmFileName);
        fclose(fp);
        return NULL;
    }

    pTexData = new xmTexData();
    pTexData->totalMipMaps = ddsd.dwMipMapCount;
    pTexData->width = ddsd.dwWidth;
    pTexData->height = ddsd.dwHeight;

    // DXT1 has a compression ratio of 8:1, DXT3 has 4:1, and DXT5 has 4:1.
    switch (ddsd.ddpfPixelFormat.dwFourCC) {
        case FOURCC_DXT1:
            pTexData->mipFactor = 2;
            pTexData->totalComponenets = 3;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;

        case FOURCC_DXT3:
            pTexData->mipFactor = 4;
            pTexData->totalComponenets = 4;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;

        case FOURCC_DXT5:
            pTexData->mipFactor = 4;
            pTexData->totalComponenets = 4;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;

        default:
            fclose(fp);
            return NULL;
            break;
    }

    if (ddsd.dwMipMapCount > 1) totalSize = imageSize * pTexData->mipFactor;
    else totalSize = imageSize;

    pTexData->texels = new unsigned char[totalSize];

    if (!pTexData->texels) {
        fclose(fp);
        return NULL;
    }

    fread(pTexData->texels, 1, totalSize, fp);

    fclose(fp);
    return pTexData;
}

bool WriteTGA(char *file, short int width, short int height, unsigned char *outImage) {
    FILE *pFile;               // The file pointer.
    unsigned char uselessChar; // used for useless char.
    short int uselessInt;      // used for useless int.
    unsigned char imageType;   // Type of image we are saving.
    int index;                 // used with the for loop.
    unsigned char bits;    // Bit depth.
    long Size;                 // Size of the picture.
    int colorMode;
    unsigned char tempColors;
    pFile = fopen(file, "wb");
    if (!pFile) {
        fclose(pFile);
        return false;
    }
    imageType = 2;
    colorMode = 3;
    bits = 24;
    uselessChar = 0;
    uselessInt = 0;
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&imageType, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&width, sizeof(short int), 1, pFile);
    fwrite(&height, sizeof(short int), 1, pFile);
    fwrite(&bits, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    Size = width * height * colorMode;
    for (index = 0; index < Size; index += colorMode) {
        tempColors = outImage[index];
        outImage[index] = outImage[index + 2];
        outImage[index + 2] = tempColors;
    }
    fwrite(outImage, sizeof(unsigned char), Size, pFile);
    fclose(pFile);
    return true;
}

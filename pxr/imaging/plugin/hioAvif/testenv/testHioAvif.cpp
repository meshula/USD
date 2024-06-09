//
// Copyright 2024 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/pxr.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/imaging/hio/image.h"
#include <array>
#include <mutex>
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE
int
main(int argc, char *argv[])
{
    // To enable running the test within xcode, where a cwd is not set, we
    // allow an optional argument to specify the root path for the test assets.
    // if there is an argument, use it as the root path for the test assets
    std::string rootPath = argc > 1 ? argv[1] : ".";
    // if the argument doesn't end with a slash, add one
    if (rootPath.back() != '/') {
        rootPath += "/";
    }
    
    std::string csGrayPng = "cs-gray-7f7f7f.png";
    std::string csGrayAvif = "cs-gray-7f7f7f.avif";

    int width = 0;
    int height = 0;
    HioImage::StorageSpec pngSpec;

    {
        // fetch basic information about the png to be compared with the avif
        std::string path = rootPath + csGrayPng;
        HioImageSharedPtr image = HioImage::OpenForReading(path);
        if (!TF_VERIFY(image)) {
            return 1;
        }

        width = image->GetWidth();
        height = image->GetHeight();
        TF_VERIFY(width > 0 && height > 0);
        pngSpec.width = width;
        pngSpec.height = height;
        pngSpec.format = image->GetFormat();
        pngSpec.flipped = false;
        std::vector<unsigned char> readback(width * height * 4);
        pngSpec.data = readback.data();
        TF_VERIFY(image->Read(pngSpec));

        std::string filename = "pngTestWriteback.png";
        HioImageSharedPtr writePngImage = HioImage::OpenForWriting(filename);
        TF_VERIFY(writePngImage);
        writePngImage->Write(pngSpec);
    }

    {
        // fetch basic information about the avif
        std::string path = rootPath + csGrayAvif;
        HioImageSharedPtr image = HioImage::OpenForReading(path);
        TF_VERIFY(image);
        TF_VERIFY(image->GetWidth() == width);
        TF_VERIFY(image->GetHeight() == height);
        TF_VERIFY(image->GetFormat() == HioFormatFloat16Vec4);

        HioImage::StorageSpec avifSpec;
        avifSpec.width = width;
        avifSpec.height = height;
        avifSpec.format = image->GetFormat(); // f16v4 is native
        std::vector<unsigned char> readback(width * height * sizeof(uint16_t) * 4);
        avifSpec.data = readback.data();
        TF_VERIFY(image->Read(avifSpec));
        {
            std::string filename = "avifTestWriteback16.exr";
            HioImageSharedPtr exrimage = HioImage::OpenForWriting(filename);
            TF_AXIOM(exrimage);
            TF_AXIOM(exrimage->Write(avifSpec));
        }

        HioImage::StorageSpec avifSpecF32;
        avifSpecF32.width = width;
        avifSpecF32.height = height;
        avifSpecF32.format = HioFormatFloat32Vec4;
        std::vector<unsigned char> readbackf32(width * height * sizeof(float) * 4);
        avifSpecF32.data = readbackf32.data();
        TF_VERIFY(image->Read(avifSpecF32));

        {
            std::string filename = "avifTestWriteback32.exr";
            HioImageSharedPtr exrimage = HioImage::OpenForWriting(filename);
            TF_AXIOM(exrimage);
            TF_AXIOM(exrimage->Write(avifSpecF32));
        }

        // compare the pixel data
        TF_VERIFY(pngSpec.data == avifSpec.data);
    }

    printf("OK\n");
    return 0;
}

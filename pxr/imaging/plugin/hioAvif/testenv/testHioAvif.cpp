//
// Copyright 2024 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
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
        storpngSpecageSpec.flipped = false;
        pngSpec.data = std::vector<unsigned char>(width * height * 4);
        TF_VERIFY(image->Read(storageSpec));
    }

    {
        // fetch basic information about the avif
        std::string path = rootPath + csGrayAvif;
        HioImageSharedPtr image = HioImage::OpenForReading(path);
        if (!TF_VERIFY(image)) {
            return 1;
        }

        TF_VERIFY(image->GetWidth() == width);
        TF_VERIFY(image->GetHeight() == height);
        TF_VERIFY(image->GetFormat() == pngSpec.format);
        TF_VERIFY(image->GetFlipped() == pngSpec.flipped);

        HioImage::StorageSpec avifSpec;
        avifSpec.width = width;
        avifSpec.height = height;
        avifSpec.format = image->GetFormat();
        avifSpec.flipped = image->GetFlipped();
        avifSpec.data = std::vector<unsigned char>(width * height * 4);
        TF_VERIFY(image->Read(avifSpec));

        // compare the pixel data
        TF_VERIFY(pngSpec.data == avifSpec.data);
    }

    printf("OK\n");
    return 0;
}

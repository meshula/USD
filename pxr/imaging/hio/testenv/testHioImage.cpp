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
#include "pxr/base/plug/plugin.h"
#include "pxr/base/plug/registry.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/usd/ar/resolver.h"
#include <array>
#include <mutex>

PXR_NAMESPACE_USING_DIRECTIVE

const int w = 256;
const int h = 256;
const std::array<uint8_t, w * h * 3>& GetRgb8Values()
{
    static std::once_flag _once;
    static std::array<uint8_t, w * h * 3> _rgb8Values;
    std::call_once(_once, []() {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int index = 3 * (y * w + x);
                _rgb8Values[index + 0] = x & 0xff;
                _rgb8Values[index + 1] = y & 0xff;
                _rgb8Values[index + 2] = (x + y) & 0xff;
            }
        }
    });
    return _rgb8Values;
}

const std::array<float, w * h * 3>& GetRgbFloatValues()
{
    static std::once_flag _once;
    static std::array<float, w * h * 3> _rgbFloatValues;
    std::call_once(_once, []() {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int index = 3 * (y * w + x);
                _rgbFloatValues[index + 0] = (x & 0xff) / 255.0f;
                _rgbFloatValues[index + 1] = (y & 0xff) / 255.0f;
                _rgbFloatValues[index + 2] = ((x + y) & 0xff) / 255.0f;
            }
        }
    });
    return _rgbFloatValues;
}

int
main(int argc, char *argv[])
{
    {
        // verify that the hio plugin exists
        bool found = false;
        for (PlugPluginPtr plug : PlugRegistry::GetInstance().GetAllPlugins()) {
            if (plug->GetName() == "hio") {
                found = true;
                break;
            }
        }
        TF_AXIOM(found);
    }
    {
        // validate that the Ar plugin loaded by asking the default resolver to
        // tell the filename extension; Ar is going to be used by Hio to load
        // images via the Asset API.
        const TfToken fileExtension(
                TfStringToLowerAscii(ArGetResolver().GetExtension("test.exr")));
        TF_AXIOM(fileExtension.GetString() == "exr");
    }
    {
        // get the TfType for HioImage
        TfType hioImageType = TfType::Find<HioImage>();
        int stockPlugins = 0;
        // validate that the HioImage subclass types are registered
        std::vector<TfType> hioImageTypes =
            PlugRegistry::GetInstance().GetDirectlyDerivedTypes(hioImageType);
        for (auto t : hioImageTypes) {
            auto tn = t.GetTypeName();
            if (tn == "Hio_OpenEXRImage" || tn == "Hio_StbImage")
                ++stockPlugins;
        }
        // at least exr and stb are always available.
        TF_AXIOM(stockPlugins == 2);
    }

    // check existence of built-in formats that should always be available.
    {
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.exr"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.bmp"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.jpg"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.jpeg"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.png"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.tga"));
        TF_AXIOM(HioImage::IsSupportedImageFile("dummy.hdr"));
        TF_AXIOM(!HioImage::IsSupportedImageFile("dummy.xml"));
    }

    // write out rgb8values as png, then read it back in and compare
    {
        const std::array<uint8_t, w * h * 3>& rgb8Values = GetRgb8Values();
        std::string filename = "test.png";
        HioImageSharedPtr image = HioImage::OpenForWriting(filename);
        TF_AXIOM(image);

        // create storage spec
        HioImage::StorageSpec storageSpec;
        storageSpec.width = w;
        storageSpec.height = h;
        storageSpec.format = HioFormatUNorm8Vec3srgb;
        storageSpec.flipped = false;
        storageSpec.data = (void*) rgb8Values.data();
        
        TF_AXIOM(image->Write(storageSpec));
        image.reset();

        image = HioImage::OpenForReading(filename);
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatUNorm8Vec3srgb);
        TF_AXIOM(image->GetBytesPerPixel() == 3);
        std::array<uint8_t, w * h * 3> readback;
        auto readSpec = storageSpec;
        readSpec.data = readback.data();
        TF_AXIOM(image->Read(readSpec));
        TF_AXIOM(rgb8Values == readback);
    }

    // repeat for jpeg
    {
        const std::array<uint8_t, w * h * 3>& rgb8Values = GetRgb8Values();
        std::string filename = "test.jpg";
        HioImageSharedPtr image = HioImage::OpenForWriting(filename);
        TF_AXIOM(image);

        // create storage spec
        HioImage::StorageSpec storageSpec;
        storageSpec.width = w;
        storageSpec.height = h;
        storageSpec.format = HioFormatUNorm8Vec3srgb;
        storageSpec.flipped = false;
        storageSpec.data = (void*) rgb8Values.data();

        TF_AXIOM(image->Write(storageSpec));
        image.reset();

        image = HioImage::OpenForReading(filename);
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatUNorm8Vec3srgb);
        TF_AXIOM(image->GetBytesPerPixel() == 3);
        std::array<uint8_t, w * h * 3> readback;
        auto readSpec = storageSpec;
        readSpec.data = readback.data();
        TF_AXIOM(image->Read(readSpec));

        // jpeg is lossy so compare the pixels with a tolerance of +/- 1
        for (int i = 0; i < w * h * 3; i++) {
            TF_AXIOM(rgb8Values[i] - 2 <= readback[i] && readback[i] <= rgb8Values[i] + 2);
        }
    }

    // do a lossless comparison for exr and float32
    {
        const std::array<float, w * h * 3>& rgbFloatValues = GetRgbFloatValues();
        std::string filename = "test.exr";
        HioImageSharedPtr image = HioImage::OpenForWriting(filename);
        TF_AXIOM(image);

        // create storage spec
        HioImage::StorageSpec storageSpec;
        storageSpec.width = w;
        storageSpec.height = h;
        storageSpec.format = HioFormatFloat32Vec3;
        storageSpec.flipped = false;
        storageSpec.data = (void*) rgbFloatValues.data();

        TF_AXIOM(image->Write(storageSpec));
        image.reset();

        image = HioImage::OpenForReading(filename);
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatFloat32Vec3);
        TF_AXIOM(image->GetBytesPerPixel() == sizeof(float) * 3);
        std::array<float, w * h * 3> readback;
        auto readSpec = storageSpec;
        readSpec.data = readback.data();
        TF_AXIOM(image->Read(readSpec));
        TF_AXIOM(rgbFloatValues == readback);
    }
    
    // test.exr now exists; read it requesting a half scale resize
    {
        const std::array<float, w * h * 3>& rgbFloatValues = GetRgbFloatValues();
        HioImageSharedPtr image = HioImage::OpenForReading("test.exr");
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatFloat32Vec3);
        TF_AXIOM(image->GetBytesPerPixel() == sizeof(float) * 3);
        std::array<float, w * h * 3 / 4> readback; // 1/2 size in each dimension

        int w2 = w/2;
        int h2 = h/2;

        HioImage::StorageSpec readSpec;
        readSpec.width = w2;
        readSpec.height = h2;
        readSpec.format = HioFormatFloat32Vec3;
        readSpec.data = readback.data();
        TF_AXIOM(image->Read(readSpec));
        // verify that the pixel values are approximately the same
        for (int y = 0; y < h2; y++) {
            for (int x = 0; x < w2; x++) {
                int index = 3 * (y * w2 + x);
                int index2 = 3 * (y * 2 * w + x * 2);
                // a loose comparison is fine for this test
                TF_AXIOM(fabsf(readback[index + 0] - rgbFloatValues[index2 + 0]) < 16.f/255.f);
                TF_AXIOM(fabsf(readback[index + 1] - rgbFloatValues[index2 + 1]) < 16.f/255.f);
                TF_AXIOM(fabsf(readback[index + 2] - rgbFloatValues[index2 + 2]) < 16.f/255.f);
            }
        }
    }

    // read the test.png as float32, which is expectd to fail
    {
        HioImageSharedPtr image = HioImage::OpenForReading("test.png");
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatUNorm8Vec3srgb);
        TF_AXIOM(image->GetBytesPerPixel() == 3);

        std::array<float, w * h * 3> readback;
        HioImage::StorageSpec readSpec;
        readSpec.width = w;
        readSpec.height = h;
        readSpec.format = HioFormatFloat32Vec3;
        readSpec.data = readback.data();
        TF_AXIOM(!image->Read(readSpec));
    }

    // read the test.jpg as rgba, which is expected to fail
    {
        HioImageSharedPtr image = HioImage::OpenForReading("test.jpg");
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatUNorm8Vec3srgb);
        TF_AXIOM(image->GetBytesPerPixel() == 3);
        std::array<uint8_t, w * h * 4> readback;
        HioImage::StorageSpec readSpec;
        readSpec.width = w;
        readSpec.height = h;
        readSpec.format = HioFormatUNorm8Vec4srgb;
        readSpec.data = readback.data();
        TF_AXIOM(!image->Read(readSpec));
    }

    // read the exr file as float32 rgba, and verify that the pixels are the
    // same and that the alpha channel is full of ones.
    {
        HioImageSharedPtr image = HioImage::OpenForReading("test.exr");
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatFloat32Vec3);
        TF_AXIOM(image->GetBytesPerPixel() == sizeof(float) * 3);
        std::array<float, w * h * 4> readback;
        HioImage::StorageSpec readSpec;
        readSpec.width = w;
        readSpec.height = h;
        readSpec.format = HioFormatFloat32Vec4;
        readSpec.data = readback.data();
        TF_AXIOM(image->Read(readSpec));
        // verify that the pixel values are the same
        const std::array<float, w * h * 3>& rgbFloatValues = GetRgbFloatValues();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int index = 4 * (y * w + x);
                int index3 = 3 * (y * w + x);
                TF_AXIOM(readback[index + 0] == rgbFloatValues[index3 + 0]);
                TF_AXIOM(readback[index + 1] == rgbFloatValues[index3 + 1]);
                TF_AXIOM(readback[index + 2] == rgbFloatValues[index3 + 2]);
                TF_AXIOM(readback[index + 3] == 1.0f);
            }
        }
    }

    // read the exr file as uint8_t rgba, and verify that the pixels are the
    // same and that the alpha channel is full of ones.
    {
        HioImageSharedPtr image = HioImage::OpenForReading("test.exr");
        TF_AXIOM(image);
        TF_AXIOM(image->GetWidth() == w);
        TF_AXIOM(image->GetHeight() == h);
        TF_AXIOM(image->GetFormat() == HioFormatFloat32Vec3);
        TF_AXIOM(image->GetBytesPerPixel() == sizeof(float) * 3);
        std::array<uint8_t, w * h * 4> readback;
        HioImage::StorageSpec readSpec;
        readSpec.width = w;
        readSpec.height = h;
        readSpec.format = HioFormatUNorm8Vec4srgb;
        readSpec.data = readback.data();
        TF_AXIOM(!image->Read(readSpec));
    }

    printf("OK\n");
    return 0;
}

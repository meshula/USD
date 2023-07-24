
#include "pxr/pxr.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/imaging/hio/types.h"
#include "pxr/base/plug/info.h"
#include <stdlib.h>
#include <vector>

#define INSTALL_LOCN "usd0715"

PXR_NAMESPACE_USING_DIRECTIVE

// ISO 17321-1:2012 Table D.1
GfVec3f ISO17321_ap0[24] = {
    { 0.11877f, 0.08709f, 0.05895f }, // patch 1
    { 0.40003f, 0.31916f, 0.23737f }, // patch 2
    { 0.18476f, 0.20398f, 0.31310f }, // patch 3
    { 0.10901f, 0.13511f, 0.06493f }, // patch 4
    { 0.26684f, 0.24604f, 0.40932f }, // patch 5
    { 0.32283f, 0.46208f, 0.40606f }, // patch 6
    { 0.38607f, 0.22744f, 0.05777f }, // patch 7
    { 0.13822f, 0.13037f, 0.33703f }, // patch 8
    { 0.30203f, 0.13752f, 0.12758f }, // patch 9
    { 0.09310f, 0.06347f, 0.13525f },
    { 0.34877f, 0.43655f, 0.10613f },
    { 0.48657f, 0.36686f, 0.08061f },
    { 0.08731f, 0.07443f, 0.27274f },
    { 0.15366f, 0.25692f, 0.09071f },
    { 0.21743f, 0.07070f, 0.05130f },
    { 0.58921f, 0.53944f, 0.09157f },
    { 0.30904f, 0.14818f, 0.27426f },
    { 0.14900f, 0.23377f, 0.35939f }, // out of gamut r709, R could be in error
    { 0.86653f, 0.86792f, 0.85818f },
    { 0.57356f, 0.57256f, 0.57169f },
    { 0.35346f, 0.35337f, 0.35391f },
    { 0.20253f, 0.20243f, 0.20287f },
    { 0.09467f, 0.09520f, 0.09637f },
    { 0.03745f, 0.03766f, 0.03895f }, // patch 24
};

GfVec3f ISO17321_r709[24] = {
   { 0.18326745, 0.08043892, 0.048751256f },
   { 0.57313635, 0.30258403, 0.202327929f },
   { 0.12166137, 0.19800845, 0.304997339f },
   { 0.10219760, 0.14818909, 0.048250962f },
   { 0.24686856, 0.22365069, 0.401356396f },
   { 0.14953951, 0.50350513, 0.364782663f },
   { 0.71046856, 0.19796405, 0.022039876f },
   { 0.07562225, 0.10815631, 0.342598452f },
   { 0.56877221, 0.09196745, 0.112304507f },
   { 0.11414032, 0.04817642, 0.135181963f },
   { 0.36200031, 0.48945945, 0.043899454f },
   { 0.80256076, 0.35834120, 0.024296749f },
   { 0.03318762, 0.05190392, 0.282282677f },
   { 0.06996592, 0.29976133, 0.057260607f },
   { 0.45702324, 0.03133905, 0.041221802f },
   { 0.86766099, 0.56453118, 0.008749157f },
   { 0.51730194, 0.09086778, 0.269164551f },
   { 0         , 0.24431530, 0.351227007f }, // out of gamut r709 r: -0.02131250
   { 0.90906175, 0.86448085, 0.783917811f },
   { 0.60242857, 0.56923897, 0.523116990f },
   { 0.37003347, 0.35136621, 0.324028232f },
   { 0.21204851, 0.20125644, 0.185761815f },
   { 0.09786984, 0.09471846, 0.088412194f },
   { 0.03839120, 0.03739227, 0.035869006f },
};

template <typename T>
void DrawRect(int w, int h, T r, T g, T b, GfVec2f uv0, GfVec2f uv1,
              T* rptr, T* gptr, T* bptr, int stride) {
    int startx = (int) (w * uv0[0]);
    int starty = (int) (h * uv0[1]);
    int endx = (int) (w * uv1[0]);
    int endy = (int) (h * uv1[1]);
    for (int y = starty; y < endy; ++y) {
        for (int x = startx; x < endx; ++x) {
            rptr[(y * w + x) * stride] = r;
            gptr[(y * w + x) * stride] = g;
            bptr[(y * w + x) * stride] = b;
        }
    }
}

template <typename T>
void ColorChipGrid(int w, int h, T* rptr, T* gptr, T* bptr, int stride) {
    int chipx = 0;
    int chipy = 0;
    const float ws = 6.f;
    const float hs = 4.f;
    float sx = 1.f / ws;
    float sy = 1.f / hs;
    chipy = 0;
    for (float ty = 0.0f; ty < 1.0f; ty += sy, chipy++) {
        chipx = 0;
        for (float tx = 0.0f; tx < 1.0f; tx += sx, chipx++) {
            GfVec2f c((tx + 0.5f * sx), (ty + 0.5f * sy));
            float k = 0.45f;
            GfVec2f uv0((c[0] - k * sx), (c[1] - k * sy));
            GfVec2f uv1((c[0] + k * sx), (c[1] + k * sy));
            int index = (chipy * 4) + chipx;
            //float r = powf((ty * 200) / 255.f, 2.2f);
            //float g = powf((tx * 255) / 255.f, 2.2f);
            //float b = powf(100.f / 255.f, 2.2f);
            GfVec3f color = ISO17321_r709[index];
            DrawRect<T>(w, h,
                        static_cast<T>(color[0]),
                        static_cast<T>(color[1]),
                        static_cast<T>(color[2]),
                        uv0, uv1,
                        rptr, gptr, bptr, stride);
        }
    }
}

template<typename T>
int PermuteCrops(HioImage::StorageSpec spec, const char* basename) {
    // we are going to permute all combinations of crop and flip during
    // reading. We'll do that by setting the crop window to all permutations
    // of zero, top, bottom, left, and right, and flip.
    // each of the resulting images will be written out.
    // The resulting thirty two images will be named:
    // t_b_l_r_f_basename.exr
    // We are going to crop top by 16, bottom by 32, left by 64, and right by 128.

    // now permute the crop window and flip and read, then write out the images:
    int w = spec.width;
    int h = spec.height;
    int stride = 3;
    for (int top = 0; top < 2; ++top) {
        for (int bottom = 0; bottom < 2; ++bottom) {
            for (int left = 0; left < 2; ++left) {
                for (int right = 0; right < 2; ++right) {
                    for (int flip = 0; flip < 2; ++flip) {
                        // read the image with the indicated crop and flip
                        spec.flipped = flip;
                        spec.width = w - left * 64 - right * 128;
                        spec.height = h - top * 16 - bottom * 32;
                        spec.data = malloc(sizeof(T) * spec.width * spec.height * stride);
                        HioImageSharedPtr _image = HioImage::OpenForReading(basename,
                                                                            0, // int subimage,
                                                                            0, // int mip,
                                                                            HioImage::SourceColorSpace::Auto,
                                                                            false); //bool suppressErrors)
                        if (!_image)
                            return EXIT_FAILURE;
                        if (!_image->ReadCropped(top * 16, bottom * 32, left * 64, right * 128, spec))
                            return EXIT_FAILURE;
                        // write the image out
                        std::string outname = std::string("t") + std::to_string(top) +
                            std::string("_b") + std::to_string(bottom) +
                            std::string("_l") + std::to_string(left) +
                            std::string("_r") + std::to_string(right) +
                            std::string("_f") + std::to_string(flip) +
                            std::string("____") + std::string(basename);
                        auto writeImage = HioImage::OpenForWriting(outname.c_str());
                        if (!writeImage)
                            return EXIT_FAILURE;
                        if (!writeImage->Write(spec))
                            return EXIT_FAILURE;

                        // repeat, and squish the image
                        outname = "t" + std::to_string(top) +
                            "_b" + std::to_string(bottom) +
                            "_l" + std::to_string(left) +
                            "_r" + std::to_string(right) +
                            "_f" + std::to_string(flip) +
                            "_sq_" + std::string(basename);
                        spec.width = 300;
                        spec.height = 200;
                        _image = HioImage::OpenForReading(basename,
                                                          0, // int subimage,
                                                          0, // int mip,
                                                          HioImage::SourceColorSpace::Auto,
                                                          false); //bool suppressErrors)
                        if (!_image->ReadCropped(top * 16, bottom * 32, left * 64, right * 128, spec))
                            return EXIT_FAILURE;
                        writeImage = HioImage::OpenForWriting(outname.c_str());
                        if (!writeImage)
                            return EXIT_FAILURE;
                        if (!writeImage->Write(spec))
                            return EXIT_FAILURE;

                        free(spec.data);
                    }
                }
            }
        }
    }
}

int
main(int argc, char **argv)
{
    using std::vector;
    vector<std::string> test;
    vector<std::string> paths;
    paths.emplace_back("/var/tmp/" INSTALL_LOCN "/lib/usd");
    paths.emplace_back("/var/tmp/" INSTALL_LOCN "/plugin/usd");
    std::vector<std::string> msgs;
    msgs.emplace_back("looking for plugs here: /var/tmp/" INSTALL_LOCN "/lib/usd");
    Plug_SetPaths(paths, msgs, true);

    const char* out32_exr = "squares32.exr";
    const char* out32_test_exr = "squares32_test.exr";
    const char* out32_test_flipped_exr = "squares32_test_flipped.exr";
    const char* out16_exr = "squares16.exr";
    const char* out16_test_exr = "squares16_test.exr";
    const char* out16_test_flipped_exr = "squares16_test_flipped.exr";
    const int w = 640;
    const int h = 360;
    vector<float> pixels(w * h * 3, 0.f);
    
    ColorChipGrid<float>(w, h, pixels.data(), pixels.data() + 1, pixels.data() + 2, 3);
    vector<GfHalf> half_pixels(w * h * 3);
    ColorChipGrid<GfHalf>(w, h, half_pixels.data(), half_pixels.data() + 1, half_pixels.data() + 2, 3);
    
    HioImage::StorageSpec _spec;
    _spec.width  = w;
    _spec.height = h;
    _spec.format = HioFormatFloat32Vec3;
    _spec.flipped = false;
    _spec.data = pixels.data();
    auto writeImage = HioImage::OpenForWriting(out32_exr);
    if (!writeImage)
        return EXIT_FAILURE;

    PermuteCrops<float>(_spec, out32_exr);
    
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;

    // read the image back and compare the pixels to the source array.
    bool canReadExr = HioImage::IsSupportedImageFile(out32_exr);
    std::unique_ptr<char[]> imageData;
    if (!canReadExr)
        return EXIT_FAILURE;

    auto _image = HioImage::OpenForReading(out32_exr,
                                        0, // int subimage,
                                        0, // int mip,
                                        HioImage::SourceColorSpace::Auto,
                                        false); //bool suppressErrors)
    if (!_image)
        return EXIT_FAILURE;

    vector<float> read32_pixels(w * h * 3, 0.f);
    _spec.data = read32_pixels.data();
    if (!_image->Read(_spec))
        return EXIT_FAILURE;

    writeImage = HioImage::OpenForWriting(out32_test_exr);
    if (!writeImage)
        return EXIT_FAILURE;
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;
    
    // now read flipped
    _spec.flipped = true;
    if (!_image->Read(_spec))
        return EXIT_FAILURE;

    writeImage = HioImage::OpenForWriting(out32_test_flipped_exr);
    if (!writeImage)
        return EXIT_FAILURE;
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;


    // now do the same thing for half
    _spec.format = HioFormatFloat16Vec3;
    _spec.data = half_pixels.data();
    _spec.flipped = false;
    writeImage = HioImage::OpenForWriting(out16_exr);
    if (!writeImage)
        return EXIT_FAILURE;
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;

    _image = HioImage::OpenForReading(out16_exr,
                                        0, // int subimage,
                                        0, // int mip,
                                        HioImage::SourceColorSpace::Auto,
                                        false); //bool suppressErrors)
    if (!_image)
        return EXIT_FAILURE;

    vector<GfHalf> read16_pixels(w * h * 3);
    _spec.data = read16_pixels.data();
    if (!_image->Read(_spec))
        return EXIT_FAILURE;

    writeImage = HioImage::OpenForWriting(out16_test_exr);
    if (!writeImage)
        return EXIT_FAILURE;
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;

    // now read flipped
    _spec.flipped = true;
    if (!_image->Read(_spec))
        return EXIT_FAILURE;

    writeImage = HioImage::OpenForWriting(out16_test_flipped_exr);
    if (!writeImage)
        return EXIT_FAILURE;
    if (!writeImage->Write(_spec))
        return EXIT_FAILURE;

    // do the cropped read checks
    return EXIT_SUCCESS;
}

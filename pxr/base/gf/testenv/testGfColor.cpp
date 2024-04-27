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
#include "pxr/base/gf/color.h"
#include "pxr/base/gf/colorSpace.h"
#include "pxr/base/tf/diagnostic.h"

PXR_NAMESPACE_USING_DIRECTIVE

int
main(int argc, char *argv[])
{
    // test default construction
    {
        GfColor c;
        TF_AXIOM(c.GetColorSpace().GetName() == GfColorSpaceCanonicalName->LinearRec709);
        TF_AXIOM(c.GetRGB() == GfVec3f(1.0f, 1.0f, 1.0f));
    }
    // test construction with color space
    {
        GfColor c(GfColorSpaceCanonicalName->SRGB);
        TF_AXIOM(c.GetColorSpace().GetName() == GfColorSpaceCanonicalName->SRGB);
        TF_AXIOM(c.GetRGB() == GfVec3f(1.0f, 1.0f, 1.0f));
    }
    // test construction with color space and RGB
    {
        GfColor c(GfColorSpaceCanonicalName->SRGB, GfVec3f(0.5f, 0.5f, 0.5f));
        TF_AXIOM(c.GetColorSpace().GetName() == GfColorSpaceCanonicalName->SRGB);
        TF_AXIOM(c.GetRGB() == GfVec3f(0.5f, 0.5f, 0.5f));
    }    

    printf("OK\n");
    return 0;
}

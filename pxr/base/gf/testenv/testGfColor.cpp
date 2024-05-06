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

bool ColorApproxEq(const GfColor& c1, const GfColor& c2)
{
    return GfIsClose(c1.GetRGB(), c2.GetRGB(), 1e-5f);
}

// Calculate barycentric coordinates of a point p with respect to a triangle formed by vertices v0, v1, and v2
bool PointInTriangle(const GfVec2f& p, const GfVec2f& v0, const GfVec2f& v1, const GfVec2f& v2) {
    GfVec2f v0v1 = v1 - v0;
    GfVec2f v0v2 = v2 - v0;
    GfVec2f vp = p - v0;

    float dot00 = GfDot(v0v1, v0v1);
    float dot01 = GfDot(v0v1, v0v2);
    float dot02 = GfDot(v0v1, vp);
    float dot11 = GfDot(v0v2, v0v2);
    float dot12 = GfDot(v0v2, vp);

    // Compute barycentric coordinates
    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point p is inside the triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

/*
    Note that by necessity, GfColor & GfColorSpace are tested together.
 */

int
main(int argc, char *argv[])
{
    GfColorSpace csSRGB(GfColorSpaceNames->SRGB);
    GfColorSpace csLinearSRGB(GfColorSpaceNames->LinearSRGB);
    GfColorSpace csLinearRec709(GfColorSpaceNames->LinearRec709);
    GfColorSpace csG22Rec709(GfColorSpaceNames->G22Rec709);
    GfColorSpace csAp0(GfColorSpaceNames->LinearAP0);
    GfColorSpace csAp0D65(GfColorSpaceNames->LinearAP0D65);
    GfColorSpace csSRGBP3(GfColorSpaceNames->SRGBDisplayP3);
    GfColorSpace csLinearRec2020(GfColorSpaceNames->LinearRec2020);

    GfColor mauveLinear(GfVec3f(0.5f, 0.25f, 0.125f), csLinearRec709);
    GfColor mauveGamma(mauveLinear, csG22Rec709);

    GfVec2f wpD65xy = GfColor(GfVec3f(1.0f, 1.0f, 1.0f), csLinearRec709).GetCIEXY();

    GfVec2f ap0Primaries[3] = {
        { 0.7347, 0.2653  },
        { 0.0000, 1.0000  },
        { 0.0001, -0.0770 },
    };
    GfVec2f rec2020Primaries[3] = {
        { 0.708, 0.292 },
        { 0.170, 0.797 },
        { 0.131, 0.046 },
    };
    GfVec2f rec709Primaries[3] = {
        { 0.640, 0.330 },
        { 0.300, 0.600 },
        { 0.150, 0.060 },
    };

    // test default construction
    {
        GfColor c;
        TF_AXIOM(c.GetColorSpace() == csLinearRec709);
        TF_AXIOM(c.GetRGB() == GfVec3f(0, 0, 0));
    }
    // test construction with color space
    {
        GfColor c(csSRGB);
        TF_AXIOM(c.GetColorSpace() == csSRGB);
        TF_AXIOM(c.GetRGB() == GfVec3f(0, 0, 0));
    }
    // test construction with color space and RGB
    {
        GfColor c(GfVec3f(0.5f, 0.5f, 0.5f), csSRGB);
        TF_AXIOM(c.GetColorSpace() == csSRGB);
        TF_AXIOM(c.GetRGB() == GfVec3f(0.5f, 0.5f, 0.5f));
    }
    {
        // test that a EOTF curve <-> Linear works
        GfColor c1(mauveLinear, csSRGB);    // convert linear to SRGB
        GfColor c2(c1, csLinearSRGB);
        TF_AXIOM(GfIsClose(mauveLinear, c2, 1e-7f));
        GfColor c3(c2, csSRGB);
        TF_AXIOM(GfIsClose(c1, c3, 1e-7f));
    }
    {
        // test round tripping to Rec2020
        GfColor c1(mauveLinear, csLinearRec2020);
        GfColor c2(c1, csLinearSRGB);
        TF_AXIOM(GfIsClose(mauveLinear, c2, 1e-7f));
    }
    // test CIEXY equality, and thus also GetCIEXY
    {
        // MauveLinear and Gamma are both have a D65 white point.
        GfColor c1(mauveLinear, csSRGB);
        GfColor c2(c1, csAp0);                      // different white point
        GfColor c2d65(c2, csAp0D65);                // adapt to d65 for comparison
        GfColor cmauve(c2, csLinearRec709);
        GfColor cmauveD65(c2d65, csLinearRec709);

        GfVec2f chr1 = mauveLinear.GetCIEXY();
        GfVec2f chr2 = mauveGamma.GetCIEXY();
        GfVec2f chr3 = c1.GetCIEXY();
        GfVec2f chr4 = c2.GetCIEXY();
        GfVec2f chr5 = c2d65.GetCIEXY();

        TF_AXIOM(GfIsClose(chr1, chr2, 1e-5f));
        TF_AXIOM(GfIsClose(chr1, chr3, 1e-5f));
        TF_AXIOM(GfIsClose(chr4, chr5, 2e-3f));
        TF_AXIOM(GfIsClose(chr1, chr4, 5e-2f));
        TF_AXIOM(GfIsClose(chr1, chr5, 2e-2f));
    }
    // test construction with adaptation
    {
        GfColor colG22Rec709(mauveLinear, csG22Rec709);
        TF_AXIOM(GfIsClose(colG22Rec709, mauveGamma, 1e-5f));
        GfColor colLinRec709(colG22Rec709, csLinearRec709);
        TF_AXIOM(GfIsClose(colLinRec709, mauveLinear, 1e-5f));

        // verify assignment didn't mutate cs
        TF_AXIOM(colG22Rec709.GetColorSpace() == csG22Rec709);

        TF_AXIOM(colLinRec709.GetColorSpace() == csLinearRec709);
        GfColor colSRGB_2(colLinRec709, csSRGB);
        GfVec2f xy1 = colG22Rec709.GetCIEXY();
        GfVec2f xy2 = colSRGB_2.GetCIEXY();
        TF_AXIOM(GfIsClose(xy1, xy2, 1e-5f));
        GfColor colAp0(colSRGB_2, csAp0);
        GfVec2f xy3 = colAp0.GetCIEXY();
        TF_AXIOM(GfIsClose(xy1, xy3, 3e-2f));
        GfColor colSRGB_3(colAp0, csSRGB);
        GfVec2f xy4 = colAp0.GetCIEXY();
        TF_AXIOM(GfIsClose(xy1, xy4, 3e-2f));
        GfColor col_SRGBP3(colSRGB_3, csSRGBP3);
        GfVec2f xy5 = col_SRGBP3.GetCIEXY();
        TF_AXIOM(GfIsClose(xy1, xy5, 3e-2f));

        // all the way back to rec709
        GfColor colLinRec709_2(col_SRGBP3, csLinearRec709);
        TF_AXIOM(GfIsClose(colLinRec709_2, colLinRec709, 1e-5f));
    }
    // test move constructor
    {
        GfColor c1(GfVec3f(0.5f, 0.25f, 0.125f), csAp0);
        GfColor c2(std::move(c1));
        GfColorSpace c1cs = c1.GetColorSpace();
        TF_AXIOM(c2.GetColorSpace() == csAp0);
        TF_AXIOM(GfIsClose(c2.GetRGB(), GfVec3f(0.5f, 0.25f, 0.125f), 1e-5f));
    }
    // test copy assignment
    {
        GfColor c1(GfVec3f(0.5f, 0.25f, 0.125f), csSRGB);
        GfColor c2 = c1;
        TF_AXIOM(GfIsClose(c1, c2, 1e-5f));
        TF_AXIOM(c1.GetColorSpace() == c2.GetColorSpace());

        // move assignment, overwriting existing values
        GfColor c3(GfVec3f(0.5f, 0.25f, 0.125f), csAp0);
        GfColor c4(GfVec3f(0.25f, 0.5f, 0.125f), csSRGB);
        c3 = std::move(c4);
        TF_AXIOM(GfIsClose(c3.GetRGB(), GfVec3f(0.25f, 0.5f, 0.125f), 1e-5f));
        TF_AXIOM(c3.GetColorSpace() == csSRGB);
    }
    // test that the color spaces are constructed from primaries
    {
        TF_AXIOM(csSRGB.IsConstructedFromPrimaries());
        TF_AXIOM(csLinearSRGB.IsConstructedFromPrimaries());
        TF_AXIOM(csLinearRec709.IsConstructedFromPrimaries());
        TF_AXIOM(csG22Rec709.IsConstructedFromPrimaries());
        TF_AXIOM(csAp0.IsConstructedFromPrimaries());
        TF_AXIOM(csAp0D65.IsConstructedFromPrimaries());
        TF_AXIOM(csSRGBP3.IsConstructedFromPrimaries());
        TF_AXIOM(csLinearRec2020.IsConstructedFromPrimaries());
    }
    // test color space inequality
    {
        TF_AXIOM(csSRGB != csLinearSRGB);
        TF_AXIOM(csSRGB != csLinearRec709);
        TF_AXIOM(csSRGB != csG22Rec709);
        TF_AXIOM(csSRGB != csAp0);
        TF_AXIOM(csSRGB != csAp0D65);
        TF_AXIOM(csSRGB != csSRGBP3);
        TF_AXIOM(csSRGB != csLinearRec2020);
    }
    // test that constructing from Kelvin at 6500 is D65
    {
        GfColor c;
        c.SetFromBlackbodyKelvin(6500, 1.0f);
        GfVec2f xy = c.GetCIEXY();
        TF_AXIOM(GfIsClose(xy, wpD65xy, 1e-2f));
    }
    // test that primaries correspond to unit vectors in their color space
    {
        GfColor c1(csAp0);
        c1.SetFromCIEXY(ap0Primaries[0]);
        GfColor c2(csAp0);
        c2.SetFromCIEXY(ap0Primaries[1]);
        GfColor c3(csAp0);
        c3.SetFromCIEXY(ap0Primaries[2]);
        TF_AXIOM(GfIsClose(c1, GfColor(GfVec3f(1, 0, 0), csAp0), 1e-5f));
        TF_AXIOM(GfIsClose(c2, GfColor(GfVec3f(0, 1, 0), csAp0), 1e-5f));
        TF_AXIOM(GfIsClose(c3, GfColor(GfVec3f(0, 0, 1), csAp0), 1e-5f));

        GfColor c4(csLinearRec2020);
        c4.SetFromCIEXY(rec2020Primaries[0]);
        GfColor c5(csLinearRec2020);
        c5.SetFromCIEXY(rec2020Primaries[1]);
        GfColor c6(csLinearRec2020);
        c6.SetFromCIEXY(rec2020Primaries[2]);
        TF_AXIOM(GfIsClose(c4, GfColor(GfVec3f(1, 0, 0), csLinearRec2020), 1e-5f));
        TF_AXIOM(GfIsClose(c5, GfColor(GfVec3f(0, 1, 0), csLinearRec2020), 1e-5f));
        TF_AXIOM(GfIsClose(c6, GfColor(GfVec3f(0, 0, 1), csLinearRec2020), 1e-5f));

        GfColor c7(csLinearRec709);
        c7.SetFromCIEXY(rec709Primaries[0]);
        GfColor c8(csLinearRec709);
        c8.SetFromCIEXY(rec709Primaries[1]);
        GfColor c9(csLinearRec709);
        c9.SetFromCIEXY(rec709Primaries[2]);
        TF_AXIOM(GfIsClose(c7, GfColor(GfVec3f(1, 0, 0), csLinearRec709), 1e-5f));
        TF_AXIOM(GfIsClose(c8, GfColor(GfVec3f(0, 1, 0), csLinearRec709), 1e-5f));
        TF_AXIOM(GfIsClose(c9, GfColor(GfVec3f(0, 0, 1), csLinearRec709), 1e-5f));
    }

    // permute the rec709 primaries through rec2020 and ap0
    // and verify that at each point the adapted colors are contained
    // within the gamut of the target color space by testing that the
    // adapted color is within the triangle formed by the target color
    // space's primaries
    {
        // Create adapted colors
        GfColor red709(GfVec3f(1.0f, 0.0f, 0.0f), csLinearRec709);
        GfColor green709(GfVec3f(0.0f, 1.0f, 0.0f), csLinearRec709);
        GfColor blue709(GfVec3f(0.0f, 0.0f, 1.0f), csLinearRec709);
        GfColor red2020(GfVec3f(1.0f, 0.0f, 0.0f), csLinearRec2020);
        GfColor green2020(GfVec3f(0.0f, 1.0f, 0.0f), csLinearRec2020);
        GfColor blue2020(GfVec3f(0.0f, 0.0f, 1.0f), csLinearRec2020);
        GfColor redAp0(GfVec3f(1.0f, 0.0f, 0.0f), csAp0);
        GfColor greenAp0(GfVec3f(0.0f, 1.0f, 0.0f), csAp0);
        GfColor blueAp0(GfVec3f(0.0f, 0.0f, 1.0f), csAp0);

        // Verify that adapted 709 colors are within rec2020 gamut
        TF_AXIOM(PointInTriangle(red709.GetCIEXY(),   
                                 red2020.GetCIEXY(), blue2020.GetCIEXY(), green2020.GetCIEXY()));
        TF_AXIOM(PointInTriangle(green709.GetCIEXY(), 
                                 red2020.GetCIEXY(), blue2020.GetCIEXY(), green2020.GetCIEXY()));
        TF_AXIOM(PointInTriangle(blue709.GetCIEXY(),  
                                 red2020.GetCIEXY(), blue2020.GetCIEXY(), green2020.GetCIEXY()));

        // Verify that adapted 709 colors are within ap0 gamut
        TF_AXIOM(PointInTriangle(red709.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));
        TF_AXIOM(PointInTriangle(green709.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));
        TF_AXIOM(PointInTriangle(blue709.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));

        // Verify that adapted rec2020 colors are within ap0 gamut
        TF_AXIOM(PointInTriangle(red2020.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));
        TF_AXIOM(PointInTriangle(green2020.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));
        TF_AXIOM(PointInTriangle(blue2020.GetCIEXY(),
                                 redAp0.GetCIEXY(), blueAp0.GetCIEXY(), greenAp0.GetCIEXY()));
    }
    
    printf("OK\n");
    return 0;
}

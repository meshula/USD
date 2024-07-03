#!/pxrpythonsubst
# 
# Copyright 2024 Pixar
#
# Licensed under the terms set forth in the LICENSE.txt file available at
# https://openusd.org/license/.
#

import unittest
from pxr import Gf

def color_approx_eq(c1, c2):
    return Gf.IsClose(c1.GetRGB(), c2.GetRGB(), 1e-5)

def point_in_triangle(p, v0, v1, v2):
    v0v1 = v1 - v0
    v0v2 = v2 - v0
    vp = p - v0

    dot00 = Gf.Dot(v0v1, v0v1)
    dot01 = Gf.Dot(v0v1, v0v2)
    dot02 = Gf.Dot(v0v1, vp)
    dot11 = Gf.Dot(v0v2, v0v2)
    dot12 = Gf.Dot(v0v2, vp)

    invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01)
    u = (dot11 * dot02 - dot01 * dot12) * invDenom
    v = (dot00 * dot12 - dot01 * dot02) * invDenom

    return (u >= 0) and (v >= 0) and (u + v <= 1)

class TestGfColor(unittest.TestCase):

    def setUp(self):
        self.csSRGB = Gf.ColorSpace(Gf.ColorSpaceNames.SRGB)
        self.csLinearSRGB = Gf.ColorSpace(Gf.ColorSpaceNames.LinearSRGB)
        self.csLinearRec709 = Gf.ColorSpace(Gf.ColorSpaceNames.LinearRec709)
        self.csG22Rec709 = Gf.ColorSpace(Gf.ColorSpaceNames.G22Rec709)
        self.csAp0 = Gf.ColorSpace(Gf.ColorSpaceNames.LinearAP0)
        self.csSRGBP3 = Gf.ColorSpace(Gf.ColorSpaceNames.SRGBDisplayP3)
        self.csLinearRec2020 = Gf.ColorSpace(Gf.ColorSpaceNames.LinearRec2020)
        self.csIdentity = Gf.ColorSpace(Gf.ColorSpaceNames.Identity)

        self.mauveLinear = Gf.Color(Gf.Vec3f(0.5, 0.25, 0.125), self.csLinearRec709)
        self.mauveGamma = Gf.Color(self.mauveLinear, self.csG22Rec709)

        self.wpD65xy = Gf.Color(Gf.Vec3f(1.0, 1.0, 1.0), self.csLinearRec709).GetChromaticity()

        self.ap0Primaries = [
            Gf.Vec2f(0.7347, 0.2653),
            Gf.Vec2f(0.0, 1.0),
            Gf.Vec2f(0.0001, -0.0770)
        ]
        self.rec2020Primaries = [
            Gf.Vec2f(0.708, 0.292),
            Gf.Vec2f(0.17, 0.797),
            Gf.Vec2f(0.131, 0.046)
        ]
        self.rec709Primaries = [
            Gf.Vec2f(0.64, 0.33),
            Gf.Vec2f(0.3, 0.6),
            Gf.Vec2f(0.15, 0.06)
        ]

    def test_default_construction(self):
        c = Gf.Color()
        self.assertEqual(c.GetColorSpace(), self.csLinearRec709)
        self.assertEqual(c.GetRGB(), Gf.Vec3f(0, 0, 0))

    def test_construction_with_color_space(self):
        c = Gf.Color(self.csSRGB)
        self.assertEqual(c.GetColorSpace(), self.csSRGB)
        self.assertEqual(c.GetRGB(), Gf.Vec3f(0, 0, 0))

    def test_construction_with_color_space_and_rgb(self):
        c = Gf.Color(Gf.Vec3f(0.5, 0.5, 0.5), self.csSRGB)
        self.assertEqual(c.GetColorSpace(), self.csSRGB)
        self.assertEqual(c.GetRGB(), Gf.Vec3f(0.5, 0.5, 0.5))

    def test_eotf_curve_linear(self):
        c1 = Gf.Color(self.mauveLinear, self.csSRGB)  # convert linear to SRGB
        c2 = Gf.Color(c1, self.csLinearSRGB)
        self.assertTrue(Gf.IsClose(self.mauveLinear, c2, 1e-7))
        c3 = Gf.Color(c2, self.csSRGB)
        self.assertTrue(Gf.IsClose(c1, c3, 1e-7))

    def test_round_tripping_to_rec2020(self):
        c1 = Gf.Color(self.mauveLinear, self.csLinearRec2020)
        c2 = Gf.Color(c1, self.csLinearSRGB)
        self.assertTrue(Gf.IsClose(self.mauveLinear, c2, 1e-7))

    def test_cie_xy_equality(self):
        col_SRGB = Gf.Color(self.mauveLinear, self.csSRGB)
        col_ap0 = Gf.Color(col_SRGB, self.csAp0)  # different white point
        col_SRGBP3 = Gf.Color(col_ap0, self.csSRGBP3)  # adapt to d65 for comparison
        col_SRGB_2 = Gf.Color(col_ap0, self.csSRGB)
        col_SRGB_3 = Gf.Color(col_SRGBP3, self.csSRGB)

        cr_baseline_linear = self.mauveLinear.GetChromaticity()
        cr_baseline_curve = self.mauveGamma.GetChromaticity()
        cr_SRGB = col_SRGB.GetChromaticity()
        cr_SRGB_2 = col_SRGB_2.GetChromaticity()
        cr_SRGB_3 = col_SRGB_3.GetChromaticity()

        self.assertTrue(Gf.IsClose(cr_baseline_linear, cr_baseline_curve, 1e-5))
        self.assertTrue(Gf.IsClose(cr_baseline_linear, cr_SRGB, 1e-5))
        self.assertTrue(Gf.IsClose(cr_SRGB_2, cr_SRGB_3, 2e-2))
        self.assertTrue(Gf.IsClose(cr_baseline_linear, cr_SRGB_2, 5e-2))
        self.assertTrue(Gf.IsClose(cr_baseline_linear, cr_SRGB_3, 2e-2))

    def test_construction_with_conversion(self):
        colG22Rec709 = Gf.Color(self.mauveLinear, self.csG22Rec709)
        self.assertTrue(Gf.IsClose(colG22Rec709, self.mauveGamma, 1e-5))
        colLinRec709 = Gf.Color(colG22Rec709, self.csLinearRec709)
        self.assertTrue(Gf.IsClose(colLinRec709, self.mauveLinear, 1e-5))

        self.assertEqual(colG22Rec709.GetColorSpace(), self.csG22Rec709)
        self.assertEqual(colLinRec709.GetColorSpace(), self.csLinearRec709)

        colSRGB_2 = Gf.Color(colLinRec709, self.csSRGB)
        xy1 = colG22Rec709.GetChromaticity()
        xy2 = colSRGB_2.GetChromaticity()
        self.assertTrue(Gf.IsClose(xy1, xy2, 1e-5))

        colAp0 = Gf.Color(colSRGB_2, self.csAp0)
        xy3 = colAp0.GetChromaticity()
        self.assertTrue(Gf.IsClose(xy1, xy3, 3e-2))

        colSRGB_3 = Gf.Color(colAp0, self.csSRGB)
        xy4 = colAp0.GetChromaticity()
        self.assertTrue(Gf.IsClose(xy1, xy4, 3e-2))

        col_SRGBP3 = Gf.Color(colSRGB_3, self.csSRGBP3)
        xy5 = col_SRGBP3.GetChromaticity()
        self.assertTrue(Gf.IsClose(xy1, xy5, 3e-2))

        colLinRec709_2 = Gf.Color(col_SRGBP3, self.csLinearRec709)
        self.assertTrue(Gf.IsClose(colLinRec709_2, colLinRec709, 1e-5))

    def test_move_constructor(self):
        c1 = Gf.Color(Gf.Vec3f(0.5, 0.25, 0.125), self.csAp0)
        c2 = Gf.Color(c1)  # Python doesn't have move semantics, but this tests copying
        self.assertEqual(c2.GetColorSpace(), self.csAp0)
        self.assertTrue(Gf.IsClose(c2.GetRGB(), Gf.Vec3f(0.5, 0.25, 0.125), 1e-5))

    def test_copy_assignment(self):
        c1 = Gf.Color(Gf.Vec3f(0.5, 0.25, 0.125), self.csAp0)
        c2 = Gf.Color()
        c2 = c1
        self.assertEqual(c2.GetColorSpace(), self.csAp0)
        self.assertTrue(Gf.IsClose(c2.GetRGB(), Gf.Vec3f(0.5, 0.25, 0.125), 1e-5))

    def test_comparison(self):
        c1 = Gf.Color(Gf.Vec3f(0.5, 0.25, 0.125), self.csAp0)
        c2 = Gf.Color(Gf.Vec3f(0.5, 0.25, 0.125), self.csAp0)
        self.assertTrue(Gf.IsClose(c1, c2, 1e-5))
        self.assertEqual(c1.GetColorSpace(), c2.GetColorSpace())
        self.assertTrue(color_approx_eq(c1, c2))

    def test_in_triangle(self):
        v0 = self.rec709Primaries[0]
        v1 = self.rec709Primaries[1]
        v2 = self.rec709Primaries[2]

        self.assertTrue(point_in_triangle(self.wpD65xy, v0, v1, v2))

        v0 = self.rec2020Primaries[0]
        v1 = self.rec2020Primaries[1]
        v2 = self.rec2020Primaries[2]

        self.assertTrue(point_in_triangle(self.wpD65xy, v0, v1, v2))

        v0 = self.ap0Primaries[0]
        v1 = self.ap0Primaries[1]
        v2 = self.ap0Primaries[2]

        self.assertTrue(point_in_triangle(self.wpD65xy, v0, v1, v2))

if __name__ == '__main__':
    unittest.main()

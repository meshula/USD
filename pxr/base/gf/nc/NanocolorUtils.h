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

#ifndef nanocolor_extras_h
#define nanocolor_extras_h

#include "Nanocolor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NcKelvinToXYZ                NCCONCAT(NCNAMESPACE, KelvinToXYZ)
#define NcISO17321_AP0_ColorChips    NCCONCAT(NCNAMESPACE, ISO17321_ap0_24)
#define NcProjectToChromaticities    NCCONCAT(NCNAMESPACE, ProjectToChromaticities)
#define NcNormalizeXYZ               NCCONCAT(NCNAMESPACE, NormalizeXYZ)
#define NcRGBFromYxy                 NCCONCAT(NCNAMESPACE, RGBFromYxy)
#define NcCIE1931ColorFromWavelength NCCONCAT(NCNAMESPACE, CIE1931ColorFromWavelength)

// returns an XYZ coordinate for the blackbody emission spectrum
// for values between 1667 and 25000K
NCAPI NcCIEXYZ NcKelvinToXYZ(float temperature, float luminosity);

// return a pointer to 24 color values in ap0 corresponding to
// the 24 color chips in ISO standard chart 17321
NCAPI NcRGB* NcISO17321_AP0_ColorChips();

// given a CIEXYZ 1931 color coordinate, project it to the
// regularized chromaticity coordinate
NCAPI NcCIEXYZ NcProjectToChromaticities(NcCIEXYZ c);

NCAPI NcCIEXYZ NcNormalizeXYZ(NcCIEXYZ c);

NCAPI NcRGB NcRGBFromYxy(NcColorSpace* cs, NcCIEXYZ c);

NCAPI NcCIEXYZ NcCIE1931ColorFromWavelength(float lambda, bool approx);

#ifdef __cplusplus
}
#endif

#endif /* nanocolor_extras_h */
 
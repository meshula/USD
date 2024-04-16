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
////////////////////////////////////////////////////////////////////////

#include "pxr/pxr.h"
#include "pxr/base/gf/color.h"

#include "pxr/base/gf/pyBufferUtils.h"

#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/pyContainerConversions.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/base/tf/wrapTypeHelpers.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/make_constructor.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_arg.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/slice.hpp>

#include <string>

using namespace boost::python;

using std::string;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

static string __repr__(GfColorSpace const &self) 
{
    GfVec3f v = self.GetRGB();
    string colorSpaceName = self.>GetName().GetString();
    return TF_PY_REPR_PREFIX + 
        TfStringPrintf("ColorSpace(%s)", 
                              v[0], v[1], v[2], colorSpaceName.c_str());
}


} // anon

void wrapColor()
{
    typedef GfColorSpace This;

    scope thisScope = 
        class_<This>("ColorSpace", "Color space", init<>())
            .def(init<>())
            .def(init<TfToken>())
            .def(init<const TfToken, 
                      const GfVec2f&, const GfVec2f&, const GfVec2f&, 
                      const GfVec2f&, float, float, float, float>())
            .def(init<const TfToken&, const GfMatrix3f&, float, float, float, float>())
            .def("GetName", &This::GetName)
            .def(self == self)
            .def(self != self)
            .def(str(self))
            .def("__repr__", __repr__)
            ;
}

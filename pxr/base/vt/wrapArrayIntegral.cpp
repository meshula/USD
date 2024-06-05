//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#define NUMERIC_OPERATORS
#define MOD_OPERATOR

#include "pxr/pxr.h"
#include "pxr/base/vt/typeHeaders.h"
#include "pxr/base/vt/wrapArray.h"
#include "pxr/base/tf/preprocessorUtilsLite.h"

PXR_NAMESPACE_USING_DIRECTIVE

void wrapArrayIntegral() {
    TF_PP_SEQ_FOR_EACH(VT_WRAP_ARRAY, ~,
                       VT_INTEGRAL_BUILTIN_VALUE_TYPES);
}

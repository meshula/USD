//
// Copyright 2024 Pixar
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef PXR_EXTERNAL_BOOST_PYTHON_OBJECT_FWD_HPP
# define PXR_EXTERNAL_BOOST_PYTHON_OBJECT_FWD_HPP

#include "pxr/pxr.h"

#ifndef PXR_USE_INTERNAL_BOOST_PYTHON
#include <boost/python/object_fwd.hpp>
#else

# include "pxr/external/boost/python/detail/prefix.hpp"

namespace boost { namespace python { 
namespace api
{
  class object;
}
using api::object;
}} // namespace boost::python

#endif // PXR_USE_INTERNAL_BOOST_PYTHON
#endif // PXR_EXTERNAL_BOOST_PYTHON_OBJECT_FWD_HPP

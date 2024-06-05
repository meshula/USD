//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
// GENERATED FILE.  DO NOT EDIT.
#include <boost/python/class.hpp>
#include "pxr/usd/usdContrived/tokens.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

// Helper to return a static token as a string.  We wrap tokens as Python
// strings and for some reason simply wrapping the token using def_readonly
// bypasses to-Python conversion, leading to the error that there's no
// Python type for the C++ TfToken type.  So we wrap this functor instead.
class _WrapStaticToken {
public:
    _WrapStaticToken(const TfToken* token) : _token(token) { }

    std::string operator()() const
    {
        return _token->GetString();
    }

private:
    const TfToken* _token;
};

template <typename T>
void
_AddToken(T& cls, const char* name, const TfToken& token)
{
    cls.add_static_property(name,
                            boost::python::make_function(
                                _WrapStaticToken(&token),
                                boost::python::return_value_policy<
                                    boost::python::return_by_value>(),
                                boost::mpl::vector1<std::string>()));
}

} // anonymous

void wrapUsdContrivedTokens()
{
    boost::python::class_<UsdContrivedTokensType, boost::noncopyable>
        cls("Tokens", boost::python::no_init);
    _AddToken(cls, "libraryToken1", UsdContrivedTokens->libraryToken1);
    _AddToken(cls, "libraryToken2", UsdContrivedTokens->libraryToken2);
    _AddToken(cls, "myColorFloat", UsdContrivedTokens->myColorFloat);
    _AddToken(cls, "myDouble", UsdContrivedTokens->myDouble);
    _AddToken(cls, "myFloat", UsdContrivedTokens->myFloat);
    _AddToken(cls, "myNormals", UsdContrivedTokens->myNormals);
    _AddToken(cls, "myPoints", UsdContrivedTokens->myPoints);
    _AddToken(cls, "myVaryingToken", UsdContrivedTokens->myVaryingToken);
    _AddToken(cls, "myVaryingTokenArray", UsdContrivedTokens->myVaryingTokenArray);
    _AddToken(cls, "myVelocities", UsdContrivedTokens->myVelocities);
    _AddToken(cls, "unsignedChar", UsdContrivedTokens->unsignedChar);
    _AddToken(cls, "unsignedInt", UsdContrivedTokens->unsignedInt);
    _AddToken(cls, "unsignedInt64Array", UsdContrivedTokens->unsignedInt64Array);
    _AddToken(cls, "variableTokenAllowed1", UsdContrivedTokens->variableTokenAllowed1);
    _AddToken(cls, "variableTokenAllowed2", UsdContrivedTokens->variableTokenAllowed2);
    _AddToken(cls, "variableTokenAllowed3", UsdContrivedTokens->variableTokenAllowed3);
    _AddToken(cls, "variableTokenArrayAllowed1", UsdContrivedTokens->variableTokenArrayAllowed1);
    _AddToken(cls, "variableTokenArrayAllowed2", UsdContrivedTokens->variableTokenArrayAllowed2);
    _AddToken(cls, "variableTokenArrayAllowed3", UsdContrivedTokens->variableTokenArrayAllowed3);
    _AddToken(cls, "variableTokenDefault", UsdContrivedTokens->variableTokenDefault);
    _AddToken(cls, "Base", UsdContrivedTokens->Base);
}

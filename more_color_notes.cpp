

/// A vec3f color source constructed from a model schema and returning
/// the schema's draw mode color.
///
/// Note that it is querying the drawModeColor from the schema each time,
/// so we can use the same pointer to _DisplayColorVec3fDataSource even if
/// model:drawModeColor was dirtied.
///
class _DisplayColorVec3fDataSource final : public HdVec3fDataSource
{
public:
    HD_DECLARE_DATASOURCE(_DisplayColorVec3fDataSource);

    VtValue GetValue(const Time shutterOffset) {
        return VtValue(GetTypedValue(shutterOffset));
    }

    GfVec3f GetTypedValue(const Time shutterOffset) {
        if (HdVec3fDataSourceHandle src = _schema.GetDrawModeColor()) {
            return src->GetTypedValue(shutterOffset);
        }
        return {0.18f, 0.18f, 0.18f};
    }

    bool GetContributingSampleTimesForInterval(
        Time startTime,
        Time endTime,
        std::vector<Time> * outSampleTimes)
    {
        if (HdVec3fDataSourceHandle src = _schema.GetDrawModeColor()) {
            return src->GetContributingSampleTimesForInterval(
                startTime, endTime, outSampleTimes);
        }
        
        return false;
    }

private:
    _DisplayColorVec3fDataSource(const UsdImagingGeomModelSchema schema)
      : _schema(schema)
    {
    }

    UsdImagingGeomModelSchema _schema;
};





/*virtual*/
VtValue
UsdImagingGprimAdapter::Get(UsdPrim const& prim,
                            SdfPath const& cachePath,
                            TfToken const& key,
                            UsdTimeCode time,
                            VtIntArray *outIndices) const
{
    TRACE_FUNCTION();
    HF_MALLOC_TAG_FUNCTION();

    VtValue value;
    UsdGeomGprim gprim(prim);
    if (!TF_VERIFY(gprim)) {
        return value;
    }

    if (key == HdTokens->displayColor) {
        // First we try to obtain color from the prim, 
        // if not present, we try to get if through inheritance,
        // and lastly, we use a fallback value.
        TfToken interp;
        if (GetColor(prim, time, &interp, &value, outIndices)) {
            return value;
        } 

        // Inheritance
        UsdGeomPrimvar pv = _GetInheritedPrimvar(prim, HdTokens->displayColor);
        if (outIndices) {
            if (pv && pv.Get(&value, time)) {
                pv.GetIndices(outIndices, time);
                return value;
            }
        } else if (pv && pv.ComputeFlattened(&value, time)) {
            return value;
        }

        // Fallback
        VtVec3fArray vec(1, GfVec3f(.5,.5,.5));
        value = vec;
        return value;
    } else if (key == HdTokens->displayOpacity) {
        // First we try to obtain color from the prim, 
        // if not present, we try to get if through inheritance,
        // and lastly, we use a fallback value.
        TfToken interp;
        if (GetOpacity(prim, time, &interp, &value, outIndices)) {
            return value;
        }
...

}



/* static */
bool
UsdImagingGprimAdapter::GetColor(UsdPrim const& prim,
                                 UsdTimeCode time,
                                 TfToken* interpolation,
                                 VtValue* color,
                                 VtIntArray *indices)
{
    ...
}



GfVec3f
UsdSkelImagingSkeletonAdapter::_GetSkeletonDisplayColor(
        const UsdPrim& prim,
        UsdTimeCode time) const
{
    HD_TRACE_FUNCTION();
    HF_MALLOC_TAG_FUNCTION();
...
}




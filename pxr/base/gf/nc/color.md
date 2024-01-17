
color input values are scene-referred, which means that the color values encoded
are just numbers that meant something in the scene they came from.

color inputs must be invertible to linear space via gamma and lift. So an srgb 
input is allowable, however an ACESCg color with a render transform is not. Such
a transformed color will be respected, but if a grade has been applied, those
colors are not going to be inveerted to the other side of the grade.

color output values are layer-referred, which means the color values are numbers
that mean something in the layer's color space

color outputs may be transformed via a gamma and lift, however general render
transformations are not allowed.

layer color is always in a linear space. This means, a space whose chromaticities,
whitepoint, and reference luminance values are indicated. No non-linearities
are preserved in a layer cs.

if a color property or attribute on a layer is overridden from another layer, reading the output of the
property or attribute will occur through a transformation from the originating
layer to the layer being interrorgated.

If a color is overridden, reading the value will involve a transformation from the originating color space to the color space of the value being interrogated.

 the key thing is that if the stronger layer direclty overrides the property's colorSpace metadata, then the weaker layer's value is not considered at all.  If the stronger layer sets colorSpace on the prim (or an ancestor prim) then the colorSpace metadata on the property is still considered the colorspace for the property, because it is "closer".  But we never need to walk through the PropertyStack to find all colorSpace opinions for a property.
 
applied api schema allows applying the attribute that names the color space and
it inherits down the naemspace

example

    token triangleSubdivisionRule = "catmullClark" (
        allowedTokens = ["catmullClark", "smooth"]
        doc = """Specifies an option to the subdivision rules for the
        Catmull-Clark scheme to try and improve undesirable artifacts when
        subdividing triangles.  Valid values are "catmullClark" for the
        standard rules (the default) and "smooth" for the improvement.

        See https://graphics.pixar.com/opensubdiv/docs/subdivision_surfaces.html#triangle-subdivision-rule""")
    
one multiapply schema declares a new colorspace

regular sg mechanisms work for custom cs is a "yellow override", ie allowed but flagged
overriding the 13 is a "red override", ie disallowed

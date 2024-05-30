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
#ifndef PXR_IMAGING_HIO_READER_H
#define PXR_IMAGING_HIO_READER_H

#include "pxr/pxr.h"
#include "pxr/imaging/hio/api.h"
#include "pxr/imaging/hio/types.h"

#include <string>

PXR_NAMESPACE_OPEN_SCOPE

// Indexing
// Note that this is designed to support:
//   - Mipmapping in all image formats (if supported).
//   - Movies/animated textures via "int frame"; this also corresponds to 
//     OpenEXR's "part".
//   - AOVs via "layerName". Our primary consideration is OpenEXR, which stores 
//     each channel individually, and has (potentially)
//     a two-part naming scheme, e.g. "z", "albedo.r, albedo.g, albedo.b", etc. 
//     For the former, layerName would be "z", and for the latter
//     layerName would be "albedo"; the rgb would be implied by the format.
struct HioImageSubImageIndex
{
    std::string layerName;
    int frame;
    int mipLevel;
};
 
// HioFormat - these are effectively used as GPU-compatible pixel layout 
// descriptions; we'll add specific layout comments to each format.
 
// Some HioFormat enumerations represent block-compressed formats.
/// @TODO Move to types.h
HIO_API
bool HioFormatIsCompressed(HioFormat fmt);

// If we're reading a sub-region of an image of a given format type, how big 
// does the buffer need to be?  Since HioFormat describes binary layout,
// this is a property only of "fmt" and "dataWindow.width, dataWindow.height".
/// @TODO Move to types.h
size_t GetSizeForDataWindow(HioFormat fmt, GfVec4i dataWindow);
 
 class HioImageReader;
using HioImageReaderSharedPtr = std::shared_ptr<class HioImageReader>;

class HioImageReader {
public:
    static HIO_API
    IsImageFileSupported(std::string const & filename);

    // Queries plugin registry for an HioImageReader implementation; if not 
    // found, queries plugin registry for an HioImage implementation
    // and constructs an HioImageReader that delegates to HioImage.
    static HIO_API
    HioImageReaderSharedPtr OpenForReading(std::string const & filename);
 
    virtual std::string const & GetFilename() const = 0;
    
    // Note this replaces GetWidth/GetHeight.
    virtual GfVec4i GetDataWindow(HioImageSubImageIndex index) const = 0;
    
    // Note, this might not match the data layout in the file (e.g. with OpenEXR
    // channels stored individually); but represents the formats the Hio plugin
    // can serialize to.  It is strongly encouraged to support at least one
    // non-compressed format.
    virtual std::vector<HioFormat> 
                   GetReadableFormats(HioImageSubImageIndex index) const = 0;
    
    virtual bool GetMetadata(TfToken const & key, VtValue * value) const = 0;
 
    // Acceptable values here are nanocolor-recognized tokens.  Note that this
    // is broken out because we want HioImage to always have a recognized
    // colorspace; if there isn't a colorspace stored in the metadata, the Hio
    // plugin author should guess based on conventions or return "raw".
    virtual TfToken GetColorSpace() const = 0;
 
    // Sub-image queries
    // Acceptable mipLevels are [0, GetNumMipLevels())
    virtual int GetNumMipLevels() const = 0;
    
    // Acceptable frames are [GetFrameRange().x, GetFrameRange().y)
    virtual GfVec2i GetFrameRange() const = 0;
    
    // Acceptable "layerName" values must be in this vector.
    virtual std::vector<std::string> GetLayerNames() const = 0;
 
    // Note: dataWindow is clipped to be a subset of GetDataWindow().  
    // "format" must be an entry in GetReadableFormats().
    // "buffer" must have size >= GetSizeForDataWindow(format, dataWindow).
    virtual bool Read(HioImageSubImageIndex index, GfVec4i dataWindow, 
                      HioFormat format, TfSpan<uint8_t> buffer) = 0;
};
 
class HioImageWriter {
public:
    static bool IsImageFileSupported(std::string const & filename);

    // Queries plugin registry for an HioImageWriter implementation; 
    // if not found, queries plugin registry for an HioImage implementation
    // and constructs an HioImageWriter that delegates to HioImage.
    static HioImageWriterSharedPtr OpenForWriting(std::string const & filename);
 
    virtual std::string const & GetFilename() const = 0;
    // Similarly to GetReadableFormats(), this represents pixel data formats that the Hio plugin can serialize from.
    virtual std::vector<HioFormat> GetWriteableFormats(HioImageSubImageIndex index) const = 0;
     
    virtual bool SetMetadata(TfToken const & key, VtValue * value) = 0;
 
    // Acceptable values here are nanocolor-recognized tokens.  Note for plugins that don't support metadata writing, this function can fail.
    virtual bool SetColorSpace(TfToken const & colorSpace) = 0;
 
    // Note that dataWindow represents the window in the image file; all of "buffer" is written.  Buffer must
    // have size >= GetSizeForDataWindow(format, dataWindow).  "format" must be in GetWriteableFormats().
    virtual bool Write(HioImageSubImageIndex index, GfVec4i dataWindow, 
                       HioFormat format, TfSpan<uint8_t> buffer) = 0;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif  // PXR_IMAGING_HIO_READER_H

//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Contributors to the OpenEXR Project.
//

#ifndef IMF_INTERNAL_DWA_HELPERS_H_HAS_BEEN_INCLUDED
#    error "only include internal_dwa_helpers.h"
#endif

#ifdef _MSC_VER
#    define EXR_DCT_ALIGN __declspec (align (_SSE_ALIGNMENT))
#else
#    define EXR_DCT_ALIGN _Alignas (_SSE_ALIGNMENT)
#endif

OPENEXR_NAMESPACE_OPEN_SCOPE

/**************************************/

typedef struct _DctCoderChannelData
{
    EXR_DCT_ALIGN float    _dctData[64];
    EXR_DCT_ALIGN uint16_t _halfZigData[64];

    uint8_t**        _rows;
    size_t           _row_alloc_count;
    size_t           _size;
    exr_pixel_type_t _type;
    uint16_t*        _dc_comp;
} DctCoderChannelData;

static void
DctCoderChannelData_construct (DctCoderChannelData* d, exr_pixel_type_t t)
{
    memset (d, 0, sizeof (DctCoderChannelData));
    d->_type = t;
}

static void
DctCoderChannelData_destroy (DctCoderChannelData* d)
{
    if (d->_rows) internal_exr_free (d->_rows);
}

static exr_result_t
DctCoderChannelData_push_row (DctCoderChannelData* d, uint8_t* r)
{
    if (d->_size == d->_row_alloc_count)
    {
        size_t    nsize = d->_size == 0 ? 16 : ((d->_size * 3) / 2);
        uint8_t** n     = (uint8_t**) internal_exr_alloc (nsize * sizeof (uint8_t*));
        if (n)
        {
            if (d->_rows)
            {
                memcpy (n, d->_rows, sizeof (uint8_t*) * d->_size);
                internal_exr_free (d->_rows);
            }
            d->_rows            = n;
            d->_row_alloc_count = nsize;
        }
        else { return EXR_ERR_OUT_OF_MEMORY; }
    }
    d->_rows[d->_size] = r;
    d->_size++;
    return EXR_ERR_SUCCESS;
}

/**************************************/

typedef struct _ChannelData
{
    exr_coding_channel_info_t* chan;
    CompressorScheme           compression;
    int                        processed;
    //
    // Incoming and outgoing data is scanline interleaved, and it's much
    // easier to operate on contiguous data.  Assuming the planare unc
    // buffer is to hold RLE data, we need to rearrange to make bytes
    // adjacent.
    //
    DctCoderChannelData _dctData;

    uint8_t* planarUncBuffer;
    uint8_t* planarUncBufferEnd;

    uint8_t* planarUncRle[4];
    uint8_t* planarUncRleEnd[4];

    exr_pixel_type_t planarUncType;
    size_t           planarUncSize;
} ChannelData;

/**************************************/

typedef struct _CscChannelSet
{
    int idx[3];
} CscChannelSet;

/**************************************/

typedef struct _CscPrefixMapItem
{
    const char* name;
    size_t      prefix_len;
    int         idx[3];
} CscPrefixMapItem;

static inline CscPrefixMapItem*
CscPrefixMap_find (
    CscPrefixMapItem* mapl, size_t maxSize, const char* cname, size_t prefixlen)
{
    size_t idx = 0;
    for (; idx < maxSize; ++idx)
    {
        if (mapl[idx].name == NULL)
        {
            mapl[idx].name       = cname;
            mapl[idx].prefix_len = prefixlen;
            mapl[idx].idx[0]     = -1;
            mapl[idx].idx[1]     = -1;
            mapl[idx].idx[2]     = -1;
            break;
        }

        if (prefixlen == mapl[idx].prefix_len &&
            !strncmp (cname, mapl[idx].name, prefixlen))
        {
            break;
        }
    }

    return mapl + idx;
}

OPENEXR_NAMESPACE_CLOSE_SCOPE

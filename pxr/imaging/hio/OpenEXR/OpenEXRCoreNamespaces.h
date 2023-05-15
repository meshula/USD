
#ifndef OPENEXRCORE_NAMESPACES_H
#define OPENEXRCORE_NAMESPACES_H

#define OPENEXR_USE_NAMESPACES
#define OPENEXR_VERSION_MAJOR 3 //@OpenEXR_VERSION_MAJOR@
#define OPENEXR_VERSION_MINOR 2 //@OpenEXR_VERSION_MINOR@
#define OPENEXR_VERSION_PATCH 0 //@OpenEXR_VERSION_PATCH@

#define OPENEXR_EXTERNAL_NAMESPACE cexr
#define OPENEXR_NS OPENEXR_EXTERNAL_NAMESPACE
#define OPENEXR_INTERNAL_NAMESPACE 2308
#define OPENEXR_INTERNAL_NS exr_ ## OPENEXR_INTERNAL_NAMESPACE ## _reserved
#define OPENEXR_NS_GLOBAL ::OPENEXR_NS

namespace OPENEXR_INTERNAL_NS { }

// The root level namespace for all source in OpenEXRCore
namespace OPENEXR_NS {
    using namespace OPENEXR_INTERNAL_NS;
}

#define OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_ENTER   namespace OPENEXR_INTERNAL_NS {
#define OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_EXIT  }

#endif

#include <Foundation/Platform/PlatformDesc.h>

plPlatformDesc g_PlatformDescWeb("Web");

#if PL_ENABLED(PL_PLATFORM_WEB)

const plPlatformDesc* plPlatformDesc::s_pThisPlatform = &g_PlatformDescWeb;

#endif

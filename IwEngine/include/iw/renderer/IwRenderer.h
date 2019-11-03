#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWRENDERER_API __declspec(dllexport)
#else
#		define IWRENDERER_API __declspec(dllimport)
#	endif
#endif

namespace IW {
inline namespace RenderAPI {
	enum MeshTopology {
		POINTS = 1,
		LINES = 2,
		TRIANGLES = 3,
		QUADS = 4
	};

	// also 16 bit versions I guess

	enum TextureFormat {
		ALPHA = 1,
		RGB = 3,
		RGBA = 4,
		DEPTH,
		STENCIL
	};
}
}

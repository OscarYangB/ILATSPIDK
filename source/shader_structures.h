#pragma once
#include "definitions.h"

struct ShaderFormatData {
	const u8* data{};
	size_t size{};
};

struct ShaderData {
	ShaderFormatData spv{};
	ShaderFormatData msl{};
	ShaderFormatData dxil{};
};

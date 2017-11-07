#pragma once

// RENDER_TARGET_NONE = -1

static const char *g_RenderTargetNames[] =
{
	"RENDER_TARGET_FRAMEBUFFER",
	"RENDER_TARGET_MAIN",
	"RENDER_TARGET_MAIN_COPY",
	"RENDER_TARGET_MAIN_ONLY_ALPHA",
	"RENDER_TARGET_NORMAL_TAAMASK_SSRMASK",
	"RENDER_TARGET_NORMAL_TAAMASK_SSRMASK_SWAP",
	"RENDER_TARGET_NORMAL_TAAMASK_SSRMASK_DOWNSAMPLED",
	"RENDER_TARGET_MOTION_VECTOR",
	"RENDER_TARGET_WATER_DISPLACEMENT",
	"RENDER_TARGET_WATER_DISPLACEMENT_SWAP",
	"RENDER_TARGET_WATER_REFLECTIONS",
	"RENDER_TARGET_WATER_FLOW",
	"RENDER_TARGET_UNDERWATER_MASK",
	"RENDER_TARGET_REFRACTION_NORMALS",
	"RENDER_TARGET_MENUBG",
	"RENDER_TARGET_PLAYER_FACEGEN_TINT",
	"RENDER_TARGET_LOCAL_MAP",
	"RENDER_TARGET_LOCAL_MAP_SWAP",
	"RENDER_TARGET_SHADOW_MASK",
	"RENDER_TARGET_GETHIT_BUFFER",
	"RENDER_TARGET_GETHIT_BLURSWAP",
	"RENDER_TARGET_BLURFULL_BUFFER",
	"RENDER_TARGET_HDR_BLURSWAP",
	"RENDER_TARGET_LDR_BLURSWAP",
	"RENDER_TARGET_HDR_BLOOM",
	"RENDER_TARGET_LDR_DOWNSAMPLE0",
	"RENDER_TARGET_HDR_DOWNSAMPLE0",
	"RENDER_TARGET_HDR_DOWNSAMPLE1",
	"RENDER_TARGET_HDR_DOWNSAMPLE2",
	"RENDER_TARGET_HDR_DOWNSAMPLE3",
	"RENDER_TARGET_HDR_DOWNSAMPLE4",
	"RENDER_TARGET_HDR_DOWNSAMPLE5",
	"RENDER_TARGET_HDR_DOWNSAMPLE6",
	"RENDER_TARGET_HDR_DOWNSAMPLE7",
	"RENDER_TARGET_HDR_DOWNSAMPLE8",
	"RENDER_TARGET_HDR_DOWNSAMPLE9",
	"RENDER_TARGET_HDR_DOWNSAMPLE10",
	"RENDER_TARGET_HDR_DOWNSAMPLE11",
	"RENDER_TARGET_HDR_DOWNSAMPLE12",
	"RENDER_TARGET_HDR_DOWNSAMPLE13",
	"RENDER_TARGET_LENSFLAREVIS",
	"RENDER_TARGET_IMAGESPACE_TEMP_COPY",
	"RENDER_TARGET_IMAGESPACE_TEMP_COPY2",
	"RENDER_TARGET_IMAGESPACE_VOLUMETRIC_LIGHTING",
	"RENDER_TARGET_IMAGESPACE_VOLUMETRIC_LIGHTING_PREVIOUS",
	"RENDER_TARGET_IMAGESPACE_VOLUMETRIC_LIGHTING_COPY",
	"RENDER_TARGET_SAO",
	"RENDER_TARGET_SAO_DOWNSCALED",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_0_ESRAM",
	"RENDER_TARGET_SAO_CAMERAZ",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_0",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_1",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_2",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_3",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_4",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_5",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_6",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_7",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_8",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_9",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_10",
	"RENDER_TARGET_SAO_CAMERAZ_MIP_LEVEL_11",
	"RENDER_TARGET_SAO_RAWAO",
	"RENDER_TARGET_SAO_RAWAO_DOWNSCALED",
	"RENDER_TARGET_SAO_RAWAO_PREVIOUS",
	"RENDER_TARGET_SAO_RAWAO_PREVIOUS_DOWNSCALED",
	"RENDER_TARGET_SAO_TEMP_BLUR",
	"RENDER_TARGET_SAO_TEMP_BLUR_DOWNSCALED",
	"RENDER_TARGET_INDIRECT",
	"RENDER_TARGET_INDIRECT_DOWNSCALED",
	"RENDER_TARGET_RAWINDIRECT",
	"RENDER_TARGET_RAWINDIRECT_DOWNSCALED",
	"RENDER_TARGET_RAWINDIRECT_PREVIOUS",
	"RENDER_TARGET_RAWINDIRECT_PREVIOUS_DOWNSCALED",
	"RENDER_TARGET_RAWINDIRECT_SWAP",
	"RENDER_TARGET_SAVE_GAME_SCREENSHOT",
	"RENDER_TARGET_SCREENSHOT",
	"RENDER_TARGET_VOLUMETRIC_LIGHTING_HALF_RES",
	"RENDER_TARGET_VOLUMETRIC_LIGHTING_BLUR_HALF_RES",
	"RENDER_TARGET_VOLUMETRIC_LIGHTING_QUARTER_RES",
	"RENDER_TARGET_VOLUMETRIC_LIGHTING_BLUR_QUARTER_RES",
	"RENDER_TARGET_TEMPORAL_AA_ACCUMULATION_1",
	"RENDER_TARGET_TEMPORAL_AA_ACCUMULATION_2",
	"RENDER_TARGET_TEMPORAL_AA_UI_ACCUMULATION_1",
	"RENDER_TARGET_TEMPORAL_AA_UI_ACCUMULATION_2",
	"RENDER_TARGET_TEMPORAL_AA_MASK",
	"RENDER_TARGET_TEMPORAL_AA_WATER_1",
	"RENDER_TARGET_TEMPORAL_AA_WATER_2",
	"RENDER_TARGET_RAW_WATER",
	"RENDER_TARGET_WATER_1",
	"RENDER_TARGET_WATER_2",
	"RENDER_TARGET_IBLENSFLARES_LIGHTS_FILTER",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_4X_4X_PING",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_4X_4X_PONG",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_4Y_PING",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_4Y_PONG",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_4Y_BLUR",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_4Y_BLUR_SWAP",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_32X_4Y_PING",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_32X_4Y_PONG",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_32X_4Y_BLUR",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_32X_4Y_BLUR_SWAP",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_16X_PING",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_16X_PONG",
	"RENDER_TARGET_IBLENSFLARES_DOWNSAMPLE_16X_16X_SWAP",
	"RENDER_TARGET_BOOK_TEXT_0",
	"RENDER_TARGET_BOOK_TEXT_1",
	"RENDER_TARGET_BOOK_TEXT_2",
	"RENDER_TARGET_BOOK_TEXT_3",
	"RENDER_TARGET_SSR",
	"RENDER_TARGET_SSR_RAW",
	"RENDER_TARGET_SSR_BLURRED0",
	"RENDER_TARGET_SNOW_SPECALPHA",
	"RENDER_TARGET_SNOW_SWAP",
	"RENDER_TARGET_CUBEMAP_REFLECTIONS"
};
#include "pch.h"

#include <globals.h>
#include <rendpipeline.h>
#include <matrix4.h>
#include <xform.h>
#include "FrontierX/RenderSmokeScene.h"

#include <stdio.h>
#include <string.h>

namespace
{
bool CapturePresentedViewport(const char * filename)
{
	RECT client;
	POINT origin = {0, 0};
	if (!hMainWindow || !GetClientRect(hMainWindow, &client) ||
		client.right < 64 || client.bottom < 64 ||
		!ClientToScreen(hMainWindow, &origin)) return false;
	HDC screen = GetDC(NULL);
	if (!screen) return false;
	HDC memory = CreateCompatibleDC(screen);
	BITMAPINFO bitmapInfo = {};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = 64;
	bitmapInfo.bmiHeader.biHeight = -64;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	void * pixels = NULL;
	HBITMAP bitmap = memory ? CreateDIBSection(screen, &bitmapInfo,
		DIB_RGB_COLORS, &pixels, NULL, 0) : NULL;
	if (!bitmap || !pixels)
	{
		if (bitmap) DeleteObject(bitmap);
		if (memory) DeleteDC(memory);
		ReleaseDC(NULL, screen);
		return false;
	}
	HGDIOBJ previous = SelectObject(memory, bitmap);
	const bool copied = BitBlt(memory, 0, 0, 64, 64, screen,
		origin.x, origin.y, SRCCOPY | CAPTUREBLT) != 0;
	GdiFlush();
	bool saved = false;
	if (copied)
	{
		FILE * output = fopen(filename, "wb");
		if (output)
		{
			saved = fprintf(output, "P6\n64 64\n255\n") == 13;
			for (int y = 0; saved && y < 64; ++y)
			{
				const unsigned char * row = static_cast<const unsigned char *>(pixels) + y * 64 * 4;
				unsigned char rgb[64 * 3];
				for (int x = 0; x < 64; ++x)
				{
					rgb[x * 3 + 0] = row[x * 4 + 2];
					rgb[x * 3 + 1] = row[x * 4 + 1];
					rgb[x * 3 + 2] = row[x * 4 + 0];
				}
				saved = fwrite(rgb, 1, sizeof(rgb), output) == sizeof(rgb);
			}
			if (fclose(output) != 0) saved = false;
		}
	}
	SelectObject(memory, previous);
	DeleteObject(bitmap);
	DeleteDC(memory);
	ReleaseDC(NULL, screen);
	if (!saved) remove(filename);
	return saved;
}
}

// Runs the shared repository fixture through the supported Win32 D3D9 pipeline.
// This explicit validation path is selected before entering the normal game loop.
bool RunWindowsRenderFixtureCapture(const char * filename)
{
	if (!PIPE || !filename || !filename[0]) return false;
	remove(filename);
	FILE * trace = fopen("debug/render_fixture_steps.txt", "w");
	#define FIXTURE_STEP(name, expression) do { \
		const GENRESULT result = (expression); \
		if (trace) { fprintf(trace, "%s=0x%08X\n", name, (unsigned int)result); fflush(trace); } \
		if (result != GR_OK) goto Done; \
	} while (0)

	const PixelFormat rgba(PF_RGBA8);
	U32 texture = 0;
	bool success = false;
	bool sceneStarted = false;
	Transform modelview;
	modelview.d[2][2] = -1.0f; // D3D9's pipeline modelview adapter already flips Z.
	Matrix4 projection(true);
	FIXTURE_STEP("create_texture", PIPE->create_texture(
		FrontierX::RenderSmokeTextureWidth,
		FrontierX::RenderSmokeTextureHeight, rgba, 1, 0, texture));
	if (!texture) goto Done;
	FIXTURE_STEP("set_texture_level_data", PIPE->set_texture_level_data(texture, 0,
			FrontierX::RenderSmokeTextureWidth,
			FrontierX::RenderSmokeTextureHeight,
			FrontierX::RenderSmokeTextureWidth * 4, rgba,
			FrontierX::RenderSmokeTextureRgba.data(), 0, 0));
	FIXTURE_STEP("set_viewport", PIPE->set_viewport(0, 0, 64, 64));
	FIXTURE_STEP("set_modelview", PIPE->set_modelview(modelview));
	FIXTURE_STEP("set_projection", PIPE->set_projection(projection));
	FIXTURE_STEP("cull_mode", PIPE->set_render_state(D3DRS_CULLMODE, D3DCULL_NONE));
	FIXTURE_STEP("lighting", PIPE->set_render_state(D3DRS_LIGHTING, FALSE));
	FIXTURE_STEP("min_filter", PIPE->set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
	FIXTURE_STEP("mag_filter", PIPE->set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
	FIXTURE_STEP("address_u", PIPE->set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	FIXTURE_STEP("address_v", PIPE->set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
	FIXTURE_STEP("configure_fixture", FrontierX::ConfigureRenderSmokeState(PIPE));
	FIXTURE_STEP("clear_color", PIPE->set_pipeline_state(RP_CLEAR_COLOR, 0xFF060911));
	FIXTURE_STEP("bind_texture", PIPE->set_texture_stage_texture(0, texture));
	FIXTURE_STEP("unbind_stage_one", PIPE->set_texture_stage_texture(1, 0));
	FIXTURE_STEP("clear", PIPE->clear_buffers(RP_CLEAR_COLOR_BIT | RP_CLEAR_DEPTH_BIT, 0));
	FIXTURE_STEP("begin_scene", PIPE->begin_scene());
	sceneStarted = true;
	FIXTURE_STEP("draw", FrontierX::SubmitRenderSmokeScene(PIPE));
	FIXTURE_STEP("end_scene", PIPE->end_scene());
	sceneStarted = false;
	FIXTURE_STEP("present", PIPE->swap_buffers());
	SetForegroundWindow(hMainWindow);
	Sleep(200);
	success = CapturePresentedViewport(filename);
	if (trace) { fprintf(trace, "capture=%u win32=%lu\n", success, GetLastError()); fflush(trace); }

Done:
	if (sceneStarted) PIPE->end_scene();
	PIPE->set_texture_stage_texture(0, 0);
	if (texture) PIPE->destroy_texture(texture);
	if (trace) fclose(trace);
	#undef FIXTURE_STEP
	return success;
}

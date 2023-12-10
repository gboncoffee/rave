#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <alloca.h>
#include <math.h>
#include "raylib.h"

uint64_t *g_frames;
uint64_t *g_frames_d;
uint64_t *render_lock = NULL;
uint64_t *callback_lock = NULL;
unsigned int cap;
int g_frames_count = 0;

void callback(void *buffer, unsigned int frames)
{
	if (frames == 0)
		return;

	while (render_lock == g_frames_d);
	callback_lock = g_frames_d;

	if (cap - g_frames_count >= frames) {
		memcpy(g_frames_d, g_frames, g_frames_count * sizeof(uint64_t));
		memcpy(g_frames_d + g_frames_count, buffer, frames * sizeof(uint64_t));
		g_frames_count += frames;
	} else if (cap >= frames) {
		memcpy(g_frames_d, g_frames + frames, (cap - frames) * sizeof(uint64_t));
		memcpy(g_frames_d + (cap - frames), buffer, frames * sizeof(uint64_t));
	} else {
		memcpy(g_frames_d, buffer, cap * sizeof(uint64_t));
		g_frames_count = cap;
	}

	uint64_t *tmp = g_frames;
	g_frames = g_frames_d;
	g_frames_d = tmp;

	callback_lock = NULL;
}

void render(uint64_t *buf, int fcount)
{
	int width = GetRenderWidth();
	int height = GetRenderHeight();

	float w = (((float) width) / (float) fcount);

	while (callback_lock == buf);
	render_lock = buf;

	for (int i = 0; i < (fcount * 2); i ++) {
		float sample = ((float *) buf)[i];

		int h = (int) (((float) height / 2) * sample);

		if (sample > 0) {
			DrawRectangle(i * w, height / 2 - h, 1, h / 5 + 1, BLACK);
		} else if (sample < 0) {
			h = -h;
			DrawRectangle(i * w, height / 2 + h - (h / 5 + 1), 1, h / 5 + 1, BLACK);
		}
	}

	render_lock = NULL;
}

void usage()
{
	printf("usage: rave <music file> [-h<height in pixels>] [-w<width in pixels>] [-b<buffer size in long words>] [-f<fps>]\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		return 1;
	}

	int buffer_size = 4800;
	int height = 800;
	int width = 1200;
	int fps = 60;
	for (int i = 2; i < argc; i++) {
		if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
			char d1, d2;
			int *parse;
			switch (argv[i][1]) {
			case 'h':
				parse = &height;
				break;
			case 'w':
				parse = &width;
				break;
			case 'b':
				parse = &buffer_size;
				break;
			case 'f':
				parse = &fps;
				break;
			default:
				usage();
				return 1;
			}

			if (sscanf(argv[i], "%c%c%d", &d1, &d2, parse) != 3) {
				usage();
				return 1;
			}
		}
	}

	InitWindow(width, height, "Raylib Audio Visualizer");
	SetTargetFPS(fps);

	cap = buffer_size;
	g_frames = alloca(buffer_size * sizeof(uint64_t));
	g_frames_d = alloca(buffer_size * sizeof(uint64_t));

	for (unsigned int i = 0; i < cap; i++) {
		g_frames[i] = 0;
		g_frames_d[i] = 0;
	}

	InitAudioDevice();
	Music s = LoadMusicStream(argv[1]);
	assert(s.stream.sampleSize == 32);
	assert(s.stream.channels == 2);
	PlayMusicStream(s);
	AttachAudioStreamProcessor(s.stream, callback);

	while (!WindowShouldClose()) {
		UpdateMusicStream(s);

		if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
			CloseWindow();
		} else {
			BeginDrawing();
			ClearBackground(WHITE);

			if (g_frames_count != 0)
				render(g_frames, g_frames_count);

			EndDrawing();
		}
	}
	DetachAudioStreamProcessor(s.stream, callback);
	StopMusicStream(s);

	return 0;
}

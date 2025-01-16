#include <assert.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile uint64_t *g_frames;
volatile uint64_t *g_frames_d;
volatile unsigned int cap;
volatile int g_frames_count = 0;

/* Black */
static const Color background = CLITERAL(Color){0x00, 0x00, 0x00, 0xff};

/* White */
static const Color foreground = CLITERAL(Color){0xff, 0xff, 0xff, 0xff};

void callback(void *buffer, unsigned int frames) {
  if (frames == 0) return;

  if (cap - g_frames_count >= frames) {
    memcpy((void *)g_frames_d, (void *)g_frames,
           g_frames_count * sizeof(uint64_t));
    memcpy((void *)(g_frames_d + g_frames_count), buffer,
           frames * sizeof(uint64_t));
    g_frames_count += frames;
  } else if (cap >= frames) {
    memcpy((void *)g_frames_d, (void *)(g_frames + frames),
           (cap - frames) * sizeof(uint64_t));
    memcpy((void *)(g_frames_d + (cap - frames)), buffer,
           frames * sizeof(uint64_t));
  } else {
    memcpy((void *)g_frames_d, buffer, cap * sizeof(uint64_t));
    g_frames_count = cap;
  }

  volatile uint64_t *tmp = g_frames;
  g_frames = g_frames_d;
  g_frames_d = tmp;
}

void render(volatile uint64_t *buf, int fcount) {
  int width = GetRenderWidth();
  int height = GetRenderHeight();

  float w = (((float)width) / (float)fcount);

  for (int i = 0; i < (fcount * 2); i++) {
    float sample = ((float *)buf)[i];

    int h = (int)(((float)height / 2) * sample);

    if (sample > 0) {
      DrawRectangle(i * w, height / 2 - h, 1, h / 5 + 1, foreground);
    } else if (sample < 0) {
      h = -h;
      DrawRectangle(i * w, height / 2 + h - (h / 5 + 1), 1, h / 5 + 1,
                    foreground);
    }
  }
}

void usage(void) {
  printf(
      "usage: rave [music file] [-h<height in pixels>] [-w<width in pixels>] "
      "[-b<buffer size in long words>] [-f<fps>]\n");
  printf("If no music file is provided, waits for drag&drop.\n");
}

char *drag_and_drop(void) {
  while (!WindowShouldClose()) {
    if (IsFileDropped()) return LoadDroppedFiles().paths[0];

		BeginDrawing();

		ClearBackground(background);
		DrawText("Drag&Drop a music file.", 0, 0, 45, foreground);

		EndDrawing();
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  int buffer_size = 4800;
  int height = 800;
  int width = 1200;
  int fps = 60;
  char *music_file = NULL;

  for (int i = 1; i < argc; i++) {
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
    } else {
      music_file = argv[i];
    }
  }

  SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(width, height, "Raylib Audio Visualizer");
  SetTargetFPS(fps);

  cap = buffer_size;
  g_frames = malloc(buffer_size * sizeof(uint64_t));
  g_frames_d = malloc(buffer_size * sizeof(uint64_t));

  for (unsigned int i = 0; i < cap; i++) {
    g_frames[i] = 0;
    g_frames_d[i] = 0;
  }

  if (music_file == NULL) {
    music_file = drag_and_drop();
    if (music_file == NULL) {
      usage();
      return 1;
    }
  }

  InitAudioDevice();
  Music s = LoadMusicStream(music_file);
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
      ClearBackground(background);

      if (g_frames_count != 0) render(g_frames, g_frames_count);

      EndDrawing();
    }
  }
  DetachAudioStreamProcessor(s.stream, callback);
  StopMusicStream(s);

  free((void *)g_frames);
  free((void *)g_frames_d);

  return 0;
}

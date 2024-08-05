#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 900
#define HEIGHT 700
#define FPS 60
#define FONT_SIZE 16

struct Buffer {
	char* buf;
	int capacity;
	int size;
};

struct TextArea {
	Rectangle input_box;
	Rectangle border;
	Vector2 cursor;
	Vector2 padding;
	struct Buffer* buffer;
	bool is_focused;
};

struct Buffer* init_buffer(size_t capacity) {
	struct Buffer* buf;
	buf = (struct Buffer*)malloc(sizeof(struct Buffer));
	if (buf == NULL) {
		perror("memory allocation error");
		exit(1);
	}
	buf->capacity = capacity;
	buf->buf = (char*)malloc(buf->capacity + 1);
	if (buf->buf == NULL) {
		perror("memory allocation error");
		exit(1);
	}
	for (size_t i = 0; i < buf->capacity + 1; ++i) {
		buf->buf[i] = '\0';
	}
	buf->size = -1;
	return buf;
}

void append(struct Buffer* buf, char value) {
	if (buf->size < buf->capacity) {
		buf->size++;
		buf->buf[buf->size] = value;
	} else {
		buf->capacity *= 2;
		buf->buf = realloc(buf->buf, buf->capacity);
		if (buf->buf == NULL) {
			perror("reallocating memory failure\n");
			exit(1);
		}
		buf->size++;
		buf->buf[buf->size] = value;
	}
}

void delete(struct Buffer* buf) {
	if (buf->size >= 0) {
		buf->buf[buf->size] = '\0';
		buf->size--;
	}
}

int main() {
	InitWindow(WIDTH, HEIGHT, "Wheel of Luck");
	SetTargetFPS(FPS);
	Rectangle input_box = {.x = 10, .y = 10, .width = WIDTH * 0.2f, .height = HEIGHT * 0.5f};
	int gap = 2;
	// int rows = input_box.height / FONT_SIZE;
	Rectangle input_box_border = {.x = input_box.x - gap,
								  .y = input_box.y - gap,
								  .width = input_box.width + gap,
								  .height = input_box.height + gap};
	Vector2 mouse = {0};
	bool mouse_on_text = false;
	struct Buffer* buf = init_buffer(40);
	struct TextArea text_area = {
		.buffer = buf,
		.input_box = input_box,
		.border = input_box_border,
		.padding =
			{
				.x = 6,
				.y = 8,
			},
		.is_focused = false,
	};
	Vector2 input_box_cursor = {
		.x = text_area.input_box.x + text_area.padding.x,
		.y = text_area.input_box.y + text_area.padding.y,
	};
	text_area.cursor = input_box_cursor;

	while (!WindowShouldClose()) {

		int key;
		while ((key = GetCharPressed()) > 0) {
			append(text_area.buffer, (char)key);
		}
		if (IsKeyPressedRepeat(KEY_BACKSPACE)) {
			delete (text_area.buffer);
			if (text_area.cursor.x > text_area.input_box.x + text_area.padding.x) {
				text_area.cursor.x--;
			} else {
				text_area.cursor.x = text_area.input_box.x + text_area.padding.x;
				if (text_area.cursor.y < text_area.input_box.y + text_area.padding.y) {
					text_area.cursor.y = text_area.input_box.y + text_area.padding.y;
				} else {

					text_area.cursor.y -= text_area.padding.y;
				}
			}
		} else if (IsKeyPressed(KEY_BACKSPACE)) {
			delete (text_area.buffer);
			if (text_area.cursor.x > text_area.input_box.x + 6) {
				text_area.cursor.x--;
			} else {
				text_area.cursor.x = text_area.input_box.x + text_area.padding.x;
				text_area.cursor.y -= text_area.padding.y;
				if (text_area.cursor.y < text_area.input_box.y + text_area.padding.y) {
					text_area.cursor.y = text_area.input_box.y + text_area.padding.y;
				} else {
					text_area.cursor.y -= text_area.padding.y;
				}
			}
		}
		if (IsKeyPressed(KEY_ENTER)) {
			text_area.cursor.y += FONT_SIZE;
			text_area.cursor.x = text_area.input_box.x + text_area.padding.x;
			append(text_area.buffer, '\n');
		}
		if (IsKeyPressed(KEY_Q)) {
			CloseWindow();
			return 0;
		}
		mouse = GetMousePosition();
		if (CheckCollisionPointRec(mouse, input_box)) {
			mouse_on_text = true;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				text_area.is_focused = true;
		} else {
			mouse_on_text = false;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				text_area.is_focused = false;
		}
		if (mouse_on_text)
			SetMouseCursor(MOUSE_CURSOR_IBEAM);
		else
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		BeginDrawing();
		ClearBackground(DARKGRAY);
		DrawRectangleRec(input_box, LIGHTGRAY);
		DrawRectangleLinesEx(text_area.border, gap, BLACK);
		if (text_area.is_focused) {
			DrawRectangleLinesEx(text_area.border, gap, RED);
			if ((int)GetTime() % 2 == 0) {
				text_area.cursor.x = MeasureText(text_area.buffer->buf, FONT_SIZE),
				DrawText("|", text_area.cursor.x, text_area.cursor.y, FONT_SIZE, BLACK);
			}
			DrawText(text_area.buffer->buf, text_area.input_box.x + text_area.padding.x,
					 text_area.input_box.y + text_area.padding.y, FONT_SIZE, YELLOW);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}

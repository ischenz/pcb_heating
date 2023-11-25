#ifndef __DINO_OLED_H__
#define __DINO_OLED_H__

#include "font.h"
#include "main.h"
#include "string.h"

typedef enum {
  DINO_OLED_COLOR_NORMAL = 0, // 正常模式 黑底白字
  DINO_OLED_COLOR_REVERSED    // 反色模式 白底黑字
} DINO_OLED_ColorMode;

void DINO_OLED_Init(void);
void DINO_OLED_DisPlay_On(void);
void DINO_OLED_DisPlay_Off(void);

void DINO_OLED_NewFrame(void);
void DINO_OLED_ShowFrame(void);
void DINO_OLED_SetPixel(uint8_t x, uint8_t y, DINO_OLED_ColorMode color);

void DINO_OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DINO_OLED_ColorMode color);
void DINO_OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DINO_OLED_ColorMode color);
void DINO_OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DINO_OLED_ColorMode color);
void DINO_OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r, DINO_OLED_ColorMode color);
void DINO_OLED_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, DINO_OLED_ColorMode color);
void DINO_OLED_DrawImage(uint8_t x, uint8_t y, const Image *img, DINO_OLED_ColorMode color);

void DINO_OLED_PrintASCIIChar(uint8_t x, uint8_t y, char ch, const ASCIIFont *font, DINO_OLED_ColorMode color);
void DINO_OLED_PrintASCIIString(uint8_t x, uint8_t y, char *str, const ASCIIFont *font, DINO_OLED_ColorMode color);
void DINO_OLED_PrintString(uint8_t x, uint8_t y, char *str, const Font *font, DINO_OLED_ColorMode color);

#endif // __DINO_OLED_H__


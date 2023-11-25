/**
 * @file oled.c
 * @brief 波特律动OLED驱动(CH1116)
 * @anchor 波特律动(keysking 博哥在学习)
 * @version 1.0
 * @date 2023-08-04
 * @license MIT License
 *
 * @attention
 * 本驱动库针对波特律动·keysking的STM32教程学习套件进行开发
 * 在其他平台或驱动芯片上使用可能需要进行移植
 *
 * 为保证中文显示正常 请将编译器的字符集设置为UTF-8
 *
 */
#include "DinoOled.h"
#include "i2c.h"
#include <math.h>
#include <stdlib.h>

// OLED器件地址
#define DINO_OLED_ADDRESS 0x78

// OLED参数
#define DINO_OLED_PAGE 8            // OLED页数
#define DINO_OLED_ROW 8 * DINO_OLED_PAGE // OLED行数
#define DINO_OLED_COLUMN 128        // OLED列数

// 显存
uint8_t DINO_OLED_GRAM[DINO_OLED_PAGE][DINO_OLED_COLUMN];

// ========================== 底层通信函数 ==========================

/**
 * @brief 向OLED发送数据的函数
 * @param data 要发送的数据
 * @param len 要发送的数据长度
 * @return None
 * @note 此函数是移植本驱动时的重要函数 将本驱动库移植到其他平台时应根据实际情况修改此函数
 */
void Send(uint8_t *data, uint8_t len) {
  HAL_I2C_Master_Transmit(&hi2c1, DINO_OLED_ADDRESS, data, len, HAL_MAX_DELAY);
}

/**
 * @brief 向OLED发送指令
 */
void DINO_OLED_SendCmd(uint8_t data) {
  uint8_t send_buf[2] = {0};
  send_buf[0] = 0x00;
  send_buf[1] = data;
  Send(send_buf, 2);
}

// ========================== OLED驱动函数 ==========================

/**
 * @brief 初始化OLED
 * @note 此函数是移植本驱动时的重要函数 将本驱动库移植到其他驱动芯片时应根据实际情况修改此函数
 */
void DINO_OLED_Init() {
  DINO_OLED_SendCmd(0xAE); /*关闭显示 display off*/

  DINO_OLED_SendCmd(0x02); /*设置列起始地址 set lower column address*/
  DINO_OLED_SendCmd(0x10); /*设置列结束地址 set higher column address*/

  DINO_OLED_SendCmd(0x40); /*设置起始行 set display start line*/

  DINO_OLED_SendCmd(0xB0); /*设置页地址 set page address*/

  DINO_OLED_SendCmd(0x81); /*设置对比度 contract control*/
  DINO_OLED_SendCmd(0xCF); /*128*/

  DINO_OLED_SendCmd(0xA1); /*设置分段重映射 从右到左 set segment remap*/

  DINO_OLED_SendCmd(0xA6); /*正向显示 normal / reverse*/

  DINO_OLED_SendCmd(0xA8); /*多路复用率 multiplex ratio*/
  DINO_OLED_SendCmd(0x3F); /*duty = 1/64*/

  DINO_OLED_SendCmd(0xAD); /*设置启动电荷泵 set charge pump enable*/
  DINO_OLED_SendCmd(0x8B); /*启动DC-DC */

  DINO_OLED_SendCmd(0x33); /*设置泵电压 set VPP 10V */

  DINO_OLED_SendCmd(0xC8); /*设置输出扫描方向 COM[N-1]到COM[0] Com scan direction*/

  DINO_OLED_SendCmd(0xD3); /*设置显示偏移 set display offset*/
  DINO_OLED_SendCmd(0x00); /* 0x00 */

  DINO_OLED_SendCmd(0xD5); /*设置内部时钟频率 set osc frequency*/
  DINO_OLED_SendCmd(0xC0);

  DINO_OLED_SendCmd(0xD9); /*设置放电/预充电时间 set pre-charge period*/
  DINO_OLED_SendCmd(0x1F); /*0x22*/

  DINO_OLED_SendCmd(0xDA); /*设置引脚布局 set COM pins*/
  DINO_OLED_SendCmd(0x12);

  DINO_OLED_SendCmd(0xDB); /*设置电平 set vcomh*/
  DINO_OLED_SendCmd(0x40);

  DINO_OLED_NewFrame();
  DINO_OLED_ShowFrame();

  DINO_OLED_SendCmd(0xAF); /*开启显示 display ON*/
}

/**
 * @brief 开启OLED显示
 */
void DINO_OLED_DisPlay_On() {
  DINO_OLED_SendCmd(0x8D); // 电荷泵使能
  DINO_OLED_SendCmd(0x14); // 开启电荷泵
  DINO_OLED_SendCmd(0xAF); // 点亮屏幕
}

/**
 * @brief 关闭OLED显示
 */
void DINO_OLED_DisPlay_Off() {
  DINO_OLED_SendCmd(0x8D); // 电荷泵使能
  DINO_OLED_SendCmd(0x10); // 关闭电荷泵
  DINO_OLED_SendCmd(0xAE); // 关闭屏幕
}

/**
 * @brief 设置颜色模式 黑底白字或白底黑字
 * @param ColorMode 颜色模式COLOR_NORMAL/COLOR_REVERSED
 * @note 此函数直接设置屏幕的颜色模式
 */
void DINO_OLED_SetColorMode(DINO_OLED_ColorMode mode) {
  if (mode == DINO_OLED_COLOR_NORMAL) {
    DINO_OLED_SendCmd(0xA6); // 正常显示
  }
  if (mode == DINO_OLED_COLOR_REVERSED) {
    DINO_OLED_SendCmd(0xA7); // 反色显示
  }
}

// ========================== 显存操作函数 ==========================

/**
 * @brief 清空显存 绘制新的一帧
 */
void DINO_OLED_NewFrame() {
  memset(DINO_OLED_GRAM, 0, sizeof(DINO_OLED_GRAM));
}

/**
 * @brief 将当前显存显示到屏幕上
 * @note 此函数是移植本驱动时的重要函数 将本驱动库移植到其他驱动芯片时应根据实际情况修改此函数
 */
void DINO_OLED_ShowFrame() {
  static uint8_t send_buf[DINO_OLED_COLUMN + 1];
  send_buf[0] = 0x40;
  for (uint8_t i = 0; i < DINO_OLED_PAGE; i++) {
    DINO_OLED_SendCmd(0xB0 + i); // 设置页地址
    DINO_OLED_SendCmd(0x02);     // 设置列地址低4位
    DINO_OLED_SendCmd(0x10);     // 设置列地址高4位
    memcpy(send_buf + 1, DINO_OLED_GRAM[i], DINO_OLED_COLUMN);
    Send(send_buf, DINO_OLED_COLUMN + 1);
  }
}

/**
 * @brief 设置一个像素点
 * @param x 横坐标
 * @param y 纵坐标
 * @param color 颜色
 */
void DINO_OLED_SetPixel(uint8_t x, uint8_t y, DINO_OLED_ColorMode color) {
  if (x >= DINO_OLED_COLUMN || y >= DINO_OLED_ROW) return;
  if (!color) {
    DINO_OLED_GRAM[y / 8][x] |= 1 << (y % 8);
  } else {
    DINO_OLED_GRAM[y / 8][x] &= ~(1 << (y % 8));
  }
}

/**
 * @brief 设置显存中一字节数据的某几位
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param start 起始位
 * @param end 结束位
 * @param color 颜色
 * @note 此函数将显存中的某一字节的第start位到第end位设置为与data相同
 * @note start和end的范围为0-7, start必须小于等于end
 * @note 此函数与DINO_OLED_SetByte_Fine的区别在于此函数只能设置显存中的某一真实字节
 */
void DINO_OLED_SetByte_Fine(uint8_t page, uint8_t column, uint8_t data, uint8_t start, uint8_t end, DINO_OLED_ColorMode color) {
  static uint8_t temp;
  if (page >= DINO_OLED_PAGE || column >= DINO_OLED_COLUMN) return;
  if (color) data = ~data;

  temp = data | (0xff << (end + 1)) | (0xff >> (8 - start));
  DINO_OLED_GRAM[page][column] &= temp;
  temp = data & ~(0xff << (end + 1)) & ~(0xff >> (8 - start));
  DINO_OLED_GRAM[page][column] |= temp;
  // 使用DINO_OLED_SetPixel实现
  // for (uint8_t i = start; i <= end; i++) {
  //   DINO_OLED_SetPixel(column, page * 8 + i, !((data >> i) & 0x01));
  // }
}

/**
 * @brief 设置显存中的一字节数据
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param color 颜色
 * @note 此函数将显存中的某一字节设置为data的值
 */
void DINO_OLED_SetByte(uint8_t page, uint8_t column, uint8_t data, DINO_OLED_ColorMode color) {
  if (page >= DINO_OLED_PAGE || column >= DINO_OLED_COLUMN) return;
  if (color) data = ~data;
  DINO_OLED_GRAM[page][column] = data;
}

/**
 * @brief 设置显存中的一字节数据的某几位
 * @param x 横坐标
 * @param y 纵坐标
 * @param data 数据
 * @param len 位数
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始向下数len位设置为与data相同
 * @note len的范围为1-8
 * @note 此函数与DINO_OLED_SetByte_Fine的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
 */
void DINO_OLED_SetBits_Fine(uint8_t x, uint8_t y, uint8_t data, uint8_t len, DINO_OLED_ColorMode color) {
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  if (bit + len > 8) {
    DINO_OLED_SetByte_Fine(page, x, data << bit, bit, 7, color);
    DINO_OLED_SetByte_Fine(page + 1, x, data >> (8 - bit), 0, len + bit - 1 - 8, color);
  } else {
    DINO_OLED_SetByte_Fine(page, x, data << bit, bit, bit + len - 1, color);
  }
  // 使用DINO_OLED_SetPixel实现
  // for (uint8_t i = 0; i < len; i++) {
  //   DINO_OLED_SetPixel(x, y + i, !((data >> i) & 0x01));
  // }
}

/**
 * @brief 设置显存中一字节长度的数据
 * @param x 横坐标
 * @param y 纵坐标
 * @param data 数据
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始向下数8位设置为与data相同
 * @note 此函数与DINO_OLED_SetByte的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
 */
void DINO_OLED_SetBits(uint8_t x, uint8_t y, uint8_t data, DINO_OLED_ColorMode color) {
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  DINO_OLED_SetByte_Fine(page, x, data << bit, bit, 7, color);
  if (bit) {
    DINO_OLED_SetByte_Fine(page + 1, x, data >> (8 - bit), 0, bit - 1, color);
  }
}

/**
 * @brief 设置一块显存区域
 * @param x 起始横坐标
 * @param y 起始纵坐标
 * @param data 数据的起始地址
 * @param w 宽度
 * @param h 高度
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始的w*h个像素设置为data中的数据
 * @note data的数据应该采用列行式排列
 */
void DINO_OLED_SetBlock(uint8_t x, uint8_t y, const uint8_t *data, uint8_t w, uint8_t h, DINO_OLED_ColorMode color) {
  uint8_t fullRow = h / 8; // 完整的行数
  uint8_t partBit = h % 8; // 不完整的字节中的有效位数
  for (uint8_t i = 0; i < w; i++) {
    for (uint8_t j = 0; j < fullRow; j++) {
      DINO_OLED_SetBits(x + i, y + j * 8, data[i + j * w], color);
    }
  }
  if (partBit) {
    uint16_t fullNum = w * fullRow; // 完整的字节数
    for (uint8_t i = 0; i < w; i++) {
      DINO_OLED_SetBits_Fine(x + i, y + (fullRow * 8), data[fullNum + i], partBit, color);
    }
  }
  // 使用DINO_OLED_SetPixel实现
  // for (uint8_t i = 0; i < w; i++) {
  //   for (uint8_t j = 0; j < h; j++) {
  //     for (uint8_t k = 0; k < 8; k++) {
  //       if (j * 8 + k >= h) break; // 防止越界(不完整的字节
  //       DINO_OLED_SetPixel(x + i, y + j * 8 + k, !((data[i + j * w] >> k) & 0x01));
  //     }
  //   }
  // }
}

// ========================== 图形绘制函数 ==========================
/**
 * @brief 绘制一条线段
 * @param x1 起始点横坐标
 * @param y1 起始点纵坐标
 * @param x2 终止点横坐标
 * @param y2 终止点纵坐标
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制线段
 */
void DINO_OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DINO_OLED_ColorMode color) {
  static uint8_t temp = 0;
  if (x1 == x2) {
    if (y1 > y2) {
      temp = y1;
      y1 = y2;
      y2 = temp;
    }
    for (uint8_t y = y1; y <= y2; y++) {
      DINO_OLED_SetPixel(x1, y, color);
    }
  } else if (y1 == y2) {
    if (x1 > x2) {
      temp = x1;
      x1 = x2;
      x2 = temp;
    }
    for (uint8_t x = x1; x <= x2; x++) {
      DINO_OLED_SetPixel(x, y1, color);
    }
  } else {
    // Bresenham直线算法
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    int16_t ux = ((dx > 0) << 1) - 1;
    int16_t uy = ((dy > 0) << 1) - 1;
    int16_t x = x1, y = y1, eps = 0;
    dx = abs(dx);
    dy = abs(dy);
    if (dx > dy) {
      for (x = x1; x != x2; x += ux) {
        DINO_OLED_SetPixel(x, y, color);
        eps += dy;
        if ((eps << 1) >= dx) {
          y += uy;
          eps -= dx;
        }
      }
    } else {
      for (y = y1; y != y2; y += uy) {
        DINO_OLED_SetPixel(x, y, color);
        eps += dx;
        if ((eps << 1) >= dy) {
          x += ux;
          eps -= dy;
        }
      }
    }
  }
}

/**
 * @brief 绘制一个矩形
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param color 颜色
 */
void DINO_OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DINO_OLED_ColorMode color) {
  DINO_OLED_DrawLine(x, y, x + w, y, color);
  DINO_OLED_DrawLine(x, y + h, x + w, y + h, color);
  DINO_OLED_DrawLine(x, y, x, y + h, color);
  DINO_OLED_DrawLine(x + w, y, x + w, y + h, color);
}

/**
 * @brief 绘制一个填充矩形
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param color 颜色
 */
void DINO_OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DINO_OLED_ColorMode color) {
  for (uint8_t i = 0; i < h; i++) {
    DINO_OLED_DrawLine(x, y + i, x + w, y + i, color);
  }
}

/**
 * @brief 绘制一个圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 圆半径
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制圆
 */
void DINO_OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r, DINO_OLED_ColorMode color) {
  int16_t a = 0, b = r, di = 3 - (r << 1);
  while (a <= b) {
    DINO_OLED_SetPixel(x - b, y - a, color);
    DINO_OLED_SetPixel(x + b, y - a, color);
    DINO_OLED_SetPixel(x - a, y + b, color);
    DINO_OLED_SetPixel(x - b, y - a, color);
    DINO_OLED_SetPixel(x - a, y - b, color);
    DINO_OLED_SetPixel(x + b, y + a, color);
    DINO_OLED_SetPixel(x + a, y - b, color);
    DINO_OLED_SetPixel(x + a, y + b, color);
    DINO_OLED_SetPixel(x - b, y + a, color);
    a++;
    if (di < 0) {
      di += 4 * a + 6;
    } else {
      di += 10 + 4 * (a - b);
      b--;
    }
    DINO_OLED_SetPixel(x + a, y + b, color);
  }
}

/**
 * @brief 绘制一个填充圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 圆半径
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制圆
 */
void DINO_OLED_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, DINO_OLED_ColorMode color) {
  int16_t a = 0, b = r, di = 3 - (r << 1);
  while (a <= b) {
    for (int16_t i = x - b; i <= x + b; i++) {
      DINO_OLED_SetPixel(i, y + a, color);
      DINO_OLED_SetPixel(i, y - a, color);
    }
    for (int16_t i = x - a; i <= x + a; i++) {
      DINO_OLED_SetPixel(i, y + b, color);
      DINO_OLED_SetPixel(i, y - b, color);
    }
    a++;
    if (di < 0) {
      di += 4 * a + 6;
    } else {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
}

/**
 * @brief 绘制一张图片
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param img 图片
 * @param color 颜色
 */
void DINO_OLED_DrawImage(uint8_t x, uint8_t y, const Image *img, DINO_OLED_ColorMode color) {
  DINO_OLED_SetBlock(x, y, img->data, img->w, img->h, color);
}

// ================================ 文字绘制 ================================

/**
 * @brief 绘制一个ASCII字符
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param ch 字符
 * @param font 字体
 * @param color 颜色
 */
void DINO_OLED_PrintASCIIChar(uint8_t x, uint8_t y, char ch, const ASCIIFont *font, DINO_OLED_ColorMode color) {
  DINO_OLED_SetBlock(x, y, font->chars + (ch - ' ') * (((font->h + 7) / 8) * font->w), font->w, font->h, color);
}

/**
 * @brief 绘制一个ASCII字符串
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param str 字符串
 * @param font 字体
 * @param color 颜色
 */
void DINO_OLED_PrintASCIIString(uint8_t x, uint8_t y, char *str, const ASCIIFont *font, DINO_OLED_ColorMode color) {
  uint8_t x0 = x;
  while (*str) {
    DINO_OLED_PrintASCIIChar(x0, y, *str, font, color);
    x0 += font->w;
    str++;
  }
}

/**
 * @brief 获取UTF-8编码的字符长度
 */
uint8_t _DINO_OLED_GetUTF8Len(char *string) {
  if ((string[0] & 0x80) == 0x00) {
    return 1;
  } else if ((string[0] & 0xE0) == 0xC0) {
    return 2;
  } else if ((string[0] & 0xF0) == 0xE0) {
    return 3;
  } else if ((string[0] & 0xF8) == 0xF0) {
    return 4;
  }
  return 0;
}

void DINO_OLED_PrintString(uint8_t x, uint8_t y, char *str, const Font *font, DINO_OLED_ColorMode color) {
  uint16_t i = 0;                                       // 字符串索引
  uint8_t oneLen = (((font->h + 7) / 8) * font->w) + 4; // 一个字模占多少字节
  uint8_t found;                                        // 是否找到字模
  uint8_t utf8Len;                                      // UTF-8编码长度
  uint8_t *head;                                        // 字模头指针
  while (str[i]) {
    found = 0;
    utf8Len = _DINO_OLED_GetUTF8Len(str + i);
    if (utf8Len == 0) break; // 有问题的UTF-8编码

    // 寻找字符  TODO 优化查找算法, 二分查找或者hash
    for (uint8_t j = 0; j < font->len; j++) {
      head = (uint8_t *)(font->chars) + (j * oneLen);
      if (memcmp(str + i, head, utf8Len) == 0) {
        DINO_OLED_SetBlock(x, y, head + 4, font->w, font->h, color);
        // 移动光标
        x += font->w;
        i += utf8Len;
        found = 1;
        break;
      }
    }

    // 若未找到字幕,且为ASCII字符, 则缺省显示ASCII字符
    if (found == 0) {
      if (utf8Len == 1){
        DINO_OLED_PrintASCIIChar(x, y, str[i], font->ascii, color);
        // 移动光标
        x += font->ascii->w; 
        i += utf8Len;
      }else{
        DINO_OLED_PrintASCIIChar(x, y, ' ', font->ascii, color);
        x += font->ascii->w; 
        i += utf8Len;
      }
    }
  }
}

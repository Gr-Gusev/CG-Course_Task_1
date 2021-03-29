#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>
#include "common.h"

static const float TILE_COEF = 0.8;
static const int TILE_SIZE = 25;
static const int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
static const float FADE_STEP = 0.02;
static const int LIGHT_LENGTH = TILE_SIZE * 3;
static const int LIGHT_COEF = 80;

struct Point
{
  int x;
  int y;
};

struct Pixel
{
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 0;

  void lightUp(float coef);
  void makeTransparentTo(float transparency); // 0 - transparent; 1 - original
  void Add(Pixel pix);
  void Sub(Pixel pix);
  void Zero();
};

Pixel Blend(Pixel back, Pixel pix);
int Distance(Point p1, Point p2);

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);
  int getWidth()    const { return width; }
  int getHeight()   const { return height; }
  int getChannels() const { return channels; }
  size_t getSize()  const { return size; }
  Pixel* getData()        { return  data; }
  void fadeTo(float fade);  //  fade == 0 - black; fade == 1 - original image
  void copyFrom(Image *src);
  void fillInBlack();
  Pixel GetPixel(int x, int y) { return data[width * y + x]; }
  void  PutPixel(int x, int y, const Pixel &pix) { data[width * y + x] = pix; }

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};

#endif //MAIN_IMAGE_H

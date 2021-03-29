#include "Image.h"
#include <iostream>
#include "common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


Pixel Blend(Pixel back, Pixel pix) {
  Pixel ret;
  float r1, g1, b1, a1, r2, g2, b2, a2;
  r1 = (float) back.r / 255.0;
  g1 = (float) back.g / 255.0;
  b1 = (float) back.b / 255.0;
  a1 = (float) back.a / 255.0;

  r2 = (float) pix.r / 255.0;
  g2 = (float) pix.g / 255.0;
  b2 = (float) pix.b / 255.0;
  a2 = (float) pix.a / 255.0;

  ret.r = (int)((r1 * (1 - a2) + r2 * a2) * 255) % 255;
  ret.g = (int)((g1 * (1 - a2) + g2 * a2) * 255) % 255;
  ret.b = (int)((b1 * (1 - a2) + b2 * a2) * 255) % 255;
  ret.a = (int)((a1 * (1 - a2) + a2 * a2) * 255) % 255;

  return ret;
}

int Distance(Point p1, Point p2)
{
  return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

void Pixel::lightUp(float coef)
{
  if (coef <= 0)
    return;

  int delta = (int)(coef * (float)(LIGHT_COEF));
  r = MIN(255, r + delta);
  g = MIN(255, g + delta);
  b = MIN(255, b + delta);
}

void Pixel::makeTransparentTo(float transparency)
{
  if (transparency < 0)
    return;

  a *= transparency;
}

void Pixel::Add(Pixel pix)
{
  r = MIN(255, r + pix.r);
  g = MIN(255, g + pix.g);
  b = MIN(255, b + pix.b);
  a = MIN(255, a + pix.a);
}

void Pixel::Sub(Pixel pix)
{
  r = MAX(0, r - pix.r);
  g = MAX(0, g - pix.g);
  b = MAX(0, b - pix.b);
  a = MAX(0, a - pix.a);
}

void Pixel::Zero()
{
  r = 0;
  g = 0;
  b = 0;
  a = 0;
}

Image::Image(const std::string &a_path)
{
  data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 4);

  if (data != nullptr) {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height]{};

  if (data != nullptr) {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if (a_path.substr(extPos, std::string::npos) == ".png"
     || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height,
                   channels, data, width * channels);
  } else
  if (a_path.substr(extPos, std::string::npos) == ".jpg"
      || a_path.substr(extPos, std::string::npos) == ".JPG"
      || a_path.substr(extPos, std::string::npos) == ".jpeg"
      || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  } else {
    std::cerr << "Unknown file extension: ";
    std::cerr << a_path.substr(extPos, std::string::npos);
    std::cerr << "in file name" << a_path << std::endl;
    return 1;
  }

  return 0;
}

Image::~Image()
{
  if (self_allocated) {
    delete [] data;
  } else {
    stbi_image_free(data);
  }
}

void Image::fadeTo(float fade)
{
  int i_size = width * height;
  if (fade < 0)
    fade = 0;
  for (int i = 0; i < i_size; i++) {
    data[i].r *= fade;
    data[i].g *= fade;
    data[i].b *= fade;
    data[i].a *= fade;
  }
}

void Image::copyFrom(Image *src)
{
  memcpy((void *)data, (void *)src->data, size);
}

void Image::fillInBlack()
{
  Pixel black{0, 0, 0, 0};
  for (int i = 0; i < width * height; i++) {
    data[i] = black;
  }
}

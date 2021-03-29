#ifndef MAIN_LEVEL_H
#define MAIN_LEVEL_H

#include <iostream>
#include <fstream>
#include "Image.h"
#include "common.h"

static const std::string FLOOR_STR("./resources/floor.png");
static const std::string HOLE_STR("./resources/hole.png");
static const std::string WALL_STR("./resources/wall.png");
static const std::string SPIKES_1_STR("./resources/spikes_1.png");
static const std::string SPIKES_2_STR("./resources/spikes_2.png");
static const std::string EXIT_STR("./resources/exit.png");
static const std::string START_STR("./resources/start.png");
static const std::string TORCH_STR("./resources/torch.png");
static const int TORCH_NUM_STATES = 9;
static const int TORCHS_SPRITE_LENGTH = 135;
static const int TORCH_TILE_LENGTH = TORCHS_SPRITE_LENGTH / TORCH_NUM_STATES;
static const int PLAYER_LIGHT_COEF = 9;

enum class TileType
{
  FLOOR, TORCH, HOLE, WALL,
  EXIT, SPIKES, START
};

struct Torch
{
  Point coords;
  int state;
  Image img;

  Torch() : img(2 * LIGHT_LENGTH, 2 * LIGHT_LENGTH, 4) {};
  ~Torch() {};
};

struct Level
{
  Level(const std::string &a_path) :
    img(WINDOW_WIDTH, WINDOW_HEIGHT, 4),
    img_floor(FLOOR_STR),
    img_hole(HOLE_STR),
    img_wall(WALL_STR),
    img_exit(EXIT_STR),
    img_spikes_1(SPIKES_1_STR),
    img_spikes_2(SPIKES_2_STR),
    img_start(START_STR),
    img_torch(TORCH_STR),
    img_light(WINDOW_WIDTH, WINDOW_HEIGHT, 4),
    path(a_path)
  {
    Init();
  };

  void Draw(Image *screen);
  void drawTorch(Image *screen, int torch);
  void updateTorchState(int torch);
  void drawLight(Image *screen);
  void redrawLight(Image *screen);
  void redrawTile(Image *screen, Point p);
  void redrawPart(Image *screen, Point start, Point end);
  void drawTorchLightQuarter(Image *screen, Point point, int quarter);
  void setPassed() { is_passed = true; }
  void setSpikesAttack(Point s);
  void setLevelNum(int n) { level_num = n; }
  void setPlayerCoords(Point co);
  bool isSpikesAttack() { return is_spikes_attack; }
  bool isPassed() { return is_passed; }
  Point getStartingPoint() { return starting_point; }
  TileType getTileType(Point pos);
  TileType getTileType(char a);
  Pixel getPixel(int x, int y) { return img.GetPixel(x, y); }
  Image *getImage() { return &img; }

  ~Level()
  {
    if (torchs)
      delete [] torchs;
  }

private:
  int level_num = 0;
  bool is_passed = false;
  int torchs_num = 0;
  Torch* torchs = nullptr;
  Point starting_point, player_coords = {0, 0}, player_old_coords = {0, 0};
  Point light_update_start = {0, 0};
  Point light_update_end {WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1};
  bool is_spikes_attack = false;
  Image img, img_floor, img_hole, img_wall, img_exit, img_start;
  Image img_torch, img_light;
  Image img_spikes_1, img_spikes_2;
  char map[(WINDOW_WIDTH / TILE_SIZE) * (WINDOW_HEIGHT / TILE_SIZE)];
  const std::string path;

  void Init();
  void setTorchs();
  void drawLightImg();
  void drawTorchLightQuarter(int torch, int quarter);
  void addTorchQuarterToLightImg(int torch, int quarter);
  void subTorchQuarterFromLightImg(int torch, int quarter);
  Point getStartImgQuarter(Point centre, int quarter);
  Point getEndImgQuarter(Point centre, int quarter);
  Point getStartTorchQuarter(int quarter);
  Point getEndTorchQuarter(int quarter);
  Image* getTileImage(TileType type);
  int getQuarterFromPoint(Point centre, Point point);
  bool isTranslucentTyle(Point p);
  bool isPlayerHere(Point p);
  bool isDirectVisibility(Point p1, Point p2);
  bool isPlayerNear(Point point, int quarter);
};

#endif //MAIN_LEVEL_H

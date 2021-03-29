#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include "Level.h"

static const int MOVE_SPEED = TILE_SIZE * 6;

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum PlayerState
{
  STAND = 0,
  RIGHT_1, RIGHT_2, RIGHT_3, RIGHT_4, RIGHT_5,
  RIGHT_6, RIGHT_7, RIGHT_8, RIGHT_9, RIGHT_10,
  LEFT_1, LEFT_2, LEFT_3, LEFT_4, LEFT_5,
  LEFT_6, LEFT_7, LEFT_8, LEFT_9, LEFT_10,
  UP_1, UP_2, UP_3, UP_4, UP_5,
  UP_6, UP_7, UP_8, UP_9, UP_10,
  DOWN_1, DOWN_2, DOWN_3, DOWN_4, DOWN_5,
  DOWN_6, DOWN_7, DOWN_8, DOWN_9, DOWN_10,
  DIE
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                  coords(pos),
                  old_coords(coords),
                  img("./resources/player.png") {}

  bool Moved() const;
  bool isAlive() { return is_alive; }
  void processInput(MovementDir dir, float current_time);
  void processTime(float current_time);
  void Draw(Image *screen, Level *level);
  void updateState();
  void setDied() { is_alive = false; }
  void returnPosition() { coords.x = old_coords.x; coords.y = old_coords.y; }
  void fixCoords() { old_coords = coords; }
  void setCoords(Point co);
  Point getCoords() { return coords; }
  Point getOldCoords() { return old_coords; }
  Pixel getPixel(int x, int y) { return img.GetPixel(x, y); }
  MovementDir getDirection() { return direction; }

  ~Player() {}

private:
  Image img;
  PlayerState state = PlayerState::STAND;
  MovementDir direction = MovementDir::RIGHT;
  bool is_alive = true;
  Point coords {.x = TILE_SIZE, .y = TILE_SIZE};
  Point old_coords {.x = TILE_SIZE, .y = TILE_SIZE};
  //int move_speed = 4;
  float time = -1, delta_time = 0, last_move_time = -1;
};

#endif //  MAIN_PLAYER_H

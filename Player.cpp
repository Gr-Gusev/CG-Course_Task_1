#include "Player.h"
#include "common.h"

static const float TIME_STEP = 0.1;

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y) {
    return false;
  } else {
    return true;
  }
}

void Player::processInput(MovementDir dir, float current_time)
{
  last_move_time = current_time;
  direction = dir;

  int move_dist = MOVE_SPEED * delta_time;
  switch (dir) {
    case MovementDir::UP:
      old_coords.y = coords.y;
      coords.y += move_dist;
      break;
    case MovementDir::DOWN:
      old_coords.y = coords.y;
      coords.y -= move_dist;
      break;
    case MovementDir::LEFT:
      old_coords.x = coords.x;
      coords.x -= move_dist;
      break;
    case MovementDir::RIGHT:
      old_coords.x = coords.x;
      coords.x += move_dist;
      break;
    default:
      break;
  }
}

void Player::processTime(float current_time)
{
  if (time == -1)
    time = current_time;

  delta_time = current_time - time;
  time = current_time;
  if (time - last_move_time >= TIME_STEP)  // Player is not moving
    fixCoords();
}

void Player::updateState()
{
  if (delta_time == 0)
    return;

  if (!Moved() && state != DIE) {
    state = STAND;
  } else
  if (!is_alive || state == DIE) {
    state = DIE;
  } else
  if (state == STAND) {
    switch (direction) {
      case MovementDir::RIGHT:
        state = RIGHT_1;
        break;
      case MovementDir::LEFT:
        state = LEFT_1;
        break;
      case MovementDir::UP:
        state = UP_1;
        break;
      case MovementDir::DOWN:
        state = DOWN_1;
        break;
    }
  } else
  if (RIGHT_1 <= state && state <= RIGHT_10) {
    switch (direction) {
      case MovementDir::RIGHT:
        if (state == RIGHT_10) {
          state = RIGHT_1;
        } else {
          state = (PlayerState) (state + 1);  // Next RIGHT_i
        }
        break;
      case MovementDir::LEFT:
        state = LEFT_1;
        break;
      case MovementDir::UP:
        state = UP_1;
        break;
      case MovementDir::DOWN:
        state = DOWN_1;
        break;
    }
  } else
  if (LEFT_1 <= state && state <= LEFT_10) {
    switch (direction) {
      case MovementDir::RIGHT:
        state = RIGHT_1;
        break;
      case MovementDir::LEFT:
        if (state == LEFT_10) {
          state = LEFT_1;
        } else {
          state = (PlayerState) (state + 1);  // Next LEFT_i
        }
        break;
      case MovementDir::UP:
        state = UP_1;
        break;
      case MovementDir::DOWN:
        state = DOWN_1;
        break;
    }
  } else
  if (UP_1 <= state && state <= UP_10) {
    switch (direction) {
      case MovementDir::RIGHT:
        state = RIGHT_1;
        break;
      case MovementDir::LEFT:
        state = LEFT_1;
        break;
      case MovementDir::UP:
        if (state == UP_10) {
          state = UP_1;
        } else {
          state = (PlayerState) (state + 1);  // Next UP_i
        }
        break;
      case MovementDir::DOWN:
        state = DOWN_1;
        break;
    }
  } else
  if (DOWN_1 <= state && state <= DOWN_10) {
    switch (direction) {
      case MovementDir::RIGHT:
        state = RIGHT_1;
        break;
      case MovementDir::LEFT:
        state = LEFT_1;
        break;
      case MovementDir::UP:
        state = UP_1;
        break;
      case MovementDir::DOWN:
        if (state == DOWN_10) {
          state = DOWN_1;
        } else {
          state = (PlayerState) (state + 1);  // Next DOWN_i
        }
        break;
    }
  }
}

void Player::setCoords(Point co)
{
  coords = co;
  old_coords = co;
  time = -1;
}


void Player::Draw(Image *screen, Level *level)
{
  PlayerState old_state = state;
  // Update state:
  updateState();

  // Draw background:
  Point start = old_coords, end = old_coords;
  start.x = MAX(start.x - TILE_SIZE, 0);
  start.y = MAX(start.y - TILE_SIZE, 0);
  end = old_coords;
  end.x = MIN(end.x + 2 * TILE_SIZE, WINDOW_WIDTH - 1);
  end.y = MIN(end.y + 2 * TILE_SIZE, WINDOW_HEIGHT - 1);
  level->redrawPart(screen, start, end);

  // Find coordinates of the tile:
  int x_start, y_start;
  if (state == STAND) {
    x_start = 0;
    y_start = 0;
  } else
  if (state == DIE) {
    x_start = 2 * TILE_SIZE;
    y_start = 0;
  } else
  if (RIGHT_1 <= state && state <= RIGHT_10) {
    x_start = (state - RIGHT_1) * TILE_SIZE;
    y_start = 7 * TILE_SIZE;
  } else
  if (LEFT_1 <= state && state <= LEFT_10) {
    x_start = (state - LEFT_1) * TILE_SIZE;
    y_start = 5 * TILE_SIZE;
  } else
  if (UP_1 <= state && state <= UP_10) {
    x_start = (state - UP_1) * TILE_SIZE;
    y_start = 6 * TILE_SIZE;
  } else
  if (DOWN_1 <= state && state <= DOWN_10) {
    x_start = (state - DOWN_1) * TILE_SIZE;
    y_start = 4 * TILE_SIZE;
  }
  // Draw player:
  Pixel tmp;
  for (int y = 0; y < TILE_SIZE; y++) {
    for (int x = 0; x < TILE_SIZE; x++) {
      tmp = Blend(screen->GetPixel(x + coords.x, y + coords.y),
                  img.GetPixel(x + x_start, y_start + TILE_SIZE - y - 1));
      screen->PutPixel(x + coords.x, y + coords.y, tmp);
    }
  }
}

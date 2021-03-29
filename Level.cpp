#include "Level.h"

TileType Level::getTileType(char a)
{
  TileType ret;
  switch (a) {
    case '.':
      ret = TileType::FLOOR;
      break;
    case '#':
      ret = TileType::WALL;
      break;
    case 'x':
      ret = TileType::EXIT;
      break;
    case '@':
      ret = TileType::START;
      break;
    case 'T':
      ret = TileType::SPIKES;
      break;
    case ' ':
      ret = TileType::HOLE;
      break;
    case 'F':
      ret = TileType::TORCH;
      break;
    default:
      ret = TileType::HOLE;
      break;
  }
  return ret;
}

TileType Level::getTileType(Point pos)
{
  int x, y;
  x = pos.x / TILE_SIZE;
  y = pos.y / TILE_SIZE;

  return getTileType(map[y * (WINDOW_WIDTH / TILE_SIZE) + x]);
}

Image* Level::getTileImage(TileType type)
{
  switch (type) {
    case TileType::START:
      return &img_start;
      break;
    case TileType::SPIKES:
      return &img_spikes_1;
      break;
    case TileType::TORCH:
    case TileType::FLOOR:
      return &img_floor;
      break;
    case TileType::HOLE:
      return &img_hole;
      break;
    case TileType::WALL:
      return &img_wall;
      break;
    case TileType::EXIT:
      return &img_exit;
      break;
  }
  return &img_hole;  // We shouldn't be here
}

void Level::Init()
{
  char buf[WINDOW_WIDTH / TILE_SIZE + 1];
  TileType type;
  Point point;
  Image *im;
  Pixel tmp;
  std::ifstream file;
  file.open(path);

  file.seekg(0, std::ios_base::beg);
  for (int i = WINDOW_HEIGHT / TILE_SIZE; i > 0 ; i--) {
    file.getline(buf, WINDOW_WIDTH / TILE_SIZE + 1);
    point.y = (i - 1) * TILE_SIZE;
    for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++) {
      point.x = j * TILE_SIZE;
      type = getTileType(buf[j]);
      im = getTileImage(type);
      if (type == TileType::START) {
        starting_point = point;
        player_coords = point;
      } else
      if (type == TileType::TORCH) {
        torchs_num += 1;
      }
      // Fill the map:
      map[(i - 1) * (WINDOW_WIDTH / TILE_SIZE) + j] = buf[j];
      // Draw tile:
      for (int y = point.y; y < point.y + TILE_SIZE; y++) {
        for (int x = point.x; x < point.x + TILE_SIZE; x++) {
          tmp = im->GetPixel(x - point.x, TILE_SIZE - (y - point.y) - 1);
          img.PutPixel(x, y, tmp);
        }
      }
    }
  }
  file.close();
  setTorchs();
  drawLightImg();
}

void Level::setTorchs()
{
  if (torchs_num == 0)
    return;

  int i = 0;
  TileType type;
  Point point;
  torchs = new Torch[torchs_num];
  for (int y = 0; y < WINDOW_HEIGHT / TILE_SIZE; y++) {
    point.y =  y * TILE_SIZE + TILE_SIZE / 2;
    for (int x = 0; x < WINDOW_WIDTH / TILE_SIZE; x++) {
      point.x = x * TILE_SIZE + TILE_SIZE / 2;
      type = getTileType(map[y * (WINDOW_WIDTH / TILE_SIZE) + x]);
      if (type == TileType::TORCH) {
        torchs[i].state = i % TORCH_NUM_STATES;
        torchs[i].coords = point;
        i += 1;
      }
    }
  }
}

void Level::drawLightImg()
{
  img_light.fillInBlack();

  if (torchs_num == 0)
    return;

  for (int i = 0; i < torchs_num; i++) {
    drawTorchLightQuarter(i, 1);
    drawTorchLightQuarter(i, 2);
    drawTorchLightQuarter(i, 3);
    drawTorchLightQuarter(i, 4);

    addTorchQuarterToLightImg(i, 1);
    addTorchQuarterToLightImg(i, 2);
    addTorchQuarterToLightImg(i, 3);
    addTorchQuarterToLightImg(i, 4);
  }
}

void Level::drawLight(Image *screen)
{
  if (torchs_num == 0)
    return;

  Pixel i_pix, t_pix;
  for (int y = light_update_start.y; y <= light_update_end.y; y++) {
    for (int x = light_update_start.x; x <= light_update_end.x; x++) {
      i_pix = img.GetPixel(x, y);
      t_pix = img_light.GetPixel(x, y);
      i_pix.Add(t_pix);
      screen->PutPixel(x, y, i_pix);
    }
  }

  Point p;
  for (int i = 0; i < torchs_num; i++) {
    p = torchs[i].coords;
    p.x -= TILE_SIZE / 2;
    p.y -= TILE_SIZE / 2;
    redrawTile(screen, p);
    drawTorch(screen, i);
    updateTorchState(i);
  }

  light_update_start.x = WINDOW_WIDTH;
  light_update_start.y = WINDOW_HEIGHT;
  light_update_end.x = 0;
  light_update_end.y = 0;
}

void Level::redrawLight(Image *screen)
{
  if (torchs_num == 0)
    return;

  bool qua[4];
  Point plr = player_coords;
  plr.x += TILE_SIZE / 2;
  plr.y += TILE_SIZE / 2;
  for (int i = 0; i < torchs_num; i++) {
    if (Distance(plr, torchs[i].coords) >= LIGHT_LENGTH + PLAYER_LIGHT_COEF)
      continue;
    qua[0] = false;
    qua[1] = false;
    qua[2] = false;
    qua[3] = false;
    plr = player_coords;
    plr.x += TILE_SIZE;
    plr.y += TILE_SIZE;
    qua[getQuarterFromPoint(torchs[i].coords, player_coords) - 1] = true;
    qua[getQuarterFromPoint(torchs[i].coords, plr) - 1] = true;
    plr = player_old_coords;
    plr.x += TILE_SIZE;
    plr.y += TILE_SIZE;
    qua[getQuarterFromPoint(torchs[i].coords, player_old_coords) - 1] = true;
    qua[getQuarterFromPoint(torchs[i].coords, plr) - 1] = true;

    for (int k = 1; k <= 4; k++) {
      if (qua[k - 1]) {
        subTorchQuarterFromLightImg(i, k);
        drawTorchLightQuarter(i, k);
        addTorchQuarterToLightImg(i, k);
      }
    }
  }

  drawLight(screen);
}

void Level::redrawTile(Image *screen, Point p)
{
  Pixel i_pix, t_pix;
  Point start = p, end;
  end.x = MIN(p.x + TILE_SIZE, WINDOW_WIDTH);
  end.y = MIN(p.y + TILE_SIZE, WINDOW_HEIGHT);
  for (int y = start.y; y < end.y; y += 1) {
    for (int x = start.x; x < end.x; x += 1) {
      i_pix = img.GetPixel(x, y);
      t_pix = img_light.GetPixel(x, y);
      i_pix.Add(t_pix);
      screen->PutPixel(x, y, i_pix);
    }
  }
}

void Level::redrawPart(Image *screen, Point start, Point end)
{
  Pixel i_pix, t_pix;
  for (int y = start.y; y <= end.y; y += 1) {
    for (int x = start.x; x <= end.x; x += 1) {
      i_pix = img.GetPixel(x, y);
      t_pix = img_light.GetPixel(x, y);
      i_pix.Add(t_pix);
      screen->PutPixel(x, y, i_pix);
    }
  }

  start.x -= TILE_SIZE;
  start.y -= TILE_SIZE;
  end.x += TILE_SIZE;
  end.y += TILE_SIZE;
  for (int i = 0; i < torchs_num; i++) {
    if (Distance(torchs[i].coords, player_coords) <= LIGHT_LENGTH)
      drawTorch(screen, i);
  }
}

void Level::drawTorchLightQuarter(int torch, int quarter)
{
  Point point, start, end;  // For detecting ligth borders
  point = {LIGHT_LENGTH, LIGHT_LENGTH};
  start = getStartTorchQuarter(quarter);
  end = getEndTorchQuarter(quarter);

  // Light up pixels:
  int dist;
  float coef;
  Pixel pix;
  Point pt;
  switch (quarter) {  // It's a hack for +=2 optimization
    case (2):
      start.x += 1;
      break;
    case (3):
      start.x += 1;
      start.y += 1;
      break;
    case (4):
      start.y += 1;
      break;
  }
  // Here is +=2 optimization
  for (int y = start.y; y < end.y; y += 2) {
    for (int x = start.x; x < end.x; x += 2) {
      pix.Zero();
      dist = Distance(point, {x, y});
      coef = 1 - (float)dist / (float)LIGHT_LENGTH;
      pix.lightUp(coef);
      if (dist <= LIGHT_LENGTH) {
        pt.x = torchs[torch].coords.x - LIGHT_LENGTH + x + 1;
        pt.y = torchs[torch].coords.y - LIGHT_LENGTH + y + 1;
        if (getTileType(pt) == TileType::HOLE) {
          pix.Zero();
        } else
        if (!isDirectVisibility(torchs[torch].coords, pt))
          pix.Zero();
      } else {
        pix.Zero();
      }
      // For optimization light up 4 pixels instead 1:
      torchs[torch].img.PutPixel(x, y, pix);
      torchs[torch].img.PutPixel(x + 1, y, pix);
      torchs[torch].img.PutPixel(x, y + 1, pix);
      torchs[torch].img.PutPixel(x + 1, y + 1, pix);
    }
  }

  Point i_start, i_end;
  i_start = getStartImgQuarter(torchs[torch].coords, quarter);
  i_end = getEndImgQuarter(torchs[torch].coords, quarter);
  light_update_start.x = MIN(light_update_start.x, i_start.x);
  light_update_start.y = MIN(light_update_start.y, i_start.y);
  light_update_end.x = MAX(light_update_end.x, i_end.x);
  light_update_end.y = MAX(light_update_end.y, i_end.y);

}

void Level::addTorchQuarterToLightImg(int torch, int quarter)
{
  Point t_start, t_end;
  t_start = getStartTorchQuarter(quarter);
  t_end = getEndTorchQuarter(quarter);

  Point i_start, i_end;
  i_start = getStartImgQuarter(torchs[torch].coords, quarter);
  i_end = getEndImgQuarter(torchs[torch].coords, quarter);

  Point pt;
  Pixel i_pix, t_pix;
  for (int y = i_start.y; y <= i_end.y; y++) {
    for (int x = i_start.x; x <= i_end.x; x++) {
      pt.x = x - torchs[torch].coords.x + LIGHT_LENGTH;
      pt.y = y - torchs[torch].coords.y + LIGHT_LENGTH;
      i_pix = img_light.GetPixel(x, y);
      t_pix = torchs[torch].img.GetPixel(pt.x, pt.y);
      i_pix.Add(t_pix);
      img_light.PutPixel(x, y, i_pix);
    }
  }
}

void Level::subTorchQuarterFromLightImg(int torch, int quarter)
{
  Point t_start, t_end;
  t_start = getStartTorchQuarter(quarter);
  t_end = getEndTorchQuarter(quarter);

  Point i_start, i_end;
  i_start = getStartImgQuarter(torchs[torch].coords, quarter);
  i_end = getEndImgQuarter(torchs[torch].coords, quarter);

  Point pt;
  Pixel i_pix, t_pix;
  for (int y = i_start.y; y <= i_end.y; y++) {
    for (int x = i_start.x; x <= i_end.x; x++) {
      pt.x = x - torchs[torch].coords.x + LIGHT_LENGTH;
      pt.y = y - torchs[torch].coords.y + LIGHT_LENGTH;
      i_pix = img_light.GetPixel(x, y);
      t_pix = torchs[torch].img.GetPixel(pt.x, pt.y);
      i_pix.Sub(t_pix);
      img_light.PutPixel(x, y, i_pix);
    }
  }
}

Point Level::getStartImgQuarter(Point centre, int quarter)
{
  Point ret;
  switch (quarter) {
    case (1):
      ret.x = centre.x;
      ret.y = centre.y;
      break;
    case (2):
      ret.x = MAX(centre.x - LIGHT_LENGTH, 0);
      ret.y = centre.y;
      break;
    case (3):
      ret.x = MAX(centre.x - LIGHT_LENGTH, 0);
      ret.y = MAX(centre.y - LIGHT_LENGTH, 0);
      break;
    case (4):
      ret.x = centre.x;
      ret.y = MAX(centre.y - LIGHT_LENGTH, 0);
      break;
  }
  return ret;
}

Point Level::getEndImgQuarter(Point centre, int quarter)
{
  Point ret;
  switch (quarter) {
    case (1):
      ret.x = MIN(centre.x + LIGHT_LENGTH - 1, WINDOW_WIDTH - 1);
      ret.y = MIN(centre.y + LIGHT_LENGTH - 1, WINDOW_HEIGHT - 1);
      break;
    case (2):
      ret.x = centre.x - 1;
      ret.y = MIN(centre.y + LIGHT_LENGTH - 1, WINDOW_HEIGHT - 1);
      break;
    case (3):
      ret.x = centre.x - 1;
      ret.y = centre.y - 1;
      break;
    case (4):
      ret.x = MIN(centre.x + LIGHT_LENGTH - 1, WINDOW_WIDTH - 1);
      ret.y = centre.y - 1;
      break;
  }
  return ret;
}

Point Level::getStartTorchQuarter(int quarter)
{
  Point ret;
  switch (quarter) {
    case (1):
      ret.x = LIGHT_LENGTH;
      ret.y = LIGHT_LENGTH;
      break;
    case (2):
      ret.x = 0;
      ret.y = LIGHT_LENGTH;
      break;
    case (3):
      ret.x = 0;
      ret.y = 0;
      break;
    case (4):
      ret.x = LIGHT_LENGTH;
      ret.y = 0;
      break;
  }
  return ret;
}

Point Level::getEndTorchQuarter(int quarter)
{
  Point ret;
  switch (quarter) {
    case (1):
      ret.x = 2 * LIGHT_LENGTH - 1;
      ret.y = 2 * LIGHT_LENGTH - 1;
      break;
    case (2):
      ret.x = LIGHT_LENGTH - 1;
      ret.y = 2 * LIGHT_LENGTH - 1;
      break;
    case (3):
      ret.x = LIGHT_LENGTH - 1;
      ret.y = LIGHT_LENGTH - 1;
      break;
    case (4):
      ret.x = 2 * LIGHT_LENGTH - 1;
      ret.y = LIGHT_LENGTH - 1;
      break;
  }
  return ret;
}

void Level::Draw(Image *screen)
{
  screen->copyFrom(&img);
}

void Level::updateTorchState(int torch)
{
  torchs[torch].state = (torchs[torch].state + 1) % TORCH_NUM_STATES;
}

void Level::drawTorch(Image *screen, int torch)
{
  Point point = torchs[torch].coords;
  point.x -= TORCH_TILE_LENGTH / 2;
  point.y -= TILE_SIZE / 2;
  Pixel pix;
  int x_start = torchs[torch].state * TORCH_TILE_LENGTH;
  for (int y = 0; y < TILE_SIZE; y++) {
    for (int x = 0; x < TORCH_TILE_LENGTH; x++) {
      pix = Blend(screen->GetPixel(point.x + x, point.y + y),
                  img_torch.GetPixel(x + x_start, TILE_SIZE - y - 1));
      screen->PutPixel(point.x + x, point.y + y, pix);
    }
  }
}

int Level::getQuarterFromPoint(Point centre, Point point)
{
  if (point.x >= centre.x && point.y >= centre.y)  // quarter 1
    return 1;
  if (point.x < centre.x && point.y >= centre.y)  // quarter 2
    return 2;
  if (point.x < centre.x && point.y < centre.y)  // quarter 3
    return 3;
  if (point.x > centre.x && point.y < centre.y)  // quarter 4
    return 4;

  return 0;  // We shouldn't be here
}

void Level::setSpikesAttack(Point sp)
{
  if (getTileType(sp) != TileType::SPIKES)
    return;

  int x_start = sp.x - (sp.x % TILE_SIZE);
  int y_start = sp.y - (sp.y % TILE_SIZE);
  is_spikes_attack = true;

  // Draw spikes:
  Pixel tmp;
  for (int y = 0; y < TILE_SIZE; y++) {
    for (int x = 0; x < TILE_SIZE; x++) {
      tmp = img_spikes_2.GetPixel(x, TILE_SIZE - y -1);
      img.PutPixel(x + x_start, y + y_start, tmp);
    }
  }
}

void Level::setPlayerCoords(Point co)
{
  player_old_coords = player_coords;
  player_coords = co;
}

bool Level::isTranslucentTyle(Point p)
{
  if (getTileType(p) == TileType::WALL) {
    return false;
  } else {
    return true;
  }
}

bool Level::isPlayerHere(Point p)
{
  Point tmp;
  tmp.x = player_coords.x + TILE_SIZE / 2;
  tmp.y = player_coords.y + TILE_SIZE / 2;

  if (Distance(tmp, p) <= PLAYER_LIGHT_COEF) {
    return true;
  } else {
    return false;
  }
}

bool Level::isDirectVisibility(Point p1, Point p2)
{
  Point tmp;
  int dist = Distance(p1, p2);
  float coef;

  if (p2.x >= WINDOW_WIDTH || p2.y >= WINDOW_HEIGHT)
    return false;

  if (p2.x < 0 || p2.y < 0)
    return false;

  for (int d = 1; d <= dist; d += 2) {
    coef = (float)(d) / (float)(dist);
    tmp.x = (int)(coef * (float)(p2.x - p1.x)) + p1.x;
    tmp.y = (int)(coef * (float)(p2.y - p1.y)) + p1.y;
    if (!isTranslucentTyle(tmp) || isPlayerHere(tmp)) {
      return false;
    }
  }

  return true;
}

bool Level::isPlayerNear(Point point, int quarter)
{
  Point player = player_coords;
  player.x += TILE_SIZE / 2;
  player.y += TILE_SIZE / 2;

  int dist = Distance(player, point);
  if (dist < LIGHT_LENGTH + PLAYER_LIGHT_COEF) {
    return true;
  }
  return false;
}

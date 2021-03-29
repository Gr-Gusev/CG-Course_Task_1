#include "common.h"
#include "Image.h"
#include "Player.h"
#include "Level.h"
#include <GLFW/glfw3.h>

static const std::string LEVEL_STR("./resources/level_");
static const std::string YOU_WIN_STR("./resources/you_win.png");
static const std::string YOU_LOSE_STR("./resources/you_lose.png");
static const std::string LEVEL_NUM_STR("./resources/numbers/level.png");
static const std::string NUMBER_STR("./resources/numbers/");
static const std::string SLASH_STR("./resources/numbers/slash.png");
static const std::string FALL_STR("./resources/fall_");
static const std::string TXT_STR(".txt");
static const std::string PNG_STR(".png");
static const int MESSAGE_WIDTH = 500;
static const int LEVEL_NUM_STR_WIDTH = 187;
static const int MESSAGE_HEIGHT = 135;
static const int NUMBER_SIZE = 50;
static const int LEVELS_NUM = 2;  // 1 <= NUM <= 9
static const int FALL_NUM = 3;
static const float TIME_STEP = 0.035;
static const float TRANSPARENCY_TIME = 4;

static GLfloat currentTime; // Changes only in main

enum class GameStatus
{
  START,
  PLAY,
  LEVEL_FADE_OUT,
  LEVEL_FADE_IN,
  WIN,
  LOSE,
  END
};

struct LevelNumber
{
  LevelNumber() :
    img(LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2, NUMBER_SIZE, 4) {};

  void Init();
  void drawMessage(Image *screen);
  void drawMessage(Image *screen, float current_time);
  void clearMessage(Image *screen, Level *level);
  void Next();
  bool isNeedToDraw();
  bool isLastLevel();
  int getLevelNumber() { return number; };

  ~LevelNumber() {};

private:
  int number = 0;
  float transparency = 0;  // between 0 and 1
  Image img;
  float start_time = -1;

  void createMessage();
};

void LevelNumber::Init()
{
  number = 1;
  transparency = 1;
  start_time = -1;
  createMessage();
}

void LevelNumber::Next()
{
  number += 1;
  transparency = 1;
  start_time = -1;
  createMessage();
}

void LevelNumber::createMessage()
{
  Pixel tmp;
  int x, y;
  Image level_str(LEVEL_NUM_STR);
  Image slash(SLASH_STR);
  Image cur_level(NUMBER_STR + std::to_string(number) + PNG_STR);
  Image max_level(NUMBER_STR + std::to_string(LEVELS_NUM) + PNG_STR);

  for (y = 0; y < NUMBER_SIZE; y++) {  // Draw "LEVEL"
    for (x = 0; x < LEVEL_NUM_STR_WIDTH; x++) {
      tmp = level_str.GetPixel(x, NUMBER_SIZE- y - 1);
      img.PutPixel(x, y, tmp);
    }
  }

  for (y = 0; y < NUMBER_SIZE; y++) {  // Draw current level number
    for (x = 0; x < NUMBER_SIZE; x++) {
      tmp = cur_level.GetPixel(x, NUMBER_SIZE- y - 1);
      img.PutPixel(x + LEVEL_NUM_STR_WIDTH + NUMBER_SIZE / 2, y, tmp);
    }
  }

  for (y = 0; y < NUMBER_SIZE; y++) {  // Draw slash
    for (x = 0; x < NUMBER_SIZE; x++) {
      tmp = slash.GetPixel(x, NUMBER_SIZE- y - 1);
      img.PutPixel(x + LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 3 / 2, y, tmp);
    }
  }

  for (y = 0; y < NUMBER_SIZE; y++) {  // Draw max level number
    for (x = 0; x < NUMBER_SIZE; x++) {
      tmp = max_level.GetPixel(x, NUMBER_SIZE- y - 1);
      img.PutPixel(x + LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 5 / 2, y, tmp);
    }
  }
}

void LevelNumber::drawMessage(Image *screen, float current_time)
{
  if (transparency == -1)
    return;

  if (start_time == -1) // First call for current level
    start_time = current_time;

  Pixel tmp, pix;
  int x_start = (WINDOW_WIDTH - (LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2)) / 2;
  int y_start = (WINDOW_HEIGHT - NUMBER_SIZE) / 2;
  for (int y = 0; y < NUMBER_SIZE; y++) {
    for (int x = 0; x < LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2; x++) {
      pix = img.GetPixel(x, y);
      pix.makeTransparentTo(transparency);
      tmp = Blend(screen->GetPixel(x + x_start, y + y_start), pix);
      screen->PutPixel(x + x_start, y + y_start, tmp);
    }
  }

  transparency = 1 - (current_time - start_time) / TRANSPARENCY_TIME;
  if (transparency < 0) {
    transparency = -1;
  }
}

void LevelNumber::drawMessage(Image *screen)
{
  Pixel tmp;
  int x_start = (WINDOW_WIDTH - (LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2)) / 2;
  int y_start = (WINDOW_HEIGHT - NUMBER_SIZE) / 2;
  for (int y = 0; y < NUMBER_SIZE; y++) {
    for (int x = 0; x < LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2; x++) {
      tmp = Blend(screen->GetPixel(x + x_start, y + y_start),
                  img.GetPixel(x, y));
      tmp.makeTransparentTo(transparency);
      screen->PutPixel(x + x_start, y + y_start, tmp);
    }
  }
}

void LevelNumber::clearMessage(Image *screen, Level *level)
{
  if (transparency == -1)
    return;

  int x_start = (WINDOW_WIDTH - (LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2)) / 2;
  int y_start = (WINDOW_HEIGHT - NUMBER_SIZE) / 2;
  int x_end = x_start + LEVEL_NUM_STR_WIDTH + NUMBER_SIZE * 7/2;
  int y_end = y_start + NUMBER_SIZE;
  for (int y = y_start; y < y_end; y += TILE_SIZE) {
    for (int x = x_start; x < x_end; x += TILE_SIZE) {
      level->redrawTile(screen, {x, y});
    }
  }
}

bool LevelNumber::isNeedToDraw()
{
  if (transparency != -1) {
    return true;
  } else {
    return false;
  }
}

bool LevelNumber::isLastLevel()
{
  if (number == LEVELS_NUM) {
    return true;
  } else {
    return false;
  }
}

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;

void OnKeyboardPressed(GLFWwindow* window, int key,
                       int scancode, int action, int mode)
{
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void processPlayerMovement(Player *player, float current_time)
{
  player->processTime(current_time);

  if (Input.keys[GLFW_KEY_W])
    player->processInput(MovementDir::UP, current_time);
  else if (Input.keys[GLFW_KEY_S])
    player->processInput(MovementDir::DOWN, current_time);
  if (Input.keys[GLFW_KEY_A])
    player->processInput(MovementDir::LEFT, current_time);
  else if (Input.keys[GLFW_KEY_D])
    player->processInput(MovementDir::RIGHT, current_time);
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse) {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}

int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Controls: " << std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor " << std::endl;
  std::cout << "W, A, S, D - movement " << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void drawEndMessage(Image *screen, std::string str)
{
  Image img(str);
  Pixel tmp;

  int x_start = (WINDOW_WIDTH - MESSAGE_WIDTH) / 2;
  int y_start = (WINDOW_HEIGHT - MESSAGE_HEIGHT) / 2;

  for (int y = 0; y < MESSAGE_HEIGHT; y++) {
    for (int x = 0; x < MESSAGE_WIDTH; x++) {
      tmp = img.GetPixel(x, MESSAGE_HEIGHT - y - 1);
      screen->PutPixel(x + x_start, y + y_start, tmp);
    }
  }
}

GameStatus drawFinalPlayerPlace(Image *screen, Player *player, Level *level)
{
  Point p_start = player->getCoords(), p_end;
  p_end = p_start;
  p_start.x = MAX(p_start.x - 2 * TILE_SIZE, 0);
  p_start.y = MAX(p_start.y - 2 * TILE_SIZE, 0);
  p_end.x = MIN(p_end.x + 2 * TILE_SIZE, WINDOW_WIDTH - 1);
  p_end.y = MIN(p_end.y + 2 * TILE_SIZE, WINDOW_HEIGHT - 1);
  level->redrawPart(screen, p_start, p_end);

  if (level->isSpikesAttack()) {
    player->Draw(screen, level);
    return GameStatus::LOSE;
  }

  // Player fall in the hole:
  static int fall_img_num = 0;
  TileType tile1, tile2, tile3, tile4;
  float coef1, coef2;
  coef1 = (1 - TILE_COEF) * (float) TILE_SIZE;
  coef2 = TILE_COEF * (float) TILE_SIZE;
  p_start = player->getCoords();
  Point p1, p2, p3, p4;
  // 2 1
  // 3 4
  p1.x = p_start.x + (int) (coef2);
  p1.y = p_start.y + (int) (coef2);
  p2.x = p_start.x + (int) (coef1);
  p2.y = p_start.y + (int) (coef2);
  p3.x = p_start.x + (int) (coef1);
  p3.y = p_start.y + (int) (coef1);
  p4.x = p_start.x + (int) (coef2);
  p4.y = p_start.y + (int) (coef1);

  tile1 = level->getTileType(p1);
  tile2 = level->getTileType(p2);
  tile3 = level->getTileType(p3);
  tile4 = level->getTileType(p4);

  if (tile1 == TileType::HOLE) {
    p_start = p1;
  } else
  if (tile2 == TileType::HOLE) {
    p_start = p2;
  } else
  if (tile3 == TileType::HOLE) {
    p_start = p3;
  } else
  if (tile4 == TileType::HOLE) {
    p_start = p4;
  }

  fall_img_num += 1;
  if (fall_img_num < FALL_NUM) {
    Image img(FALL_STR + std::to_string(fall_img_num) + PNG_STR);
    Pixel tmp;
    p_start.x -= p_start.x % TILE_SIZE;
    p_start.y -= p_start.y % TILE_SIZE;
    p_start.y += TILE_SIZE - img.getHeight();
    for (int y = 0; y < img.getHeight(); y++) {
      for (int x = 0; x < img.getWidth(); x++) {
        tmp = Blend(screen->GetPixel(p_start.x + x, p_start.y + y),
                    img.GetPixel(x, img.getHeight() - y - 1));
        screen->PutPixel(p_start.x + x, p_start.y + y, tmp);
      }
    }
  }

  if (fall_img_num <= FALL_NUM) {
    return GameStatus::PLAY;
  } else {
    return GameStatus::LOSE;
  }
}

void gameAction(Level *level, Player *player)
{
  TileType tile_type;
  Point pos = player->getCoords(), tmp_pos1, tmp_pos2, tmp_pos3, tmp_pos4;
  TileType tile1, tile2, tile3, tile4;
  float coef1, coef2;
  coef1 = (1 - TILE_COEF) * (float) TILE_SIZE;
  coef2 = TILE_COEF * (float) TILE_SIZE;

  if (!player->Moved()) {
    return;
  }

  // 2 1
  // 3 4
  tmp_pos1.x = pos.x + (int) (coef2);
  tmp_pos1.y = pos.y + (int) (coef2);

  tmp_pos2.x = pos.x + (int) (coef1);
  tmp_pos2.y = pos.y + (int) (coef2);

  tmp_pos3.x = pos.x + (int) (coef1);
  tmp_pos3.y = pos.y + (int) (coef1);

  tmp_pos4.x = pos.x + (int) (coef2);
  tmp_pos4.y = pos.y + (int) (coef1);

  tile1 = level->getTileType(tmp_pos1);
  tile2 = level->getTileType(tmp_pos2);
  tile3 = level->getTileType(tmp_pos3);
  tile4 = level->getTileType(tmp_pos4);

  // Checking walls and torchs:
  if (tile1 == TileType::WALL
      || tile2 == TileType::WALL
      || tile3 == TileType::WALL
      || tile4 == TileType::WALL
      || tile1 == TileType::TORCH
      || tile2 == TileType::TORCH
      || tile3 == TileType::TORCH
      || tile4 == TileType::TORCH)
  {
    player->returnPosition();
    return;
  }

  // Checking end of map:
  if (pos.x + TILE_SIZE >= WINDOW_WIDTH - coef1 - 2
      || pos.y + TILE_SIZE >= WINDOW_HEIGHT - coef1 - 2
      || pos.x <= coef1 + 1
      || pos.y <= coef1 + 1)
  {
    player->returnPosition();
    return;
  }

  // Checking spikes and holes:
  if (tile1 == TileType::SPIKES
      || tile2 == TileType::SPIKES
      || tile3 == TileType::SPIKES
      || tile4 == TileType::SPIKES
      || tile1 == TileType::HOLE
      || tile2 == TileType::HOLE
      || tile3 == TileType::HOLE
      || tile4 == TileType::HOLE)
  {
    player->setDied();
    level->setSpikesAttack(tmp_pos1);
    level->setSpikesAttack(tmp_pos2);
    level->setSpikesAttack(tmp_pos3);
    level->setSpikesAttack(tmp_pos4);
    return;
  }

  // Checking exit:
  if (tile1 == TileType::EXIT
      || tile2 == TileType::EXIT
      || tile3 == TileType::EXIT
      || tile4 == TileType::EXIT)
  {
    level->setPassed();
    return;
  }

  level->setPlayerCoords(player->getCoords());
}

GameStatus gameStart(Image *screen, Level *level,
                     Player *player, LevelNumber *level_num)
{
  level->Draw(screen);
  level->drawLight(screen);
  player->Draw(screen, level);
  level_num->drawMessage(screen);
  return GameStatus::LEVEL_FADE_OUT;
}

GameStatus gamePlay(Image *screen, Level *level,
                    Player *player, LevelNumber *level_num)
{
  if (player->isAlive()) {
    processPlayerMovement(player, currentTime);
    gameAction(level, player);
    level_num->clearMessage(screen, level);
    level->redrawLight(screen);
    player->Draw(screen, level);
    level_num->drawMessage(screen, currentTime);
    if (level->isPassed()) {
      if (level_num->isLastLevel()) {  // Finish - Win
        return GameStatus::WIN;
      } else {  // Go to the next level
        return GameStatus::LEVEL_FADE_IN;
      }
    } else {
      return GameStatus::PLAY;
    }
  } else {  // Finish - Lose
    return drawFinalPlayerPlace(screen, player, level);
  }
}

int main(int argc, char** argv)
{
	if (!glfwInit())
    return -1;
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        "task1 base project", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, OnKeyboardPressed);
	glfwSetCursorPosCallback(window, OnMouseMove);
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback(window, OnMouseScroll);

	if (initGL() != 0)
		return -1;

  // Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

  LevelNumber level_num;
  level_num.Init();
  Level *level;
  level = new Level(LEVEL_STR
                    + std::to_string(level_num.getLevelNumber())
                    + TXT_STR);
  Point starting_pos = level->getStartingPoint();
  Player player(starting_pos);
	Image screen_buffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
  Image buffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
  GameStatus game_status = GameStatus::START;
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  currentTime = glfwGetTime();
  GLfloat deltaTime = 0.0f;
  GLfloat lastTime = currentTime - TIME_STEP;
  float fade = 0;

  // Game loop:
  while (!glfwWindowShouldClose(window)) {
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    if (deltaTime <= TIME_STEP)
      continue;

    lastTime = currentTime;
    glfwPollEvents();

    if (game_status == GameStatus::START) {
      game_status = gameStart(&buffer, level, &player, &level_num);
    } else
    if (game_status == GameStatus::PLAY) {
      game_status = gamePlay(&screen_buffer, level, &player, &level_num);
      if (level->isPassed()) {
        buffer.copyFrom(&screen_buffer);
      }
    } else
    if (game_status == GameStatus::LEVEL_FADE_IN) {
      fade -= FADE_STEP;
      if (fade <= 0) {
        fade = 0;
        delete level;
        level_num.Next();
        level = new Level(LEVEL_STR
                          + std::to_string(level_num.getLevelNumber())
                          + TXT_STR);
        player.setCoords(level->getStartingPoint());
        game_status = GameStatus::START;
      }
      screen_buffer.copyFrom(&buffer);
      screen_buffer.fadeTo(fade);
    } else
    if (game_status == GameStatus::LEVEL_FADE_OUT) {
      fade += FADE_STEP;
      screen_buffer.copyFrom(&buffer);
      if (fade >= 1) {
        fade = 1;
        game_status = GameStatus::PLAY;
      }
      screen_buffer.fadeTo(fade);
    } else
    if (game_status == GameStatus::WIN) {
      drawEndMessage(&screen_buffer, YOU_WIN_STR);
      game_status = GameStatus::END;
    } else
    if (game_status == GameStatus::LOSE) {
      drawEndMessage(&screen_buffer, YOU_LOSE_STR);
      game_status = GameStatus::END;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA,
                  GL_UNSIGNED_BYTE, screen_buffer.getData());  GL_CHECK_ERRORS;
    glfwSwapBuffers(window);
  }  // Game loop

  delete level;
	glfwTerminate();
	return 0;
}

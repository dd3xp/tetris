#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>

class Tetris {

  static const std::uint32_t lines   {20};
  static const std::uint32_t cols    {10};
  static const std::uint32_t squares {4};
  static const std::uint32_t shapes  {7};

  std::vector<std::vector<std::uint32_t>> area;
  std::vector<std::vector<std::uint32_t>> forms;

  struct Coords {
    std::uint32_t x, y;
  } z[squares], k[squares];

  std::shared_ptr<sf::RenderWindow> window;
  sf::Texture tiles, bg;
  std::shared_ptr<sf::Sprite> sprite, background;
  sf::Clock clock;
  sf::Font  font;
  sf::Text txtScore, txtGameOver;
  sf::Music music;
  std::vector<std::string> musicFiles;
  std::size_t currentMusicIndex;

  int dirx, color, score, specialShape1 = 0;
  bool rotate, gameover, generateSpecialBlocks, specialShape2 = false;
  float timercount, delay;
  std::ofstream logFile;
  std::uint32_t currentShape;
  int scoreThreshold;

protected:
  void events();
  void draw();
  void moveToDown();
  void setRotate();
  void resetValues();
  void changePosition();
  bool maxLimit();
  void setScore();
  void fillRandomLines(std::uint32_t count);
  void fillTopLine(); 
  void fillBottomLine();
  void switchMusic();

public:
  Tetris();
  ~Tetris();
  void run();
};

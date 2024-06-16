#include "tetris.hpp"
#include <queue>
#include <algorithm> // 添加用于随机打乱的库

Tetris::Tetris() : specialShape1(0), specialShape2(false), scoreThreshold(0), currentMusicIndex(0) {
    logFile.open("shape_log.txt", std::ios::app);

    area.resize(lines);
    for (std::size_t i {}; i < area.size(); ++i) {
        area[i].resize(cols); 
    }

    forms = {
        {1,3,5,7}, // I: 0
        {2,4,5,7}, // Z: 1 
        {3,5,4,6}, // S: 2
        {3,5,4,7}, // T: 3
        {2,3,5,7}, // L: 4
        {3,5,7,6}, // J: 5
        {2,3,4,5}, // O: 6
        {1,1,1,1}, // 特殊形状o: 7
        {2,2,2,2}, // 特殊形状n: 8
    };

    window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(360, 720),
        "Tetris (remix)",
        sf::Style::Titlebar | sf::Style::Close
    );
    window->setPosition(sf::Vector2i(100, 100));

    tiles.loadFromFile("./resources/img/squares.png");
    sprite = std::make_shared<sf::Sprite>();
    sprite->setTexture(tiles);
    sprite->setTextureRect(sf::IntRect(0, 0, 36, 36));

    bg.loadFromFile("./resources/img/background.png");
    background = std::make_shared<sf::Sprite>();
    background->setTexture(bg);

    dirx = score = {0};
    rotate = gameover = {false};
    timercount = {0.f};
    delay = {0.3f};
    color = {1};

    currentShape = std::rand() % shapes;
    for (std::size_t i {}; i < squares; ++i) {
        z[i].x = forms[currentShape][i] % 2; 
        z[i].y = forms[currentShape][i] / 2; 
    }
    const char* shapeLetters = "IZSTLJOou";
    logFile << "Generated shape: " << shapeLetters[currentShape] << std::endl;
    std::cout << "Generated shape: " << shapeLetters[currentShape] << std::endl;

    font.loadFromFile("./resources/font/font.ttf");
    txtScore.setFont(font);
    txtScore.setPosition(100.f, 10.f);
    txtScore.setString("SCORE: " + std::to_string(score));
    txtScore.setCharacterSize(30);
    txtScore.setOutlineThickness(3);

    txtGameOver.setFont(font);
    txtGameOver.setPosition(30.f, 300.f);
    txtGameOver.setString("GAME OVER");
    txtGameOver.setCharacterSize(50);
    txtGameOver.setOutlineThickness(3);

    musicFiles = {
        "./resources/music/music1.mp3",
        "./resources/music/music2.mp3",
        "./resources/music/music3.mp3"
    };
    if (!music.openFromFile(musicFiles[currentMusicIndex])) {
        std::cerr << "Failed to load music file: " << musicFiles[currentMusicIndex] << std::endl;
    }
    music.setLoop(false);
    music.play();
}

Tetris::~Tetris() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Tetris::switchMusic() {
    currentMusicIndex = (currentMusicIndex + 1) % musicFiles.size();
    if (!music.openFromFile(musicFiles[currentMusicIndex])) {
        std::cerr << "Failed to load music file: " << musicFiles[currentMusicIndex] << std::endl;
    }
    music.play();
}

void Tetris::events() {
    float time = clock.getElapsedTime().asSeconds();
    clock.restart();
    timercount += time;

    auto e = std::make_shared<sf::Event>();
    while (window->pollEvent(*e)) {
        if (e->type == sf::Event::Closed) {
            window->close();
        }

        if (e->type == sf::Event::KeyPressed) {
            if (e->key.code == sf::Keyboard::Up) {
                rotate = true;
            } else if (e->key.code == sf::Keyboard::Right) {
                ++dirx;
            } else if (e->key.code == sf::Keyboard::Left) {
                --dirx;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        delay = 0.05f;
    }

    if (music.getStatus() == sf::Music::Stopped) {
        switchMusic();
    }
}

void Tetris::draw() {
    window->clear(sf::Color::Black);
    window->draw(*background);

    for (std::size_t i {}; i < lines; ++i) {
        for (std::size_t j {}; j < cols; ++j) {
            if (area[i][j] != 0) {
                sprite->setTextureRect(sf::IntRect(area[i][j] * 36, 0, 36, 36));
                sprite->setPosition(j * 36, i * 36);
                window->draw(*sprite);
            }
        } 
    }

    for (std::size_t i {}; i < squares; ++i) {
        sprite->setTextureRect(sf::IntRect(color * 36, 0, 36, 36));
        sprite->setPosition(z[i].x * 36, z[i].y * 36);
        window->draw(*sprite);
    }

    window->draw(txtScore);
    if (gameover) {
        window->draw(txtGameOver);
    }
    window->display();
}

void Tetris::run() {
    while (window->isOpen()) {
        events();
        if (!gameover) {
            changePosition();
            setRotate();
            moveToDown();
            setScore();
            resetValues();
        }
        draw();
    }
}

void Tetris::moveToDown() {
    if (timercount > delay) {
        for (std::size_t i {}; i < squares; ++i) {
            k[i] = z[i];
            ++z[i].y; 
        }

        if (maxLimit()) {
            for (std::size_t i {}; i < squares; ++i) {
                area[k[i].y][k[i].x] = color; 
            }

            setScore();

            color = std::rand() % shapes + 1;

            if (specialShape1 > 0) {
                currentShape = 7;
                specialShape1--;
            }
            else if (specialShape2 == true) {
                currentShape = 8;
                specialShape2 = false;
            }
            else currentShape = std::rand() % shapes;

            for (std::size_t i {}; i < squares; ++i) {
                z[i].x = forms[currentShape][i] % 2; 
                z[i].y = forms[currentShape][i] / 2; 
            }
            const char* shapeLetters = "IZSTLJOou";
            logFile << "Generated shape: " << shapeLetters[currentShape] << std::endl;
            std::cout << "Generated shape: " << shapeLetters[currentShape] << std::endl;
        }

        timercount = 0;
    }
}

void Tetris::setRotate() {
    if (rotate) {
        Coords coords = z[1];
        for (std::size_t i {}; i < squares; ++i) {
            int x = z[i].y - coords.y; 
            int y = z[i].x - coords.x;

            z[i].x = coords.x - x;
            z[i].y = coords.y + y;
        }

        if (maxLimit()) {
            for (std::size_t i {}; i < squares; ++i) {
                z[i] = k[i]; 
            }
        }
    }
}

void Tetris::resetValues() {
    dirx = 0;
    rotate = false;
    delay = 0.3f;
}

void Tetris::changePosition() {
    for (std::size_t i {}; i < squares; ++i) {
        k[i] = z[i];
        z[i].x += dirx; 
    }

    if (maxLimit()) {
        for (std::size_t i {}; i < squares; ++i) {
            z[i] = k[i]; 
        }
    }
}

bool Tetris::maxLimit() {
    for (std::size_t i {}; i < squares; ++i) {
        if (z[i].x < 0 || 
            z[i].x >= cols || 
            z[i].y >= lines || 
            area[z[i].y][z[i].x]) {
            return true;
        }
    }
    return false;
}

// ==================================O, L型消除处理==================================
void Tetris::fillRandomLines(std::uint32_t count) {
    std::vector<std::uint32_t> availableLines;
    for (std::uint32_t i = 1; i < lines - 1; ++i) {
        bool hasContent = false;
        for (std::size_t j = 0; j < cols; ++j) {
            if (area[i][j] != 0) {
                hasContent = true;
                break;
            }
        }
        if (hasContent) {
            availableLines.push_back(i);
        }
    }

    std::random_shuffle(availableLines.begin(), availableLines.end());
    count = std::min(count, static_cast<std::uint32_t>(availableLines.size()));

    for (std::uint32_t i = 0; i < count; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            area[availableLines[i]][j] = 8;
        }
    }

    currentShape = 99; // 设置一个特殊值
}

// ==================================S型和Z型消除处理==================================
void Tetris::fillTopLine() {
    for (std::uint32_t i = 1; i < lines; ++i) {
        bool hasContent = false;
        for (std::size_t j = 0; j < cols; ++j) {
            if (area[i][j] != 0) {
                hasContent = true;
                break;
            }
        }
        if (hasContent) {
            for (std::size_t j = 0; j < cols; ++j) {
                area[i][j] = 8;
            }
            break;
        }
    }
}

void Tetris::fillBottomLine() {
    for (std::int32_t i = lines - 2; i >= 0; --i) {
        bool hasContent = false;
        for (std::size_t j = 0; j < cols; ++j) {
            if (area[i][j] != 0) {
                hasContent = true;
                break;
            }
        }
        if (hasContent) {
            for (std::size_t j = 0; j < cols; ++j) {
                area[i][j] = 8;
            }
            break;
        }
    }
}

void Tetris::setScore() {
    std::uint32_t match = lines - 1;
    std::queue<std::uint32_t> clearedLines;
    for (std::size_t i = match; i >= 1; --i) {
        std::uint32_t sum {};
        for (std::size_t j {}; j < cols; ++j) {
            if (area[i][j]) {
                if (i == 1) {
                    gameover = true;
                }
                ++sum;
            }
            area[match][j] = area[i][j];
        }
        if (sum < cols) {
            --match;
        } else {
            const char* shapeLetters = "IZSTLJOo";
            logFile << "Cleared by shape: " << shapeLetters[currentShape] << std::endl;
            std::cout << "Cleared by shape: " << shapeLetters[currentShape] << std::endl;
            std::cout << "Cleared line: " << i << std::endl;

            // 如果当前形状是T型方块，则+5分而不是1分
            if (currentShape == 3) {
                score += 5;
            } else {
                score += 1;
            }
            txtScore.setString("SCORE: " + std::to_string(score));

            // 如果当前形状是I型方块，将清除的行加入队列
            if (currentShape == 0) {
                clearedLines.push(i);
            }

            // 如果当前形状是O型方块，填满随机3行
            if (currentShape == 6) {
                fillRandomLines(3);
            }

            // 如果当前形状是S型方块，填满最顶上的有内容的行
            if (currentShape == 2) {
                fillTopLine();
            }

            // 如果当前形状是Z型方块，填满最底下的有内容的行
            if (currentShape == 1) {
                fillBottomLine();
            }

            // 如果当前形状是J型，则会生成3个特殊的小块块
            if (currentShape == 5) {
                specialShape1 = 3;
            } // 这种小块块要是消除了某一行，小块块会多生成一个
            if (currentShape == 7) {
                specialShape1++;
            }

            // 如果当前形状是L行，则会生成1个超级特殊的小块块
            if (currentShape == 4) {
                specialShape2 = true;
            } // 如果超级特殊的小块块消除某一行，则会把最下面的5行清除
            if (currentShape == 8) {
                fillRandomLines(5);
            }

            // 检查分数是否增加了5分
            if (score / 5 > scoreThreshold) {
                scoreThreshold = score / 5;
                fillBottomLine();
            }
        }
    }
    

    // ==================================I形消除处理==================================
    while (!clearedLines.empty()) {
        std::uint32_t clearedLine = clearedLines.front();
        clearedLines.pop();

        if (clearedLine < lines - 2) {
            for (std::size_t j {}; j < cols; ++j) {
                area[clearedLine + 1][j] = 8;
            }
        }
        if (clearedLine > 0) {
            for (std::size_t j {}; j < cols; ++j) {
                area[clearedLine - 1][j] = 8;
            }
        }
    }
}

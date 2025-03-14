#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace sf;

const int TILE_SIZE = 50;

int MAP_SIZE;
Vector2i playerPos;
int playerHealth = 100;
int playerAttack = 15;
int playerDefense = 5;
int enemiesDefeated = 0;

Vector2i enemyPos;
bool enemyExists = true;

vector<Vector2i> healPacks;

void drawMap(RenderWindow &window, Font &font,
            Sprite& tileSprite, Sprite& playerSprite,
            Sprite& enemySprite, Sprite& healSprite)
{
    window.clear(Color::Black);

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            tileSprite.setPosition(Vector2f(x * TILE_SIZE, y * TILE_SIZE));
            tileSprite.setScale(Vector2f(1.0f, 1.0f));
            window.draw(tileSprite);
        }
    }

    playerSprite.setPosition(Vector2f(playerPos.x * TILE_SIZE, playerPos.y * TILE_SIZE));
    playerSprite.setScale(Vector2f(1.0f, 1.0f));
    window.draw(playerSprite);

    if (enemyExists) {
        enemySprite.setPosition(Vector2f(enemyPos.x * TILE_SIZE, enemyPos.y * TILE_SIZE));
        enemySprite.setScale(Vector2f(1.0f, 1.0f));
        window.draw(enemySprite);
    }

    for (auto &heal : healPacks) {
        healSprite.setPosition(Vector2f(heal.x * TILE_SIZE, heal.y * TILE_SIZE));
        healSprite.setScale(Vector2f(1.0f, 1.0f));
        window.draw(healSprite);
    }

    Text info(font, "Здоровье: " + to_string(playerHealth) + " | Побеждено врагов: " + to_string(enemiesDefeated), 20);
    info.setFillColor(Color::White);
    info.setPosition(Vector2f(10, MAP_SIZE * TILE_SIZE + 10));
    window.draw(info);

    window.display();
}

void movePlayer(Vector2i newPos) {
    if (newPos.x >= 0 && newPos.x < MAP_SIZE && newPos.y >= 0 && newPos.y < MAP_SIZE) {
        playerPos = newPos;
        playerHealth -= 3;

        auto it = find_if(healPacks.begin(), healPacks.end(), [&](const Vector2i& pack) {
            return pack.x == playerPos.x && pack.y == playerPos.y;
        });

        if (it != healPacks.end()) {
            int healAmount = rand() % 11 + 10;
            playerHealth += healAmount;
            healPacks.erase(it);
        }
    }
}

void battle() {
    int enemyHealth = rand() % 50 + 30;
    int enemyAttack = rand() % 10 + 5;
    while (playerHealth > 0 && enemyHealth > 0) {
        enemyHealth -= (playerAttack - (rand() % 5));
        if (enemyHealth <= 0) {
            enemiesDefeated++;
            enemyExists = false;
            enemyPos = {rand() % MAP_SIZE, rand() % MAP_SIZE};
            enemyExists = true;

            healPacks.clear();
            int numHeals = rand() % 3 + 1;
            for (int i = 0; i < numHeals; i++) {
                healPacks.push_back({rand() % MAP_SIZE, rand() % MAP_SIZE});
            }
            return;
        }
        playerHealth -= (enemyAttack - playerDefense);
    }
}

// Остальные функции остаются без изменений (movePlayer, battle)

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "Добро пожаловать в приключение Нелианы!" << endl;
    do {
        cout << "Выберите размер карты (от 5 до 10): ";
        cin >> MAP_SIZE;
    } while (MAP_SIZE < 5 || MAP_SIZE > 10);

    RenderWindow window(VideoMode(Vector2u(MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE)), "Приключение Нелианы");
    Font font;
    if (!font.openFromFile("arial.ttf")) {
        cerr << "Ошибка загрузки шрифта!" << endl;
        return -1;
    }

    // Инициализация текстур и спрайтов
    Texture playerTexture, enemyTexture, healTexture, tileTexture;
    if (!playerTexture.loadFromFile("player.png") ||
        !enemyTexture.loadFromFile("enemy.png") ||
        !healTexture.loadFromFile("heal.png") ||
        !tileTexture.loadFromFile("tile.png"))
    {
        cerr << "Ошибка загрузки изображений!" << endl;
        return -1;
    }

    Sprite playerSprite(playerTexture);
    Sprite enemySprite(enemyTexture);
    Sprite healSprite(healTexture);
    Sprite tileSprite(tileTexture);

    srand(time(0));
    playerPos = Vector2i(MAP_SIZE / 2, MAP_SIZE / 2);
    enemyPos = Vector2i(rand() % MAP_SIZE, rand() % MAP_SIZE);

    bool keyProcessed = false;

    while (window.isOpen() && playerHealth > 0) {
        keyProcessed = false;
        while (auto event = window.pollEvent()) {
            event->visit([&](auto&& e) {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, sf::Event::Closed>) {
                    window.close();
                }
                if constexpr (std::is_same_v<T, sf::Event::KeyPressed>) {
                    if (!keyProcessed) {
                        switch (e.code) {
                            case sf::Keyboard::Key::W: movePlayer({playerPos.x, playerPos.y - 1}); break;
                            case sf::Keyboard::Key::S: movePlayer({playerPos.x, playerPos.y + 1}); break;
                            case sf::Keyboard::Key::A: movePlayer({playerPos.x - 1, playerPos.y}); break;
                            case sf::Keyboard::Key::D: movePlayer({playerPos.x + 1, playerPos.y}); break;
                            default: break;
                        }
                        keyProcessed = true;
                    }
                }
            });
        }

        if (enemyExists && playerPos == enemyPos) battle();
        drawMap(window, font, tileSprite, playerSprite, enemySprite, healSprite);
        sf::sleep(sf::milliseconds(50));
    }

    cout << "Игра окончена. Ваши результаты на карте размера " << MAP_SIZE << ": побеждено " << enemiesDefeated << " противников." << endl;
    return 0;
}

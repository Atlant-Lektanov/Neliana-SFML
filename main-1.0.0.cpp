#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const char PLAYER_ICON = 'N';
const char EMPTY_ICON = '.';
const char ENEMY_ICON = 'E';
const char HEAL_ICON = '+';

int MAP_SIZE;
const int TILE_SIZE = 50;

Vector2u playerPos;
int playerHealth = 100;
int playerAttack = 15;
int playerDefense = 5;
int enemiesDefeated = 0;

Vector2u enemyPos;
bool enemyExists = true;

vector<Vector2u> healPacks;

void drawMap(RenderWindow &window, Font &font) {
    window.clear(Color::Black);

    RectangleShape tile(Vector2f(TILE_SIZE, TILE_SIZE));
    tile.setOutlineColor(Color::White);
    tile.setOutlineThickness(1);

    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            tile.setPosition(Vector2f(x * TILE_SIZE, y * TILE_SIZE));
            tile.setFillColor(Color::Black);
            window.draw(tile);
        }
    }

    CircleShape player(TILE_SIZE / 3);
    player.setFillColor(Color::Blue);
    player.setPosition(Vector2f(playerPos.x * TILE_SIZE, playerPos.y * TILE_SIZE));
    window.draw(player);

    if (enemyExists) {
        CircleShape enemy(TILE_SIZE / 3);
        enemy.setFillColor(Color::Red);
        enemy.setPosition(Vector2f(enemyPos.x * TILE_SIZE, enemyPos.y * TILE_SIZE));
        window.draw(enemy);
    }

    for (auto &heal : healPacks) {
        CircleShape healPack(TILE_SIZE / 4);
        healPack.setFillColor(Color::Green);
        healPack.setPosition(Vector2f(heal.x * TILE_SIZE, heal.y * TILE_SIZE));
        window.draw(healPack);
    }

    Text info(font, "Здоровье: " + to_string(playerHealth) + " | Побеждено врагов: " + to_string(enemiesDefeated), 20);
    info.setFillColor(Color::White);
    info.setPosition(Vector2f(10, MAP_SIZE * TILE_SIZE + 10));
    window.draw(info);

    window.display();
}

void movePlayer(Vector2u newPos) {
    if (newPos.x < MAP_SIZE && newPos.y < MAP_SIZE) {
        playerPos = newPos;
        playerHealth -= 3;

        for (auto it = healPacks.begin(); it != healPacks.end(); ) {
            if (playerPos == *it) {
                int healAmount = rand() % 11 + 10;
                playerHealth += healAmount;
                it = healPacks.erase(it);
            } else {
                ++it;
            }
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

    srand(time(0));
    playerPos = {MAP_SIZE / 2, MAP_SIZE / 2};
    enemyPos = {rand() % MAP_SIZE, rand() % MAP_SIZE};

    while (window.isOpen() && playerHealth > 0) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movePlayer({playerPos.x, playerPos.y - 1});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movePlayer({playerPos.x, playerPos.y + 1});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movePlayer({playerPos.x - 1, playerPos.y});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movePlayer({playerPos.x + 1, playerPos.y});

        if (enemyExists && playerPos == enemyPos) battle();
        drawMap(window, font);
    }

    cout << "Игра окончена. Ваши результаты на карте размера " << MAP_SIZE << ": побеждено " << enemiesDefeated << " противников." << endl;
    return 0;
}

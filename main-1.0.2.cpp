#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace sf;

const char PLAYER_ICON = 'N';
const char EMPTY_ICON = '.';
const char ENEMY_ICON = 'E';
const char HEAL_ICON = '+';

int MAP_SIZE;
const int TILE_SIZE = 50;

Vector2i playerPos;
int playerHealth = 100;
int playerAttack = 15;
int playerDefense = 5;
int enemiesDefeated = 0;

Vector2i enemyPos;
bool enemyExists = true;

vector<Vector2i> healPacks;

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

    Text info(font, "��������: " + to_string(playerHealth) + " | ��������� ������: " + to_string(enemiesDefeated), 20);
    info.setFillColor(Color::White);
    info.setPosition(Vector2f(10, MAP_SIZE * TILE_SIZE + 10));
    window.draw(info);

    window.display();
}

void movePlayer(Vector2i newPos) {
    // ��������� ��� �������
    if (newPos.x >= 0 && newPos.x < MAP_SIZE &&
        newPos.y >= 0 && newPos.y < MAP_SIZE)
    {
        playerPos = newPos;
        playerHealth -= 3;

        // ��������� �����
        // ������������ ����� � ������� find_if
        auto it = find_if(healPacks.begin(), healPacks.end(),
            [&](const Vector2i& pack) {
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

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "����� ���������� � ����������� �������!" << endl;
    do {
        cout << "�������� ������ ����� (�� 5 �� 10): ";
        cin >> MAP_SIZE;
    } while (MAP_SIZE < 5 || MAP_SIZE > 10);

    RenderWindow window(VideoMode(Vector2u(MAP_SIZE * TILE_SIZE, MAP_SIZE * TILE_SIZE)), "����������� �������");
    Font font;
    if (!font.openFromFile("arial.ttf")) {
        cerr << "������ �������� ������!" << endl;
        return -1;
    }

    srand(time(0));
    playerPos = Vector2i(MAP_SIZE/2, MAP_SIZE/2);
    enemyPos = Vector2i(rand() % MAP_SIZE, rand() % MAP_SIZE);

    // ��������� ���� ��� ������������ �������
    bool keyProcessed = false;

    while (window.isOpen() && playerHealth > 0) {
        // ��������� �������
        keyProcessed = false; // ���������� ���� ������ ����
        while (auto event = window.pollEvent()) {
            event->visit([&](auto&& e) {
                using T = std::decay_t<decltype(e)>;

                // � ���� visit ��������:
                if constexpr (std::is_same_v<T, sf::Event::KeyReleased>) {
                    keyProcessed = false; // ��������� ����� �������
                }

                if constexpr (std::is_same_v<T, sf::Event::Closed>) {
                    window.close();
                }

                // ������������ ������ ������� ������� �������
                if constexpr (std::is_same_v<T, sf::Event::KeyPressed>) {
                    if (!keyProcessed) { // ��������� ������������� ���������
                        switch (e.code) {
                            case sf::Keyboard::Key::W:
                                movePlayer({playerPos.x, playerPos.y - 1});
                                keyProcessed = true;
                                break;
                            case sf::Keyboard::Key::S:
                                movePlayer({playerPos.x, playerPos.y + 1});
                                keyProcessed = true;
                                break;
                            case sf::Keyboard::Key::A:
                                movePlayer({playerPos.x - 1, playerPos.y});
                                keyProcessed = true;
                                break;
                            case sf::Keyboard::Key::D:
                                movePlayer({playerPos.x + 1, playerPos.y});
                                keyProcessed = true;
                                break;
                            default:
                                break;
                        }
                    }
                }
            });
        }

        // ������ ��������� ������������ ����� ������

        if (enemyExists && playerPos == enemyPos) battle();
        drawMap(window, font);

        // ��������� ��������� �������� ��� ���������
        sf::sleep(sf::milliseconds(50));
    }
    cout << "���� ��������. ���� ���������� �� ����� ������� " << MAP_SIZE << ": ��������� " << enemiesDefeated << " �����������." << endl;
    return 0;
}

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cstdint>

struct character {
    std::string name;
    int life = 0;
    int armor = 0;
    int damage = 0;
    int x = 0;
    int y = 0;
};


void commandCheck(std::string& command) {
    while (command != "L" && command != "R" && command != "U" && command != "D" && command != "save" && command != "load")
    {
        std::cerr << "Wrong command! Try again: ";
        std::cin >> command;
    }
}

void saveGame(const character& player, const std::vector<character>& enemies, const bool(&battleF)[20][20]) {
    std::ofstream file("C:\\tutorial-data\\Game\\save.bin", std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Error opening file for saving!" << std::endl;
        return;
    }

    size_t nameLength = player.name.size();
    file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    file.write(player.name.c_str(), nameLength);

    file.write(reinterpret_cast<const char*>(&player.life), sizeof(player.life));
    file.write(reinterpret_cast<const char*>(&player.armor), sizeof(player.armor));
    file.write(reinterpret_cast<const char*>(&player.damage), sizeof(player.damage));
    file.write(reinterpret_cast<const char*>(&player.x), sizeof(player.x));
    file.write(reinterpret_cast<const char*>(&player.y), sizeof(player.y));

    size_t numEnemies = enemies.size();
    file.write(reinterpret_cast<const char*>(&numEnemies), sizeof(numEnemies));

    for (const auto& enemy : enemies) {
        size_t enemyNameLength = enemy.name.size();
        file.write(reinterpret_cast<const char*>(&enemyNameLength), sizeof(enemyNameLength));
        file.write(enemy.name.c_str(), enemyNameLength);
        file.write(reinterpret_cast<const char*>(&enemy.life), sizeof(enemy.life));
        file.write(reinterpret_cast<const char*>(&enemy.armor), sizeof(enemy.armor));
        file.write(reinterpret_cast<const char*>(&enemy.damage), sizeof(enemy.damage));
        file.write(reinterpret_cast<const char*>(&enemy.x), sizeof(enemy.x));
        file.write(reinterpret_cast<const char*>(&enemy.y), sizeof(enemy.y));
    }

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            uint8_t val = battleF[i][j] ? 1 : 0;
            file.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }
    }

    file.close();
}
void loadGame(character& player, std::vector<character>& enemies, bool(&battleF)[20][20]) {
    std::ifstream file("C:\\tutorial-data\\Game\\save.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for loading!" << std::endl;
        return;
    }

    std::memset(battleF, 0, 20 * 20 * sizeof(bool)); // Очистка поля перед загрузкой
    enemies.clear(); // Очистка списка врагов перед загрузкой

    size_t nameLength;
    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    player.name.resize(nameLength);
    file.read(&player.name[0], nameLength);

    file.read(reinterpret_cast<char*>(&player.life), sizeof(player.life));
    file.read(reinterpret_cast<char*>(&player.armor), sizeof(player.armor));
    file.read(reinterpret_cast<char*>(&player.damage), sizeof(player.damage));
    file.read(reinterpret_cast<char*>(&player.x), sizeof(player.x));
    file.read(reinterpret_cast<char*>(&player.y), sizeof(player.y));

    size_t numEnemies;
    file.read(reinterpret_cast<char*>(&numEnemies), sizeof(numEnemies));
    enemies.resize(numEnemies);

    for (auto& enemy : enemies) {
        size_t enemyNameLength;
        file.read(reinterpret_cast<char*>(&enemyNameLength), sizeof(enemyNameLength));
        enemy.name.resize(enemyNameLength);
        file.read(&enemy.name[0], enemyNameLength);

        file.read(reinterpret_cast<char*>(&enemy.life), sizeof(enemy.life));
        file.read(reinterpret_cast<char*>(&enemy.armor), sizeof(enemy.armor));
        file.read(reinterpret_cast<char*>(&enemy.damage), sizeof(enemy.damage));
        file.read(reinterpret_cast<char*>(&enemy.x), sizeof(enemy.x));
        file.read(reinterpret_cast<char*>(&enemy.y), sizeof(enemy.y));

        battleF[enemy.x][enemy.y] = true; // Восстановление позиции врагов на поле
    }
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            uint8_t val;
            file.read(reinterpret_cast<char*>(&val), sizeof(val));
            battleF[i][j] = (val != 0);
        }
    }

    file.close();
}

void enemyInit(std::vector<character>& enemies, bool(&battleF)[20][20]) {
    for (int i = 0; i < 5; ++i) {
        character enemy;

        enemy.name = "Enemy#" + std::to_string(i + 1);
        enemy.life = 50 + rand() % 101;
        enemy.armor = rand() % 51;
        enemy.damage = rand() % 31;
        int x, y;
        do {
            x = rand() % 20;
            y = rand() % 20;
        } while (battleF[x][y]);
        battleF[x][y] = true;
        enemy.x = x;
        enemy.y = y;
        enemies.push_back(enemy);
    }
}
void playerInit(character& player, bool(&battleF)[20][20], std::string command, std::vector<character>& enemies)
{
    std::cout << "Welcome to battlefield game! Please input-new for new game, or-load to continue last game: " << std::endl;
    std::cin >> command;
    while (command != "load" || command != "new") {
        if (command == "load") {
            loadGame(player, enemies, battleF);
            std::cout << "Game loaded!" << std::endl;
            break;
        }
        else if (command == "new") {
            std::cout << "Input your character name: " << std::endl;
            std::cin >> player.name;
            std::cout << "Input your character life: " << std::endl;
            std::cin >> player.life;
            std::cout << "Input your character armor: " << std::endl;
            std::cin >> player.armor;
            std::cout << "Input your character damage: " << std::endl;
            std::cin >> player.damage;
            int x, y;
            do {
                x = rand() % 20;
                y = rand() % 20;
            } while (battleF[x][y]);
            battleF[x][y] = true;
            player.x = x;
            player.y = y;
            break;
        }
        else {
            std::cerr << "Wrong command. Try again: ";
            std::cin >> command;
        }
    }


}

void enemyTakeDamage(std::vector<character>& enemies, character& player, bool(&battleF)[20][20]) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        int oldX = player.x;
        int oldY = player.y;
        if (enemies[i].x == player.x && enemies[i].y - 1 == player.y ||
            enemies[i].x == player.x && enemies[i].y + 1 == player.y ||
            enemies[i].x + 1 == player.x && enemies[i].y == player.y ||
            enemies[i].x - 1 == player.x && enemies[i].y == player.y) {
            std::cout << player.name << " attack " << enemies[i].name << "!" << std::endl;

            int actualDmg = player.damage;

            if (enemies[i].armor > 0) {
                if (actualDmg <= enemies[i].armor) {
                    enemies[i].armor -= actualDmg;
                    actualDmg = 0;
                }
                else {
                    actualDmg -= enemies[i].armor;
                    enemies[i].armor = 0;
                }
            }

            enemies[i].life -= actualDmg;
            std::cout << enemies[i].name << " takes " << actualDmg << " damage. Lifes left: " << enemies[i].life <<
                ", armor: " << enemies[i].armor << std::endl;
            player.x = oldX;
            player.y = oldY;

            if (enemies[i].life <= 0) {
                std::cout << enemies[i].name << " dead!" << std::endl;
                battleF[enemies[i].x][enemies[i].y] = false;
                enemies.erase(enemies.begin() + i);
                --i;
            }
            return;
        }
    }
}
void playerTakeDamage(character& player, character& enemy) {
    int actualDamage = enemy.damage;

    if (player.armor > 0) {
        if (actualDamage <= player.armor) {
            player.armor -= actualDamage;
            actualDamage = 0;
        }
        else {
            actualDamage -= player.armor;
            player.armor = 0;
        }
    }

    player.life -= actualDamage;
    std::cout << player.name << " takes " << actualDamage << " damage.Lifes left: " << player.life << ", armor: " <<
        player.armor << std::endl;
    if (player.life <= 0) {
        std::cout << player.name << " Is died! Game over!" << std::endl;
    }
}
void characterMove(std::string& command, character& player, bool(&battleF)[20][20], std::vector<character>& enemies) {
    commandCheck(command);

    // Удаляем игрока с текущей позиции
    battleF[player.x][player.y] = false;


    // Перемещение игрока в зависимости от команды
    if (command == "L" && player.y > 0 && !battleF[player.x][player.y - 1]) {
        player.y--;
    }
    else if (command == "R" && player.y <= 19 && !battleF[player.x][player.y + 1]) {
        player.y++;
    }
    else if (command == "U" && player.x > 0 && !battleF[player.x - 1][player.y]) {
        player.x--;
    }
    else if (command == "D" && player.x <= 19 && !battleF[player.x + 1][player.y]) {
        player.x++;
    }
    else if (command == "R" && player.y <= 19 && battleF[player.x][player.y + 1]) {
        enemyTakeDamage(enemies, player, battleF);
    }
    // Добавляем игрока на новую позицию
    battleF[player.x][player.y] = true;
}

void moveTowardsPlayer(character& enemy, const character& player, bool(&battleF)[20][20]) {
    int deltaX = player.x - enemy.x;
    int deltaY = player.y - enemy.y;

    // Определение направления по оси X
    if (abs(deltaX) > abs(deltaY)) {
        if (deltaX > 0 && enemy.x < 19 && !battleF[enemy.x + 1][enemy.y]) { enemy.x++; }
        else if (deltaX < 0 && enemy.x > 0 && !battleF[enemy.x - 1][enemy.y]) { enemy.x--; }
    }
    // Определение направления по оси Y
    else {
        if (deltaY > 0 && enemy.y < 19 && !battleF[enemy.x][enemy.y + 1]) { enemy.y++; }
        else if (deltaY < 0 && enemy.y > 0 && !battleF[enemy.x][enemy.y - 1]) { enemy.y--; }
    }
}
bool isAdjacent(character& player, character& enemy) {
    int dx = abs(player.x - enemy.x);
    int dy = abs(player.y - enemy.y);

    // Враг может атаковать только если находится на клетке по горизонтали или вертикали на расстоянии 1
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}
void enemyMove(character& player, character& enemy, bool(&battleF)[20][20]) {
    int oldX = enemy.x;
    int oldY = enemy.y;

    // Проверяем, был ли враг рядом с игроком до перемещения
    bool wasAdjacentBeforeMove = isAdjacent(player, enemy);

    // Убираем старую позицию врага с карты
    battleF[oldX][oldY] = false;

    // Перемещаем врага к игроку
    moveTowardsPlayer(enemy, player, battleF);

    // Проверяем, если враг находится рядом с игроком после перемещения
    if (wasAdjacentBeforeMove) {
        // Если враг был рядом до перемещения, он атакует
        if (isAdjacent(player, enemy)) {
            std::cout << enemy.name << " attacks " << player.name << "!" << std::endl;
            playerTakeDamage(player, enemy);

            // Откатываем врага на его старое место, если он атакует
            enemy.x = oldX;
            enemy.y = oldY;

            // Обновляем карту: враг возвращается на старую позицию
            battleF[enemy.x][enemy.y] = true;
        }
    }
    else {
        // Если враг не был рядом до перемещения, он просто перемещается и не атакует
        battleF[enemy.x][enemy.y] = true;
    }
}
void moveEnemies(character& player, std::vector<character>& enemies, bool(&battleF)[20][20]) {
    for (auto& enemy : enemies) {
        enemyMove(player, enemy, battleF);
    }
}
void printBf(character& player, bool(&battleF)[20][20]) {
    for (int x = 0; x < 20; ++x) {
        for (int y = 0; y < 20; ++y) {
            if (x == player.x && y == player.y) {
                std::cout << "P";
            }
            else if (battleF[x][y]) {
                std::cout << "E";
            }
            else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

int main()
{
    std::string command;
    bool battleF[20][20] = { false };
    std::vector<character> enemies;
    character player;

    enemyInit(enemies, battleF);
    playerInit(player, battleF, command, enemies);



    std::cout << "Input moving command(L-go left, R-go righ, U-go up, D-go down, save-save game, load-load game)" << std::endl;

    while (player.life > 0 && !enemies.empty()) {
        printBf(player, battleF);

        std::cout << "Input a moving command: ";
        std::cin >> command;

        if (command == "save") {
            saveGame(player, enemies, battleF);
            std::cout << "Game saved!" << std::endl;
        }

        characterMove(command, player, battleF, enemies);
        enemyTakeDamage(enemies, player, battleF);
        moveEnemies(player, enemies, battleF);
    }
    if (player.life < 0) {
        std::cout << "GG , you lose!";
    }
    else if (enemies.empty()) {
        std::cout << "All enemies are dead. GGWP!";
    }


    return  0;
}

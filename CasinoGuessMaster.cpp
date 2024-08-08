#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;

struct Achievement {
    string name;
    string description;
};

struct Player {
    string name;
    int totalScore;
    int gamesPlayed;
    int gamesWon;
    int winStreak;
    bool firstWin;
    vector<string> gameHistory;
    vector<Achievement> achievements;
};

class CasinoNumberGuessingGame {
private:
    unordered_map<string, Player> players;
    Player* currentPlayer;
    int numberToGuess;
    int maxTries;
    unordered_map<string, bool> soundSettings;
    int lastDailyBonus;
    int betAmount;
    unordered_map<string, int> leaderboard;

    void generateRandomNumber(int max) {
        srand(time(0));
        numberToGuess = rand() % max + 1;
    }

    void saveGame() {
        ofstream file("game_data.txt");
        for (const auto& pair : players) {
            file << pair.first << " " << pair.second.totalScore << " " << pair.second.gamesPlayed << " " 
                 << pair.second.gamesWon << " " << pair.second.winStreak << " " << pair.second.firstWin << endl;
            for (const auto& history : pair.second.gameHistory) {
                file << history << endl;
            }
            for (const auto& achievement : pair.second.achievements) {
                file << achievement.name << " " << achievement.description << endl;
            }
        }
        file.close();
    }

    void loadGame() {
        ifstream file("game_data.txt");
        if (file.is_open()) {
            string name;
            while (file >> name) {
                Player player;
                player.name = name;
                file >> player.totalScore >> player.gamesPlayed >> player.gamesWon >> player.winStreak >> player.firstWin;
                string history;
                while (getline(file, history) && !history.empty()) {
                    player.gameHistory.push_back(history);
                }
                string achName, achDesc;
                while (file >> achName >> achDesc) {
                    Achievement ach{achName, achDesc};
                    player.achievements.push_back(ach);
                }
                players[name] = player;
            }
            file.close();
        }
    }

    void saveLeaderboard() {
        ofstream file("leaderboard.txt");
        for (const auto& pair : leaderboard) {
            file << pair.first << " " << pair.second << endl;
        }
        file.close();
    }

    void loadLeaderboard() {
        ifstream file("leaderboard.txt");
        if (file.is_open()) {
            string name;
            int score;
            while (file >> name >> score) {
                leaderboard[name] = score;
            }
            file.close();
        }
    }

    void updateLeaderboard() {
        leaderboard[currentPlayer->name] = currentPlayer->totalScore;
    }

    void addAchievement(const string& achievement) {
        Achievement ach{achievement, "Achieved " + achievement};
        currentPlayer->achievements.push_back(ach);
        cout << "Achievement unlocked: " << achievement << endl;
    }

    void playSound(const string& sound) {
        if (soundSettings[sound]) {
            cout << "Playing sound: " << sound << endl;
        }
    }

    void advancedHints() {
        cout << "Advanced Hint: The number is ";
        if (numberToGuess % 2 == 0) {
            cout << "even." << endl;
        } else {
            cout << "odd." << endl;
        }
    }

    void enterBetAmount() {
        cout << "Enter your bet amount: ";
        cin >> betAmount;
    }

    void dailyBonus() {
        time_t now = time(0);
        tm* localtm = localtime(&now);
        int today = localtm->tm_yday;
        if (today != lastDailyBonus) {
            cout << "Daily Bonus: 100 points!" << endl;
            currentPlayer->totalScore += 100;
            lastDailyBonus = today;
        }
    }

    void playMultiplayerGame() {
        cout << "Enter player names (comma separated): ";
        string playerNames;
        cin.ignore();
        getline(cin, playerNames);
        vector<string> names;
        size_t pos = 0;
        while ((pos = playerNames.find(',')) != string::npos) {
            names.push_back(playerNames.substr(0, pos));
            playerNames.erase(0, pos + 1);
        }
        names.push_back(playerNames);

        generateRandomNumber(100);
        cout << "A number between 1 and 100 has been generated." << endl;

        for (int round = 1; round <= maxTries; ++round) {
            for (const auto& name : names) {
                if (players.find(name) == players.end()) {
                    Player newPlayer;
                    newPlayer.name = name;
                    players[name] = newPlayer;
                }

                Player* player = &players[name];
                cout << "Round " << round << ": " << name << ", enter your guess: ";
                int playerGuess;
                cin >> playerGuess;

                if (playerGuess == numberToGuess) {
                    cout << "Congratulations " << name << "! You've guessed the number!" << endl;
                    playSound("win_sound");
                    player->totalScore += 100;
                    player->gamesWon++;
                    player->totalScore += 100;
                    if (round == 1) {
                        addAchievement("First Attempt Win");
                    }
                    if (player->winStreak >= 3) {
                        addAchievement("Win Streak Master");
                    }
                    if (!player->firstWin) {
                        addAchievement("First Win");
                        player->firstWin = true;
                    }
                    return;
                } else if (playerGuess < numberToGuess) {
                    cout << "The number is higher than " << playerGuess << endl;
                    playSound("low_guess_sound");
                } else {
                    cout << "The number is lower than " << playerGuess << endl;
                    playSound("high_guess_sound");
                }
            }
        }

        cout << "Sorry, no one guessed the number. The number was " << numberToGuess << "." << endl;
        playSound("lose_sound");
    }

public:
    CasinoNumberGuessingGame() : maxTries(5), lastDailyBonus(0), betAmount(0) {
        loadGame();
        loadLeaderboard();
        soundSettings = {
            {"win_sound", true},
            {"low_guess_sound", true},
            {"high_guess_sound", true},
            {"lose_sound", true}
        };
    }

    ~CasinoNumberGuessingGame() {
        saveGame();
        saveLeaderboard();
    }

    void setDifficulty() {
        int choice;
        cout << "Select Difficulty Level: " << endl;
        cout << "1. Easy (1-10)" << endl;
        cout << "2. Medium (1-50)" << endl;
        cout << "3. Hard (1-100)" << endl;
        cin >> choice;
        switch (choice) {
            case 1:
                generateRandomNumber(10);
                maxTries = 5;
                break;
            case 2:
                generateRandomNumber(50);
                maxTries = 7;
                break;
            case 3:
                generateRandomNumber(100);
                maxTries = 10;
                break;
            default:
                cout << "Invalid choice. Setting to Easy." << endl;
                generateRandomNumber(10);
                maxTries = 5;
        }
    }

    void playGame() {
        cout << "Enter your name: ";
        string name;
        cin >> name;

        if (players.find(name) == players.end()) {
            Player newPlayer;
            newPlayer.name = name;
            players[name] = newPlayer;
        }

        currentPlayer = &players[name];
        currentPlayer->gamesPlayed++;
        dailyBonus();
        enterBetAmount();
        auto start = high_resolution_clock::now();

        for (int attempt = 1; attempt <= maxTries; ++attempt) {
            cout << "Attempt " << attempt << ": Enter your guess: ";
            int playerGuess;
            cin >> playerGuess;

            if (playerGuess == numberToGuess) {
                cout << "Congratulations! You've guessed the number!" << endl;
                playSound("win_sound");
                currentPlayer->totalScore += betAmount * 2;
                currentPlayer->gamesWon++;
                currentPlayer->totalScore += betAmount * 2;
                if (attempt == 1) {
                    addAchievement("First Attempt Win");
                }
                if (currentPlayer->winStreak >= 3) {
                    addAchievement("Win Streak Master");
                }
                if (!currentPlayer->firstWin) {
                    addAchievement("First Win");
                    currentPlayer->firstWin = true;
                }
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<seconds>(stop - start).count();
                currentPlayer->gameHistory.push_back("Win in " + to_string(duration) + " seconds.");
                updateLeaderboard();
                saveGame();
                saveLeaderboard();
                return;
            } else if (playerGuess < numberToGuess) {
                cout << "The number is higher than " << playerGuess << endl;
                playSound("low_guess_sound");
            } else {
                cout << "The number is lower than " << playerGuess << endl;
                playSound("high_guess_sound");
            }
        }

        cout << "Sorry, you've used all your attempts. The number was " << numberToGuess << "." << endl;
        playSound("lose_sound");
        currentPlayer->winStreak = 0;
        currentPlayer->totalScore -= betAmount;
        updateLeaderboard();
        saveGame();
        saveLeaderboard();
    }

    void viewAchievements() {
        cout << "Achievements for " << currentPlayer->name << ":" << endl;
        for (const auto& achievement : currentPlayer->achievements) {
            cout << achievement.name << ": " << achievement.description << endl;
        }
    }

    void viewStatistics() {
        cout << "Statistics for " << currentPlayer->name << ":" << endl;
        cout << "Total Score: " << currentPlayer->totalScore << endl;
        cout << "Games Played: " << currentPlayer->gamesPlayed << endl;
        cout << "Games Won: " << currentPlayer->gamesWon << endl;
        cout << "Win Streak: " << currentPlayer->winStreak << endl;
    }

    void viewLeaderboard() {
        cout << "Global Leaderboard:" << endl;
        vector<pair<int, string>> sortedLeaderboard;
        for (const auto& pair : leaderboard) {
            sortedLeaderboard.push_back(make_pair(pair.second, pair.first));
        }
        sort(sortedLeaderboard.rbegin(), sortedLeaderboard.rend());
        for (const auto& pair : sortedLeaderboard) {
            cout << pair.second << ": " << pair.first << " points" << endl;
        }
    }

    void setSoundSettings() {
        cout << "Sound Settings:" << endl;
        for (auto& pair : soundSettings) {
            cout << pair.first << " (1=on, 0=off): ";
            cin >> pair.second;
        }
    }

    void advancedOptions() {
        int choice;
        cout << "Advanced Options:" << endl;
        cout << "1. Enable/Disable Sound" << endl;
        cout << "2. Advanced Hints" << endl;
        cin >> choice;
        switch (choice) {
            case 1:
                setSoundSettings();
                break;
            case 2:
                advancedHints();
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }

    void showMenu() {
        int choice;
        do {
            cout << "1. Play Game" << endl;
            cout << "2. Set Difficulty" << endl;
            cout << "3. View Achievements" << endl;
            cout << "4. View Statistics" << endl;
            cout << "5. View Leaderboard" << endl;
            cout << "6. Advanced Options" << endl;
            cout << "7. Play Multiplayer Game" << endl;
            cout << "8. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
                case 1:
                    playGame();
                    break;
                case 2:
                    setDifficulty();
                    break;
                case 3:
                    viewAchievements();
                    break;
                case 4:
                    viewStatistics();
                    break;
                case 5:
                    viewLeaderboard();
                    break;
                case 6:
                    advancedOptions();
                    break;
                case 7:
                    playMultiplayerGame();
                    break;
                case 8:
                    cout << "Exiting game. Goodbye!" << endl;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        } while (choice != 8);
    }
};

int main() {
    CasinoNumberGuessingGame game;
    game.showMenu();
    return 0;
}

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

// --- PROJECT CONSTANTS ---
const int MAX_STR_LEN = 256;
const int MAX_QUESTIONS_BANK = 100;
const int QUESTIONS_PER_QUIZ = 10;
const int TIME_PER_QUESTION = 10;
const int MAX_HIGH_SCORES = 5;


// --- GLOBAL VARIABLES (Parallel Arrays) ---
char qStatements[MAX_QUESTIONS_BANK][MAX_STR_LEN];
char qOptionA[MAX_QUESTIONS_BANK][MAX_STR_LEN];
char qOptionB[MAX_QUESTIONS_BANK][MAX_STR_LEN];
char qOptionC[MAX_QUESTIONS_BANK][MAX_STR_LEN];
char qOptionD[MAX_QUESTIONS_BANK][MAX_STR_LEN];
int qCorrectAnswer[MAX_QUESTIONS_BANK];
int qDifficulty[MAX_QUESTIONS_BANK];

int totalQuestionsLoaded = 0;
int quizQuestionsIndices[QUESTIONS_PER_QUIZ];

// Session tracking arrays
int userAnswers[QUESTIONS_PER_QUIZ];
int correctAnswersSession[QUESTIONS_PER_QUIZ];
bool wasCorrect[QUESTIONS_PER_QUIZ];

// High Score Data
char hsNames[MAX_HIGH_SCORES][MAX_STR_LEN];
int hsScores[MAX_HIGH_SCORES];
char hsDifficulty[MAX_HIGH_SCORES][20];
char hsDate[MAX_HIGH_SCORES][30];

// Lifeline tracking
bool lifeline5050Used = false;
bool lifelineSkipUsed = false;
bool lifelineSwapUsed = false;
bool lifelineTimeUsed = false;

// --- UTILITY FUNCTIONS ---

int getStringLength(const char s[])
{
    int length = 0;
    while (s[length] != '\0') length++;
    return length;
}

void copyString(char dest[], const char src[])
{
    int i = 0;
    while (src[i] != '\0' && i < MAX_STR_LEN - 1)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void appendString(char dest[], const char src[])
{
    int i = 0;
    while (dest[i] != '\0') i++;
    int j = 0;
    while (src[j] != '\0' && i < MAX_STR_LEN - 1)
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';
}

int stringToInt(const char str[])
{
    int result = 0;
    int i = 0;
    bool isNegative = false;

    if (str[0] == '-')
    {
        isNegative = true;
        i = 1;
    }

    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        i++;
    }

    return isNegative ? -result : result;
}

void getCurrentDate(char dateStr[])
{
    time_t now = time(0);
    char temp[30];
    int idx = 0;

    // Simple date format: DD/MM/YYYY
    copyString(dateStr, "07/12/2025");
}



// --- HIGH SCORE LOGIC ---

void sortHighScores()
{
    for (int i = 0; i < MAX_HIGH_SCORES - 1; i++)
    {
        for (int j = 0; j < MAX_HIGH_SCORES - 1 - i; j++)
        {
            if (hsScores[j] < hsScores[j + 1])
            {
                // Swap Score
                int tempScore = hsScores[j];
                hsScores[j] = hsScores[j + 1];
                hsScores[j + 1] = tempScore;

                // Swap Name
                char tempName[MAX_STR_LEN];
                copyString(tempName, hsNames[j]);
                copyString(hsNames[j], hsNames[j + 1]);
                copyString(hsNames[j + 1], tempName);

                // Swap Difficulty
                char tempDiff[20];
                copyString(tempDiff, hsDifficulty[j]);
                copyString(hsDifficulty[j], hsDifficulty[j + 1]);
                copyString(hsDifficulty[j + 1], tempDiff);

                // Swap Date
                char tempDate[30];
                copyString(tempDate, hsDate[j]);
                copyString(hsDate[j], hsDate[j + 1]);
                copyString(hsDate[j + 1], tempDate);
            }
        }
    }
}

void loadHighScores()
{
    ifstream file("high_scores.txt");
    if (!file.is_open())
    {
        for (int i = 0; i < MAX_HIGH_SCORES; i++)
        {
            copyString(hsNames[i], "Empty");
            hsScores[i] = 0;
            copyString(hsDifficulty[i], "N/A");
            copyString(hsDate[i], "N/A");
        }
        return;
    }

    int i = 0;
    while (i < MAX_HIGH_SCORES)
    {
        char line[MAX_STR_LEN];
        if (!file.getline(line, MAX_STR_LEN)) break;

        // Parse line: Name|Date|Score|Difficulty
        int part = 0;
        int charIdx = 0;
        char tempName[MAX_STR_LEN] = "";
        char tempDate[30] = "";
        char tempScore[10] = "";
        char tempDiff[20] = "";

        int nameIdx = 0, dateIdx = 0, scoreIdx = 0, diffIdx = 0;

        while (line[charIdx] != '\0')
        {
            if (line[charIdx] == '|')
            {
                part++;
                charIdx++;
                continue;
            }

            if (part == 0) tempName[nameIdx++] = line[charIdx];
            else if (part == 1) tempDate[dateIdx++] = line[charIdx];
            else if (part == 2) tempScore[scoreIdx++] = line[charIdx];
            else if (part == 3) tempDiff[diffIdx++] = line[charIdx];

            charIdx++;
        }

        tempName[nameIdx] = '\0';
        tempDate[dateIdx] = '\0';
        tempScore[scoreIdx] = '\0';
        tempDiff[diffIdx] = '\0';

        copyString(hsNames[i], tempName);
        copyString(hsDate[i], tempDate);
        copyString(hsDifficulty[i], tempDiff);
        hsScores[i] = stringToInt(tempScore);

        i++;
    }
    file.close();
    sortHighScores();
}

void saveHighScores()
{
    ofstream file("high_scores.txt");
    if (!file.is_open()) return;

    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        file << hsNames[i] << "|" << hsDate[i] << "|"
            << hsScores[i] << "|" << hsDifficulty[i] << "\n";
    }
    file.close();
}

void updateHighScores(int newScore, const char playerName[], const char difficulty[])
{
    if (newScore > hsScores[MAX_HIGH_SCORES - 1])
    {
        hsScores[MAX_HIGH_SCORES - 1] = newScore;
        copyString(hsNames[MAX_HIGH_SCORES - 1], playerName);
        copyString(hsDifficulty[MAX_HIGH_SCORES - 1], difficulty);
        getCurrentDate(hsDate[MAX_HIGH_SCORES - 1]);
        sortHighScores();
        saveHighScores();
        cout << "\n*** CONGRATULATIONS! New High Score! ***" << endl;
    }
}

void viewHighScores()
{
    clearScreen();
    cout << "\n============================================" << endl;
    cout << "          QUIZMASTER LEADERBOARD            " << endl;
    cout << "============================================" << endl;
    cout << "Rank | Name | Date | Score | Difficulty" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        cout << (i + 1) << ". " << hsNames[i] << " | "
            << hsDate[i] << " | " << hsScores[i] << " pts | "
            << hsDifficulty[i] << endl;
    }
    cout << "============================================" << endl;
    cout << "\nPress ENTER to return...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// --- FILE LOADING & SELECTION ---

void loadQuestions(const char filename[], int selectedDifficulty)
{
    totalQuestionsLoaded = 0;
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "\nError: Could not open file: " << filename << endl;
        return;
    }

    while (totalQuestionsLoaded < MAX_QUESTIONS_BANK)
    {
        char diffLine[10];
        if (!file.getline(diffLine, 10)) break;
        int difficulty = stringToInt(diffLine);

        // Filter by difficulty
        if (difficulty != selectedDifficulty)
        {
            // Skip this question
            char temp[MAX_STR_LEN];
            file.getline(temp, MAX_STR_LEN); // Question
            file.getline(temp, MAX_STR_LEN); // A
            file.getline(temp, MAX_STR_LEN); // B
            file.getline(temp, MAX_STR_LEN); // C
            file.getline(temp, MAX_STR_LEN); // D
            file.getline(temp, MAX_STR_LEN); // Answer
            continue;
        }

        int i = totalQuestionsLoaded;
        qDifficulty[i] = difficulty;

        if (!file.getline(qStatements[i], MAX_STR_LEN)) break;
        if (!file.getline(qOptionA[i], MAX_STR_LEN)) break;
        if (!file.getline(qOptionB[i], MAX_STR_LEN)) break;
        if (!file.getline(qOptionC[i], MAX_STR_LEN)) break;
        if (!file.getline(qOptionD[i], MAX_STR_LEN)) break;

        char answerStr[10];
        if (!file.getline(answerStr, 10)) break;

        if (answerStr[0] >= '1' && answerStr[0] <= '4')
        {
            qCorrectAnswer[i] = answerStr[0] - '0';
            totalQuestionsLoaded++;
        }
    }
    file.close();
}

void selectQuizQuestions()
{
    if (totalQuestionsLoaded == 0) return;
    int used[MAX_QUESTIONS_BANK] = { 0 };
    int count = 0;
    int maxQs = (totalQuestionsLoaded < QUESTIONS_PER_QUIZ) ? totalQuestionsLoaded : QUESTIONS_PER_QUIZ;

    while (count < maxQs)
    {
        int randomIndex = rand() % totalQuestionsLoaded;
        if (used[randomIndex] == 0)
        {
            quizQuestionsIndices[count] = randomIndex;
            used[randomIndex] = 1;
            count++;
        }
    }
}

// --- LIFELINE FUNCTIONS ---

void apply5050(int idx, int correctAns)
{
    cout << "\n*** 50/50 LIFELINE ACTIVATED ***" << endl;
    cout << "Two incorrect options removed!" << endl;

    int removed = 0;
    int hideOptions[2];
    int hideIdx = 0;

    for (int i = 1; i <= 4 && removed < 2; i++)
    {
        if (i != correctAns)
        {
            hideOptions[hideIdx++] = i;
            removed++;
        }
    }

    cout << "\nRemaining options:" << endl;
    for (int i = 1; i <= 4; i++)
    {
        bool hide = false;
        for (int j = 0; j < hideIdx; j++)
        {
            if (i == hideOptions[j])
            {
                hide = true;
                break;
            }
        }

        if (!hide)
        {
            if (i == 1) cout << "1. " << qOptionA[idx] << endl;
            else if (i == 2) cout << "2. " << qOptionB[idx] << endl;
            else if (i == 3) cout << "3. " << qOptionC[idx] << endl;
            else if (i == 4) cout << "4. " << qOptionD[idx] << endl;
        }
    }
}

// --- SESSION LOGGING ---

void logSession(const char playerName[], int finalScore, int correct, int wrong, const char difficulty[])
{
    ofstream file("quiz_logs.txt", ios::app);
    if (!file.is_open()) return;

    char dateStr[30];
    getCurrentDate(dateStr);

    file << "=== Quiz Session ===" << "\n";
    file << "Date: " << dateStr << "\n";
    file << "Player: " << playerName << "\n";
    file << "Difficulty: " << difficulty << "\n";
    file << "Correct: " << correct << "\n";
    file << "Wrong: " << wrong << "\n";
    file << "Final Score: " << finalScore << "\n";
    file << "===================" << "\n\n";

    file.close();
}

// --- REVIEW MODE ---

void reviewMode(int questionsAttempted)
{
    cout << "\n============================================" << endl;
    cout << "          REVIEW INCORRECT ANSWERS          " << endl;
    cout << "============================================" << endl;

    bool foundIncorrect = false;

    for (int i = 0; i < questionsAttempted; i++)
    {
        if (!wasCorrect[i])
        {
            foundIncorrect = true;
            int idx = quizQuestionsIndices[i];

            cout << "\nQuestion " << (i + 1) << ":" << endl;
            cout << qStatements[idx] << endl;
            cout << "1. " << qOptionA[idx] << endl;
            cout << "2. " << qOptionB[idx] << endl;
            cout << "3. " << qOptionC[idx] << endl;
            cout << "4. " << qOptionD[idx] << endl;
            cout << "\nYour Answer: " << userAnswers[i] << endl;
            cout << "Correct Answer: " << correctAnswersSession[i] << endl;
            cout << "--------------------------------------------" << endl;
        }
    }

    if (!foundIncorrect)
    {
        cout << "\nPerfect! You answered all questions correctly!" << endl;
    }

    cout << "\nPress ENTER to continue...";
    cin.get();
}

// --- GAME LOOP ---

void startQuiz(const char category[], int difficultyLevel, int scoreMultiplier, const char difficultyName[], int negativeMarking)
{
    char filename[MAX_STR_LEN];
    copyString(filename, category);
    appendString(filename, ".txt");

    cout << "\nLoading questions from " << category << " category..." << endl;
    loadQuestions(filename, difficultyLevel);

    if (totalQuestionsLoaded < QUESTIONS_PER_QUIZ)
    {
        cout << "\nError: Not enough questions for " << difficultyName << " difficulty." << endl;
        cout << "Found only " << totalQuestionsLoaded << " questions." << endl;
        cout << "Press ENTER to return...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return;
    }

    char playerName[MAX_STR_LEN];
    cout << "\nEnter your name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(playerName, MAX_STR_LEN);

    // Reset lifelines
    lifeline5050Used = false;
    lifelineSkipUsed = false;
    lifelineSwapUsed = false;
    lifelineTimeUsed = false;

    selectQuizQuestions();

    int totalScore = 0;
    int correctCount = 0;
    int wrongCount = 0;
    int consecutiveCorrect = 0;

    cout << "\n*** Quiz Starting! ***" << endl;
    cout << "Time limit: " << TIME_PER_QUESTION << " seconds per question." << endl;
    cout << "Press ENTER to begin...";
    cin.get();

    for (int i = 0; i < QUESTIONS_PER_QUIZ; i++)
    {
        clearScreen();
        int idx = quizQuestionsIndices[i];

        cout << "\n============================================" << endl;
        cout << " Question " << (i + 1) << "/" << QUESTIONS_PER_QUIZ << " | Score: " << totalScore << endl;
        cout << " Streak: " << consecutiveCorrect << " | Difficulty: " << difficultyName << endl;
        cout << "============================================" << endl;
        cout << qStatements[idx] << "\n" << endl;
        cout << "1. " << qOptionA[idx] << endl;
        cout << "2. " << qOptionB[idx] << endl;
        cout << "3. " << qOptionC[idx] << endl;
        cout << "4. " << qOptionD[idx] << "\n" << endl;

        // Display available lifelines
        cout << "Lifelines: ";
        if (!lifeline5050Used) cout << "[5] 50/50 ";
        if (!lifelineSkipUsed) cout << "[6] Skip ";
        if (!lifelineSwapUsed) cout << "[7] Swap ";
        if (!lifelineTimeUsed) cout << "[8] Time+10 ";
        cout << "\n" << endl;

        cout << "Enter answer (1-4) or lifeline: ";

        clock_t startTime = clock();
        int playerAnswer = 0;
        bool valid = false;
        bool skipped = false;
        int timeBonus = 0;

        if (cin >> playerAnswer)
        {
            // Check for lifeline usage
            if (playerAnswer == 5 && !lifeline5050Used)
            {
                lifeline5050Used = true;
                apply5050(idx, qCorrectAnswer[idx]);
                cout << "\nEnter your answer (1-4): ";
                cin >> playerAnswer;
                if (playerAnswer >= 1 && playerAnswer <= 4) valid = true;
            }
            else if (playerAnswer == 6 && !lifelineSkipUsed)
            {
                lifelineSkipUsed = true;
                cout << "\n*** Question Skipped (No Penalty) ***" << endl;
                skipped = true;
                userAnswers[i] = 0;
                correctAnswersSession[i] = qCorrectAnswer[idx];
                wasCorrect[i] = false;
            }
            else if (playerAnswer == 7 && !lifelineSwapUsed)
            {
                lifelineSwapUsed = true;
                cout << "\n*** Question Swapped! ***" << endl;

                // Find a new random question not in current quiz
                int newIdx = rand() % totalQuestionsLoaded;
                quizQuestionsIndices[i] = newIdx;

                cout << "Press ENTER for new question...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                i--; // Repeat this iteration
                continue;
            }
            else if (playerAnswer == 8 && !lifelineTimeUsed)
            {
                lifelineTimeUsed = true;
                timeBonus = 10;
                cout << "\n*** +10 Seconds Added! ***" << endl;
                cout << "Enter your answer (1-4): ";
                cin >> playerAnswer;
                if (playerAnswer >= 1 && playerAnswer <= 4) valid = true;
            }
            else if (playerAnswer >= 1 && playerAnswer <= 4)
            {
                valid = true;
            }
            else
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        clock_t endTime = clock();
        double elapsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;

        if (!skipped)
        {
            if (elapsed > (TIME_PER_QUESTION + timeBonus))
            {
                cout << "\n*** TIME'S UP! ***" << endl;
                wrongCount++;
                totalScore -= negativeMarking;
                consecutiveCorrect = 0;
                userAnswers[i] = 0;
                correctAnswersSession[i] = qCorrectAnswer[idx];
                wasCorrect[i] = false;
            }
            else if (valid)
            {
                userAnswers[i] = playerAnswer;
                correctAnswersSession[i] = qCorrectAnswer[idx];

                if (playerAnswer == qCorrectAnswer[idx])
                {
                    cout << "\n*** CORRECT! ***" << endl;
                    correctCount++;
                    totalScore += scoreMultiplier;
                    consecutiveCorrect++;
                    wasCorrect[i] = true;

                    // Streak bonuses
                    if (consecutiveCorrect == 3)
                    {
                        totalScore += 5;
                        cout << "*** 3-STREAK BONUS: +5 points! ***" << endl;
                    }
                    else if (consecutiveCorrect == 5)
                    {
                        totalScore += 15;
                        cout << "*** 5-STREAK BONUS: +15 points! ***" << endl;
                    }
                }
                else
                {
                    cout << "\n*** WRONG! ***" << endl;
                    cout << "Correct answer was: " << qCorrectAnswer[idx] << endl;
                    wrongCount++;
                    totalScore -= negativeMarking;
                    consecutiveCorrect = 0;
                    wasCorrect[i] = false;
                }
            }
            else
            {
                cout << "\n*** INVALID INPUT! ***" << endl;
                wrongCount++;
                totalScore -= negativeMarking;
                consecutiveCorrect = 0;
                userAnswers[i] = 0;
                correctAnswersSession[i] = qCorrectAnswer[idx];
                wasCorrect[i] = false;
            }
        }

        cout << "\nPress ENTER to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    int finalScore = (totalScore > 0 ? totalScore : 0);

    clearScreen();
    cout << "\n=== QUIZ COMPLETED ===" << endl;
    cout << "Player: " << playerName << endl;
    cout << "Difficulty: " << difficultyName << endl;
    cout << "Correct Answers: " << correctCount << endl;
    cout << "Wrong Answers: " << wrongCount << endl;
    cout << "Final Score: " << finalScore << " points" << endl;
    cout << "========================" << endl;

    logSession(playerName, finalScore, correctCount, wrongCount, difficultyName);
    updateHighScores(finalScore, playerName, difficultyName);

    cout << "\nWould you like to review incorrect answers? (1=Yes, 2=No): ";
    int reviewChoice;
    cin >> reviewChoice;

    if (reviewChoice == 1)
    {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        reviewMode(QUESTIONS_PER_QUIZ);
    }

    cout << "\nPress ENTER to return to main menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// --- MENUS ---

void selectDifficulty(const char category[])
{
    int choice;
    while (true)
    {
        clearScreen();
        cout << "\n============================================" << endl;
        cout << "        SELECT DIFFICULTY LEVEL             " << endl;
        cout << "============================================" << endl;
        cout << "Category: " << category << endl;
        cout << "--------------------------------------------" << endl;
        cout << "1. Easy   (10 pts per Q, -2 for wrong)" << endl;
        cout << "2. Medium (20 pts per Q, -3 for wrong)" << endl;
        cout << "3. Hard   (30 pts per Q, -5 for wrong)" << endl;
        cout << "4. Back to Categories" << endl;
        cout << "============================================" << endl;
        cout << "Option: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 4) return;

        if (choice == 1)
        {
            startQuiz(category, 1, 10, "Easy", 2);
            return;
        }
        else if (choice == 2)
        {
            startQuiz(category, 2, 20, "Medium", 3);
            return;
        }
        else if (choice == 3)
        {
            startQuiz(category, 3, 30, "Hard", 5);
            return;
        }
        else
        {
            cout << "Invalid choice. Try again." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    }
}

void selectCategory()
{
    int choice;
    while (true)
    {
        clearScreen();
        cout << "\n============================================" << endl;
        cout << "           SELECT QUIZ CATEGORY             " << endl;
        cout << "============================================" << endl;
        cout << "1. Science" << endl;
        cout << "2. Sports" << endl;
        cout << "3. History" << endl;
        cout << "4. Computer Science" << endl;
        cout << "5. IQ & Logic" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "============================================" << endl;
        cout << "Option: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice)
        {
        case 1: selectDifficulty("science"); break;
        case 2: selectDifficulty("sports"); break;
        case 3: selectDifficulty("history"); break;
        case 4: selectDifficulty("computer"); break;
        case 5: selectDifficulty("iq"); break;
        case 6: return;
        default:
            cout << "Invalid category. Try again." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    }
}

void displayMenu()
{
    int choice;
    while (true)
    {
        clearScreen();
        cout << "\n============================================" << endl;
        cout << "          WELCOME TO QUIZMASTER             " << endl;
        cout << "       Console-Based Quiz Game           " << endl;
        cout << "============================================" << endl;
        cout << "1. Start New Quiz" << endl;
        cout << "2. View Leaderboard" << endl;
        cout << "3. Exit Game" << endl;
        cout << "============================================" << endl;
        cout << "Option: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice)
        {
        case 1:
            selectCategory();
            break;
        case 2:
            viewHighScores();
            break;
        case 3:
            clearScreen();
            cout << "\n============================================" << endl;
            cout << "     Thank you for playing QUIZMASTER!     " << endl;
            cout << "============================================" << endl;
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    }
}

int main()
{
    srand(time(0));
    loadHighScores();
    displayMenu();
    return 0;
}
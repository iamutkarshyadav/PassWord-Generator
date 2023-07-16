#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;

class PasswordGenerator
{
private:
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    int length;

public:
    PasswordGenerator(int length) : length(length)
    {
        srand(time(0));
    }

    string generatePassword()
    {
        string password;

        for (int i = 0; i < length; ++i)
        {
            int index = rand() % charset.length();
            password += charset[index];
        }

        return password;
    }
};

class PasswordStrengthChecker
{
private:
    string password;
    double lengthScore;
    double complexityScore;
    double randomnessScore;
    double dictionaryScore;
    double substitutionScore;
    double personalScore;
    double reusedScore;
    double patternScore;

    const string commonSubstitutions[5][2] = {
        {"a", "@"},
        {"e", "3"},
        {"i", "1"},
        {"o", "0"},
        {"s", "$"}};

    const string personalInfo[9] = {
        "password", "123456", "qwerty", "abc123", "letmein",
        "monkey", "football", "iloveyou", "admin"};

    const string reusedPasswords[5] = {
        "password1", "password2", "password3", "password4", "password5"};

public:
    PasswordStrengthChecker(string password) : password(password)
    {
        double length = password.length();
        lengthScore = 10 * (1 - exp(-length / 10));

        bool uppercase = false;
        bool lowercase = false;
        bool digit = false;
        bool specialChar = false;
        for (char c : password)
        {
            if (isupper(c))
            {
                uppercase = true;
            }
            else if (islower(c))
            {
                lowercase = true;
            }
            else if (isdigit(c))
            {
                digit = true;
            }
            else if (!isalnum(c))
            {
                specialChar = true;
            }
        }

        complexityScore = 0;
        if (uppercase)
        {
            complexityScore += 2;
        }
        if (lowercase)
        {
            complexityScore += 2;
        }
        if (digit)
        {
            complexityScore += 2;
        }
        if (specialChar)
        {
            complexityScore += 4;
        }

        double entropy = 0;
        if (length > 0)
        {
            string uniqueChars;
            for (char c : password)
            {
                if (uniqueChars.find(c) == string::npos)
                {
                    uniqueChars += c;
                }
            }
            double characterSetSize = uniqueChars.length();
            entropy = length * log2(characterSetSize);
        }
        randomnessScore = (entropy / 128) * 100;

        dictionaryScore = 0;
        ifstream file("dictionary.txt");
        string line;
        while (getline(file, line))
        {
            if (password.find(line) != string::npos)
            {
                dictionaryScore = -20;
                break;
            }
        }
        file.close();

        substitutionScore = 0;
        for (const auto &entry : commonSubstitutions)
        {
            if (password.find(entry[0]) != string::npos ||
                password.find(entry[1]) != string::npos ||
                password.find(toupper(entry[1][0])) != string::npos)
            {
                substitutionScore -= 10;
                break;
            }
        }

        string lowercasePassword = password;
        transform(lowercasePassword.begin(), lowercasePassword.end(), lowercasePassword.begin(), ::tolower);
        personalScore = 0;
        for (const string &info : personalInfo)
        {
            if (lowercasePassword == info)
            {
                personalScore -= 20;
                break;
            }
        }

        reusedScore = 0;
        for (const string &reused : reusedPasswords)
        {
            if (lowercasePassword == reused)
            {
                reusedScore -= 10;
                break;
            }
        }

        patternScore = password.find_first_of("0123456789") == string::npos ||
                               password.find_first_of("abcdefghijklmnopqrstuvwxyz") == string::npos ||
                               password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos ||
                               password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == string::npos
                           ? -10
                           : 0;
    }

    double calculateScore()
    {
        return min(round(lengthScore + complexityScore + randomnessScore +
                         dictionaryScore + substitutionScore + personalScore +
                         reusedScore + patternScore),
                   100.0);
    }
};

void displayStrengthBar(double strength)
{
    const int totalWidth = 50;

    int filledChars = static_cast<int>(strength * (totalWidth / 100.0));
    string strengthBar = "[";

    for (int i = 0; i < filledChars; ++i)
    {
        strengthBar += "=";
    }

    for (int i = filledChars; i < totalWidth; ++i)
    {
        strengthBar += " ";
    }

    strengthBar += "]";
    cout << strengthBar << " " << strength << "%\n";
}

int main()
{
    string password;
    cout << "Enter the password (or press '1' to generate a random password):\n ";
    cin >> password;

    if (password == "1")
    {
        int passwordLength;
        cout << "Enter the length of the generated password: \n";
        cin >> passwordLength;

        if (passwordLength <= 0)
        {
            cout << "Invalid password length. Please enter a positive integer.\n";
            return 1;
        }

        PasswordGenerator generator(passwordLength);
        password = generator.generatePassword();
        cout << "Generated Password: " << password << endl;
    }
    else
    {
        PasswordStrengthChecker checker(password);
        double strength = checker.calculateScore();
        cout << "Password strength: ";
        displayStrengthBar(strength);
    }

    return 0;
}

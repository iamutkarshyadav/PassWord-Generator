#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <ctime>
using namespace std;

string generate_password(int length)
{
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    string password;
    srand(time(0));

    for (int i = 0; i < length; ++i)
    {
        int index = rand() % charset.length();
        password += charset[index];
    }

    return password;
}

double password_strength(string password)
{

    double length = password.length();
    double length_score = 10 * (1 - exp(-length / 10));

    bool uppercase = false;
    bool lowercase = false;
    bool digit = false;
    bool special_char = false;
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
            special_char = true;
        }
    }
    double complexity_score = 0;
    if (uppercase)
    {
        complexity_score += 2;
    }
    if (lowercase)
    {
        complexity_score += 2;
    }
    if (digit)
    {
        complexity_score += 2;
    }
    if (special_char)
    {
        complexity_score += 4;
    }

    double entropy = 0;
    if (length > 0)
    {
        unordered_set<char> character_set(password.begin(), password.end());
        double character_set_size = character_set.size();
        entropy = length * log2(character_set_size);
    }
    double randomness_score = (entropy / 128) * 100;
    unordered_set<string> dictionary;
    ifstream file("dictionary.txt");
    string line;
    while (getline(file, line))
    {
        dictionary.insert(line);
    }
    file.close();
    unordered_set<string> words;
    size_t pos = 0;
    string delimiter = " ";
    while ((pos = password.find(delimiter)) != string::npos)
    {
        words.insert(password.substr(0, pos));
        password.erase(0, pos + delimiter.length());
    }
    words.insert(password);
    double dictionary_score = 0;
    for (const string &word : words)
    {
        if (dictionary.count(word) > 0)
        {
            dictionary_score = -20;
            break;
        }
    }
    unordered_map<char, string> common_substitutions = {
        {'a', "@"},
        {'e', "3"},
        {'i', "1"},
        {'o', "0"},
        {'s', "$"}};
    double substitution_score = 0;
    for (const auto &entry : common_substitutions)
    {
        if (password.find(entry.first) != string::npos ||
            password.find(entry.second) != string::npos ||
            password.find(toupper(entry.second[0])) != string::npos)
        {
            substitution_score -= 10;
            break;
        }
    }

    unordered_set<string> personal_info = {
        "password", "123456", "qwerty", "abc123", "letmein",
        "monkey", "football", "iloveyou", "admin"};
    double personal_score = 0;
    string lowercase_password = password;
    transform(lowercase_password.begin(), lowercase_password.end(), lowercase_password.begin(), ::tolower);
    if (personal_info.count(lowercase_password) > 0)
    {
        personal_score -= 20;
    }

    unordered_set<string> reused_passwords = {
        "password1", "password2", "password3", "password4", "password5"};
    double reused_score = 0;
    if (reused_passwords.count(lowercase_password) > 0)
    {
        reused_score -= 10;
    }

    double pattern_score = 0;
    if (password.find_first_of("0123456789") == string::npos ||
        password.find_first_of("abcdefghijklmnopqrstuvwxyz") == string::npos ||
        password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos ||
        password.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == string::npos)
    {
        pattern_score -= 10;
    }

    double score = length_score + complexity_score + randomness_score +
                   dictionary_score + substitution_score + personal_score +
                   reused_score + pattern_score;

    return round(max(score, 0.0));
}

int main()
{
    string password;
    cout << "Enter the password (or press '1' to generate a random password):\n ";
    cin >> password;

    if (password == "1")
    {
        int password_length;
        cout << "Enter the length of the generated password: \n";
        cin >> password_length;

        if (password_length <= 0)
        {
            cout << "Invalid password length. Please enter a positive integer.\n";
            return 1;
        }

        password = generate_password(password_length);
        cout << "Generated Password: " << password << endl;
    }
    else
    {
        double strength = password_strength(password);
        cout << "Password strength: " << strength << endl;
    }

    return 0;
}
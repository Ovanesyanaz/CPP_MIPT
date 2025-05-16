#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

// Предварительное объявление класса Group
class Group;

class User {
private:
    std::string userId;
    std::string username;
    std::string email;
    int age;
    Group* group;

public:
    User(const std::string& id, const std::string& name, const std::string& email, int age);

    // Геттеры
    const std::string& getId() const { return userId; }
    const std::string& getUsername() const { return username; }
    const std::string& getEmail() const { return email; }
    int getAge() const { return age; }
    Group* getGroup() const { return group; }

    // Сеттеры
    void setGroup(Group* newGroup);
    void removeGroup() { group = nullptr; }

    void printInfo() const;
};

class Group {
private:
    std::string groupId;
    std::vector<User*> users;

public:
    Group(const std::string& id) : groupId(id) {}

    // Геттеры
    const std::string& getId() const { return groupId; }
    const std::vector<User*>& getUsers() const { return users; }

    // Добавление пользователя в группу
    void addUser(User* user) {
        if (user && std::find(users.begin(), users.end(), user) == users.end()) {
            users.push_back(user);
            user->setGroup(this);
        }
    }

    // Удаление пользователя из группы
    void removeUser(User* user) {
        auto it = std::find(users.begin(), users.end(), user);
        if (it != users.end()) {
            users.erase(it);
            user->removeGroup();
        }
    }

    void printInfo() const {
        std::cout << "Group ID: " << groupId << "\n";
        std::cout << "Members (" << users.size() << "):\n";
        for (const auto& user : users) {
            std::cout << "  - " << user->getUsername() << " (ID: " << user->getId() << ")\n";
        }
        std::cout << "------------------\n";
    }
};

// Реализация методов User после определения Group
User::User(const std::string& id, const std::string& name, const std::string& email, int age)
    : userId(id), username(name), email(email), age(age), group(nullptr) {}

void User::setGroup(Group* newGroup) {
    group = newGroup;
}

void User::printInfo() const {
    std::cout << "User ID: " << userId << "\n";
    std::cout << "Username: " << username << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Age: " << age << "\n";
    if (group) {
        std::cout << "Group ID: " << group->getId() << "\n";
    } else {
        std::cout << "Not in any group\n";
    }
    std::cout << "------------------\n";
}

// Класс для управления пользователями и группами
class UserGroupManager {
private:
    std::unordered_map<std::string, std::unique_ptr<User>> users;
    std::unordered_map<std::string, std::unique_ptr<Group>> groups;

public:
    // Методы для работы с пользователями
    bool createUser(const std::string& userId, const std::string& username, 
                   const std::string& email, int age) {
        if (users.find(userId) != users.end()) return false;
        users[userId] = std::make_unique<User>(userId, username, email, age);
        return true;
    }

    bool deleteUser(const std::string& userId) {
        auto it = users.find(userId);
        if (it == users.end()) return false;

        // Удаляем пользователя из группы, если он в ней состоит
        if (auto group = it->second->getGroup()) {
            group->removeUser(it->second.get());
        }

        users.erase(it);
        return true;
    }

    void printAllUsers() const {
        std::cout << "All Users (" << users.size() << "):\n";
        for (const auto& pair : users) {
            pair.second->printInfo();
        }
    }

    void printUser(const std::string& userId) const {
        auto it = users.find(userId);
        if (it != users.end()) {
            it->second->printInfo();
        } else {
            std::cout << "User not found.\n";
        }
    }

    // Методы для работы с группами
    bool createGroup(const std::string& groupId) {
        if (groups.find(groupId) != groups.end()) return false;
        groups[groupId] = std::make_unique<Group>(groupId);
        return true;
    }

    bool deleteGroup(const std::string& groupId) {
        auto it = groups.find(groupId);
        if (it == groups.end()) return false;

        // Удаляем всех пользователей из группы перед удалением самой группы
        for (auto user : it->second->getUsers()) {
            user->removeGroup();
        }

        groups.erase(it);
        return true;
    }

    void printAllGroups() const {
        std::cout << "All Groups (" << groups.size() << "):\n";
        for (const auto& pair : groups) {
            pair.second->printInfo();
        }
    }

    void printGroup(const std::string& groupId) const {
        auto it = groups.find(groupId);
        if (it != groups.end()) {
            it->second->printInfo();
        } else {
            std::cout << "Group not found.\n";
        }
    }

    // Метод для добавления пользователя в группу
    bool addUserToGroup(const std::string& userId, const std::string& groupId) {
        auto userIt = users.find(userId);
        auto groupIt = groups.find(groupId);
        
        if (userIt == users.end() || groupIt == groups.end()) return false;
        
        groupIt->second->addUser(userIt->second.get());
        return true;
    }

    // Метод для удаления пользователя из группы
    bool removeUserFromGroup(const std::string& userId, const std::string& groupId) {
        auto userIt = users.find(userId);
        auto groupIt = groups.find(groupId);
        
        if (userIt == users.end() || groupIt == groups.end()) return false;
        
        groupIt->second->removeUser(userIt->second.get());
        return true;
    }
};

// Функция для обработки команд
void processCommand(const std::string& command, UserGroupManager& manager) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = command.find(' ');
    
    while (end != std::string::npos) {
        tokens.push_back(command.substr(start, end - start));
        start = end + 1;
        end = command.find(' ', start);
    }
    tokens.push_back(command.substr(start));
    
    if (tokens.empty()) return;
    
    if (tokens[0] == "createUser" && tokens.size() >= 4) {
        int age = tokens.size() > 4 ? std::stoi(tokens[4]) : 0;
        if (manager.createUser(tokens[1], tokens[2], tokens[3], age)) {
            std::cout << "User created successfully.\n";
        } else {
            std::cout << "Failed to create user (user ID already exists).\n";
        }
    }
    else if (tokens[0] == "deleteUser" && tokens.size() >= 2) {
        if (manager.deleteUser(tokens[1])) {
            std::cout << "User deleted successfully.\n";
        } else {
            std::cout << "Failed to delete user (user not found).\n";
        }
    }
    else if (tokens[0] == "allUsers") {
        manager.printAllUsers();
    }
    else if (tokens[0] == "getUser" && tokens.size() >= 2) {
        manager.printUser(tokens[1]);
    }
    else if (tokens[0] == "createGroup" && tokens.size() >= 2) {
        if (manager.createGroup(tokens[1])) {
            std::cout << "Group created successfully.\n";
        } else {
            std::cout << "Failed to create group (group ID already exists).\n";
        }
    }
    else if (tokens[0] == "deleteGroup" && tokens.size() >= 2) {
        if (manager.deleteGroup(tokens[1])) {
            std::cout << "Group deleted successfully.\n";
        } else {
            std::cout << "Failed to delete group (group not found).\n";
        }
    }
    else if (tokens[0] == "allGroups") {
        manager.printAllGroups();
    }
    else if (tokens[0] == "getGroup" && tokens.size() >= 2) {
        manager.printGroup(tokens[1]);
    }
    else if (tokens[0] == "addUserToGroup" && tokens.size() >= 3) {
        if (manager.addUserToGroup(tokens[1], tokens[2])) {
            std::cout << "User added to group successfully.\n";
        } else {
            std::cout << "Failed to add user to group (user or group not found).\n";
        }
    }
    else if (tokens[0] == "removeUserFromGroup" && tokens.size() >= 3) {
        if (manager.removeUserFromGroup(tokens[1], tokens[2])) {
            std::cout << "User removed from group successfully.\n";
        } else {
            std::cout << "Failed to remove user from group (user or group not found).\n";
        }
    }
    else {
        std::cout << "Unknown command or invalid arguments.\n";
    }
}

int main() {
    UserGroupManager manager;
    std::string command;
    
    std::cout << "User and Group Management System\n";
    std::cout << "Available commands:\n";
    std::cout << "  createUser {userId} {username} {email} [age]\n";
    std::cout << "  deleteUser {userId}\n";
    std::cout << "  allUsers\n";
    std::cout << "  getUser {userId}\n";
    std::cout << "  createGroup {groupId}\n";
    std::cout << "  deleteGroup {groupId}\n";
    std::cout << "  allGroups\n";
    std::cout << "  getGroup {groupId}\n";
    std::cout << "  addUserToGroup {userId} {groupId}\n";
    std::cout << "  removeUserFromGroup {userId} {groupId}\n";
    std::cout << "  exit\n\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command == "exit") break;
        
        processCommand(command, manager);
    }
    
    return 0;
}
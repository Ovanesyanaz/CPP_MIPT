#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>

class Log {
public:
    // Уровни важности событий
    enum Level { NORMAL, WARNING, ERROR };
    
    // Получение экземпляра синглтона
    static Log& getInstance() {
        static Log instance;
        return instance;
    }
    
    // Добавление сообщения в лог
    void message(Level level, const std::string& msg) {
        entries.emplace_back(std::time(nullptr), level, msg);
        if (entries.size() > MAX_ENTRIES) {
            entries.erase(entries.begin());
        }
    }
    
    // Вывод последних сообщений
    void print() {
        std::cout << "=== Last " << MAX_ENTRIES << " log entries ===" << std::endl;
        for (const auto& [time, level, message] : entries) {
            char timeStr[20];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
            
            std::cout << "[" << timeStr << "] "
                      << (level == NORMAL ? "NORMAL" : level == WARNING ? "WARNING" : "ERROR")
                      << ": " << message << std::endl;
        }
        std::cout << "=============================" << std::endl;
    }
    
    // Удаляем конструкторы копирования и присваивания
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

private:
    // Приватный конструктор
    Log() = default;
    
    // Структура для хранения записи лога
    struct Entry {
        std::time_t time;
        Level level;
        std::string message;
        Entry(std::time_t t, Level l, const std::string& m) : time(t), level(l), message(m) {}
    };
    
    std::vector<Entry> entries;
    static const size_t MAX_ENTRIES = 10;
};

// Пример использования
int main() {
    Log &log1 = Log::getInstance();
    log1.message(Log::NORMAL, "Program started");
    log1.message(Log::WARNING, "Low memory detected");
    log1.message(Log::ERROR, "Critical failure in module X");
    
    log1.print();
    //можно ли создать 2 экземпляр ?
    Log &log2 = Log::getInstance();
    log2.print(); // тот же объект

    //ограниченность хранящихся записей
    for (int i = 1; i <= 12; ++i) {
        Log::getInstance().message(Log::NORMAL, "Processing item " + std::to_string(i));
    }
    
    Log::getInstance().print();
    
    return 0;
}
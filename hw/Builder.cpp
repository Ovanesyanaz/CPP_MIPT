#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <utility>

// Базовый класс контрольного пункта
class ControlPoint {
protected:
    std::string name;
    double latitude;
    double longitude;
    
public:
    ControlPoint(const std::string& name, double lat, double lon)
        : name(name), latitude(lat), longitude(lon) {
        if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
            throw std::invalid_argument("Invalid coordinates");
        }
    }
    
    virtual ~ControlPoint() = default;
    
    virtual double getPenalty() const = 0;
    virtual bool isMandatory() const = 0;
    
    const std::string& getName() const { return name; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
};

// Обязательный контрольный пункт
class MandatoryControlPoint : public ControlPoint {
public:
    MandatoryControlPoint(const std::string& name, double lat, double lon)
        : ControlPoint(name, lat, lon) {}
    
    double getPenalty() const override { return 0.0; }
    bool isMandatory() const override { return true; }
};

// Необязательный контрольный пункт
class OptionalControlPoint : public ControlPoint {
    double penalty;
    
public:
    OptionalControlPoint(const std::string& name, double lat, double lon, double penalty)
        : ControlPoint(name, lat, lon), penalty(penalty) {
        if (penalty < 0) {
            throw std::invalid_argument("Penalty cannot be negative");
        }
    }
    
    double getPenalty() const override { return penalty; }
    bool isMandatory() const override { return false; }
};

// Абстрактный строитель
class ControlPointBuilder {
public:
    virtual ~ControlPointBuilder() = default;
    virtual void addControlPoint(const ControlPoint& cp) = 0;
    virtual void buildResult() = 0;
};

// Строитель для текстового вывода
class TextListBuilder : public ControlPointBuilder {
    std::vector<std::reference_wrapper<const ControlPoint>> points;
    
public:
    void addControlPoint(const ControlPoint& cp) override {
        points.push_back(cp);
    }
    
    void buildResult() override {
        std::cout << "\nСписок контрольных пунктов:\n";
        std::cout << "==================================================\n";
        std::cout << " № | Название          | Координаты           | Штраф\n";
        std::cout << "--------------------------------------------------\n";
        
        int index = 1;
        for (const auto& cp_ref : points) {
            const ControlPoint& cp = cp_ref.get();
            
            // Форматируем вывод
            std::cout << std::setw(2) << std::right << index++ << " | ";
            std::cout << std::setw(17) << std::left << cp.getName() << " | ";
            
            // Координаты с фиксированной точностью
            std::cout << "(" << std::fixed << std::setprecision(6) 
                      << cp.getLatitude() << ", " 
                      << cp.getLongitude() << ") | ";
            
            // Вывод штрафа или отметки
            if (cp.isMandatory()) {
                std::cout << "незачёт СУ";
            } else {
                std::cout << std::setprecision(1) << cp.getPenalty() << " ч";
            }
            
            std::cout << "\n";
        }
        std::cout << "==================================================\n";
    }
};

// Строитель для подсчёта штрафов
class PenaltyCalculatorBuilder : public ControlPointBuilder {
    double totalPenalty = 0.0;
    int skippedPoints = 0;
    
public:
    void addControlPoint(const ControlPoint& cp) override {
        if (!cp.isMandatory()) {
            totalPenalty += cp.getPenalty();
            skippedPoints++;
        }
    }
    
    void buildResult() override {
        std::cout << "\nСтатистика по необязательным КП:\n";
        std::cout << "----------------------------------\n";
        std::cout << "Количество пропущенных КП: " << skippedPoints << "\n";
        std::cout << "Суммарный штраф: " << std::fixed << std::setprecision(1) 
                  << totalPenalty << " часов\n";
        std::cout << "----------------------------------\n";
    }
};

// Директор - управляет процессом построения
class ControlPointProcessor {
    std::vector<std::unique_ptr<ControlPoint>> controlPoints;
    
public:
    void addControlPoint(std::unique_ptr<ControlPoint> cp) {
        controlPoints.push_back(std::move(cp));
    }
    
    void process(ControlPointBuilder& builder) const {
        for (const auto& cp : controlPoints) {
            builder.addControlPoint(*cp);
        }
        builder.buildResult();
    }
};

int main() {
    try {
        ControlPointProcessor processor;
        
        // Добавляем контрольные пункты
        processor.addControlPoint(
            std::make_unique<MandatoryControlPoint>("Старт", 55.752222, 37.615555));
        
        processor.addControlPoint(
            std::make_unique<OptionalControlPoint>("Горный перевал", 55.755800, 37.617600, 2.5));
        
        processor.addControlPoint(
            std::make_unique<MandatoryControlPoint>("Речная переправа", 55.758600, 37.622100));
        
        processor.addControlPoint(
            std::make_unique<OptionalControlPoint>("Лесной участок", 55.761200, 37.626500, 1.5));
        
        processor.addControlPoint(
            std::make_unique<MandatoryControlPoint>("Финиш", 55.763900, 37.630500));
        
        // Обработка разными строителями
        TextListBuilder textBuilder;
        processor.process(textBuilder);
        
        PenaltyCalculatorBuilder penaltyBuilder;
        processor.process(penaltyBuilder);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
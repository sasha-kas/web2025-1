#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Operator {
public:
    int ID;
    double talkingCharge, messageCost, networkCharge;
    int discountRate;

    Operator(int id, double talk, double msg, double net, int discount)
        : ID(id), talkingCharge(talk), messageCost(msg), networkCharge(net), discountRate(discount) {}

    double calcTalkCost(int minutes, int age) const {
        double cost = minutes * talkingCharge;
        return (age < 18 || age > 65) ? cost * (1 - discountRate / 100.0) : cost;
    }

    double calcMessageCost(int quantity, bool sameOperator) const {
        double cost = quantity * messageCost;
        return sameOperator ? cost * (1 - discountRate / 100.0) : cost;
    }

    double calcNetworkCost(double amount) const {
        return amount * networkCharge;
    }
};

class Bill {
public:
    double limit, debt = 0;

    Bill(double limit) : limit(limit) {}

    bool canAdd(double amount) const { return debt + amount <= limit; }
    void add(double amount) { if (canAdd(amount)) debt += amount; }
    void pay(double amount) { debt = max(0.0, debt - amount); }
};

class Customer {
public:
    int ID;
    int age;
    string name;
    Operator* op;
    Bill* bill;

    Customer(int id, string name, int age, Operator* op, Bill* bill)
        : ID(id), age(age), name(name), op(op), bill(bill) {}

    void talk(int minutes, Customer& other) {
        double cost = op->calcTalkCost(minutes, age);
        if (bill->canAdd(cost)) {
            bill->add(cost);
            cout << name << " говорив з " << other.name << " протягом " << minutes << " хвилин. Вартість: " << cost << endl;
        } else {
            cout << name << " не може здійснити дзвінок. Ліміт перевищено." << endl;
        }
    }

    void message(int quantity, Customer& other) {
        bool sameOp = op->ID == other.op->ID;
        double cost = op->calcMessageCost(quantity, sameOp);
        if (bill->canAdd(cost)) {
            bill->add(cost);
            cout << name << " надіслав " << quantity << " повідомлень до " << other.name << ". Вартість: " << cost << endl;
        } else {
            cout << name << " не може надіслати повідомлення. Ліміт перевищено." << endl;
        }
    }

    void connect(double amount) {
        double cost = op->calcNetworkCost(amount);
        if (bill->canAdd(cost)) {
            bill->add(cost);
            cout << name << " підключився до інтернету. Використано даних: " << amount << " МБ. Вартість: " << cost << endl;
        } else {
            cout << name << " не може підключитися до інтернету. Ліміт перевищено." << endl;
        }
    }
};

// Main program
int main() {
    int numCustomers, numOperators;

    cout << "Введіть кількість операторів: ";
    cin >> numOperators;
    vector<Operator*> operators;

    for (int i = 0; i < numOperators; ++i) {
        int discount;
        double talkCharge, messageCost, networkCharge;
        cout << "Введіть дані для оператора " << i << " (вартість дзвінка, вартість повідомлення, вартість інтернету, знижка): ";
        cin >> talkCharge >> messageCost >> networkCharge >> discount;
        operators.push_back(new Operator(i, talkCharge, messageCost, networkCharge, discount));
    }

    cout << "Введіть кількість клієнтів: ";
    cin >> numCustomers;
    vector<Customer*> customers;

    for (int i = 0; i < numCustomers; ++i) {
        int age, operatorID;
        string name;
        double billLimit;
        cout << "Введіть дані для клієнта " << i << " (ім'я, вік, ID оператора, ліміт рахунку): ";
        cin >> name >> age >> operatorID >> billLimit;

        Bill* bill = new Bill(billLimit);
        customers.push_back(new Customer(i, name, age, operators[operatorID], bill));
    }

    // Simulating interactions
    int action;
    do {
        cout << "\nОберіть дію (1: Дзвінок, 2: Повідомлення, 3: Інтернет-з'єднання, 4: Оплата рахунку, 5: Вихід): ";
        cin >> action;

        if (action == 1) {
            int fromID, toID, minutes;
            cout << "Введіть ID клієнтів (від кого, до кого) і тривалість дзвінка (хвилини): ";
            cin >> fromID >> toID >> minutes;
            customers[fromID]->talk(minutes, *customers[toID]);
        } else if (action == 2) {
            int fromID, toID, quantity;
            cout << "Введіть ID клієнтів (від кого, до кого) і кількість повідомлень: ";
            cin >> fromID >> toID >> quantity;
            customers[fromID]->message(quantity, *customers[toID]);
        } else if (action == 3) {
            int fromID;
            double data;
            cout << "Введіть ID клієнта та обсяг даних (МБ): ";
            cin >> fromID >> data;
            customers[fromID]->connect(data);
        } else if (action == 4) {
            int fromID;
            double amount;
            cout << "Введіть ID клієнта і суму оплати: ";
            cin >> fromID >> amount;
            customers[fromID]->bill->pay(amount);
            cout << "Поточний борг клієнта " << customers[fromID]->name << ": " << customers[fromID]->bill->debt << endl;
        }
    } while (action != 5);

    for (auto customer : customers) delete customer;
    for (auto op : operators) delete op;

    return 0;
}

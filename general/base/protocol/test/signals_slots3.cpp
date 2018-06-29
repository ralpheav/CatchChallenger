#include "Signal.hpp"
#include <iostream>

class Person {
  public:
    Person(std::string const &name) : name_(name) {}

    Signal<std::string const &> say;

    void listen(std::string const &message) {
        std::cout << name_ << " received: " << message << std::endl;
    }

  private:
    std::string name_;
};

int main() {
    Person alice("Alice"), bob("Bob");

    alice.say.connect_member(&bob, &Person::listen);
    bob.say.connect_member(&alice, &Person::listen);

    alice.say.emit("Have a nice day!");
    bob.say.emit("Thank you!");

    return 0;
}

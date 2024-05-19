#include "Arduino.h"
#pragma once

class Unit {
private:
    int id;
    String name;
    String abbreviation;

public:
    Unit(int id, const String& name, const String& abbreviation)
        : id(id), name(name), abbreviation(abbreviation) {}

    int getId() const {
        return id;
    }

    void setId(int newId) {
        id = newId;
    }

    const String& getName() const {
        return name;
    }

    void setName(const String& newName) {
        name = newName;
    }

    const String& getAbbreviation() const {
        return abbreviation;
    }

    void setAbbreviation(const String& newAbbreviation) {
        abbreviation = newAbbreviation;
    }
};
#pragma once

#include <Arduino.h>

class Room {
private:
    int id;
    String name;
    String description;

public:
    Room(int id, const String& name, const String& description)
        : id(id), name(name), description(description) {}

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

    const String& getDescription() const {
        return description;
    }

    void setDescription(const String& newDescription) {
        description = newDescription;
    }


};
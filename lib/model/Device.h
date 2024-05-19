
#pragma once
#include <Arduino.h>
#include "Unit.h"
#include "Room.h"

class Device
{
private:
    int id;
    String name;
    String description;
    bool status;
    String pinMode;
    int value;
    String image;
    Room *room;
    Unit *unit;
    bool isSensor;
    int unitId;
    int roomId;

public:
    // Constructor
    Device(int id, String name, String description, bool status, String pinMode, int value, String image, Room *room, Unit *unit, bool isSensor, int unitId, int roomId)
    {
        this->id = id;
        this->name = name;
        this->description = description;
        this->status = status;
        this->pinMode = pinMode;
        this->value = value;
        this->image = image;
        this->room = room;
        this->unit = unit;
        this->isSensor = isSensor;
        this->unitId = unitId;
        this->roomId = roomId;
    }

    // Getters
    int getId() const
    {
        return id;
    }

    String getName() const
    {
        return name;
    }

    String getDescription() const
    {
        return description;
    }

    bool getStatus() const
    {
        return status;
    }

    String getPinMode() const
    {
        return pinMode;
    }

    int getValue() const
    {
        return value;
    }

    String getImage() const
    {
        return image;
    }

    Room *getRoom() const
    {
        return room;
    }

    Unit *getUnit() const
    {
        return unit;
    }

    bool getIsSensor() const
    {
        return isSensor;
    }

    int getUnitId() const
    {
        return unitId;
    }

    int getRoomId() const
    {
        return roomId;
    }
};
#pragma once

#include "../tool.h"


class InvertEffect : public Effect {
public:
    inline InvertEffect(ToolManager *manager_) :
        Effect(manager_) {}

    virtual void apply(Layer &layer) override;

protected:
    //

};
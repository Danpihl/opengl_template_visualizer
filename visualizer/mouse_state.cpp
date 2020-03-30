#include "visualizer/mouse_state.h"

#include <iostream>
#include <string>
#include <vector>

const std::vector<std::string> mouse_strings = {"Left", "Middle", "Right"};

MouseButton::MouseButton(const int button_enum) : button_enum_(button_enum)
{
    is_pressed_ = 0;
    x_ = -1;
    y_ = -1;
    px_ = -1;
    py_ = -1;
    dx_ = 0;
    dy_ = 0;
}

int MouseButton::getDx() const
{
    return dx_;
}

int MouseButton::getDy() const
{
    return dy_;
}

void MouseButton::updateOnPress(const int button, const int state, const int x, const int y)
{
    if (button == button_enum_)
    {
        is_pressed_ = !state;
        if (is_pressed_)
        {
            x_ = x;
            y_ = y;
            px_ = x;
            py_ = y;
            dx_ = 0;
            dy_ = 0;
        }
        else
        {
            x_ = -1;
            y_ = -1;
            px_ = -1;
            py_ = -1;
            dx_ = 0;
            dy_ = 0;
        }
    }
}

void MouseButton::updateOnMotion(const int x, const int y)
{
    if (is_pressed_)
    {
        px_ = x_;
        py_ = y_;
        x_ = x;
        y_ = y;
        dx_ = x_ - px_;
        dy_ = y_ - py_;
    }
}

bool MouseButton::isPressed() const
{
    return is_pressed_;
}

void MouseButton::print() const
{
    const std::string p = is_pressed_ ? "pressed" : "not pressed";
    std::cout << "Button " << mouse_strings[button_enum_] << " is " << p << " with xy coords [ "
              << x_ << ", " << y_ << " ]" << std::endl;
}

// ****************************************
// ************** MouseState **************
// ****************************************

MouseState::MouseState(const int enum_left, const int enum_middle, const int enum_right)
    : left_button_(enum_left), middle_button_(enum_middle), right_button_(enum_right)
{
}

MouseButton MouseState::getLeftButton() const
{
    return left_button_;
}

MouseButton MouseState::getRightButton() const
{
    return right_button_;
}

MouseButton MouseState::getMiddleButton() const
{
    return middle_button_;
}

void MouseState::updateOnPress(const int button, const int state, const int x, const int y)
{
    left_button_.updateOnPress(button, state, x, y);
    middle_button_.updateOnPress(button, state, x, y);
    right_button_.updateOnPress(button, state, x, y);
}

void MouseState::updateOnMotion(const int x, const int y)
{
    left_button_.updateOnMotion(x, y);
    middle_button_.updateOnMotion(x, y);
    right_button_.updateOnMotion(x, y);
}

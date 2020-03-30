#ifndef MOUSE_STATE_H_
#define MOUSE_STATE_H_

class MouseButton
{
private:
    int x_;
    int y_;
    int px_;
    int py_;
    int dx_;
    int dy_;

    const int button_enum_;
    bool is_pressed_;

public:
    MouseButton(const int button_enum);
    void updateOnPress(const int button, const int state, const int x, const int y);
    void updateOnMotion(const int x, const int y);

    bool isPressed() const;

    void print() const;

    int getDx() const;
    int getDy() const;
};

class MouseState
{
private:
    MouseButton left_button_;
    MouseButton middle_button_;
    MouseButton right_button_;

public:
    MouseState() = default;
    MouseState(const int enum_left, const int enum_middle, const int enum_right);

    MouseButton getLeftButton() const;
    MouseButton getRightButton() const;
    MouseButton getMiddleButton() const;

    void updateOnPress(const int button, const int state, const int x, const int y);
    void updateOnMotion(const int x, const int y);
};

#endif

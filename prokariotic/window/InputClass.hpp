#ifndef __INPUTCLASS_H_
#define __INPUTCLASS_H_

class InputClass {
public:
    InputClass();

    InputClass(const InputClass &);

    ~InputClass();

    void Initialize();

    void KeyDown(unsigned int);

    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int);

private:
    bool m_keys[256];
};

#endif	//__INPUTCLASS_H_

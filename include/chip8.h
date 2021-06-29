#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>

#include "graphics.h"

class Chip8 final
{
public:
    explicit Chip8(const std::string& path); 
    Chip8() = delete;
    ~Chip8() = default;
    Chip8(const Chip8& other) = delete;
    constexpr Chip8(Chip8&& other) = delete;
    Chip8& operator=(const Chip8& other) = delete;
    constexpr Chip8& operator=(Chip8&& other) = delete; 
    void run();


  
private:
    void load_game(const std::string& path);
    void handle_opcode();
    void update_timers();
    void update_key(const sf::Event& event, const uint8_t state);
    void init_font();
    void init_opcode_table();
    void init_opcode_args();

    void inst_00E0();
    void inst_00EE();
    void inst_1NNN();
    void inst_2NNN();
    void inst_3XNN();
    void inst_4XNN();
    void inst_5XY0();
    void inst_6XNN();
    void inst_7XNN();
    void inst_8XY0();
    void inst_8XY1();
    void inst_8XY2();
    void inst_8XY3();
    void inst_8XY4();
    void inst_8XY5();
    void inst_8XY6();
    void inst_8XY7();
    void inst_8XYE();
    void inst_9XY0();
    void inst_ANNN();
    void inst_BNNN();
    void inst_CXNN();
    void inst_DXYN();
    void inst_EX9E();
    void inst_EXA1();
    void inst_FX07();
    void inst_FX0A();
    void inst_FX15();
    void inst_FX18();
    void inst_FX1E();
    void inst_FX29();
    void inst_FX33();
    void inst_FX55();
    void inst_FX65();

    enum class Key_state : uint8_t 
    {
        released = 0,
        pressed = 1
    };
    static constexpr uint16_t MEMORY_SIZE = 4096;
    static constexpr uint8_t RES_WIDTH = 64;
    static constexpr uint8_t RES_HEIGHT = 32;
    static constexpr uint8_t STACK_DEPTH = 16;
    static constexpr uint8_t REG_AMOUNT= 16;
    static constexpr uint8_t KEY_AMOUNT = 16;  
    static constexpr uint8_t SCALE_FACTOR = 10;
    static constexpr uint16_t PROGRAM_START_ADDRESS = 0x200;
    static constexpr uint8_t FONT_SET_SIZE = 80;
    static constexpr uint8_t MAX_VAL= 0xFF; 

    std::array<uint8_t, MEMORY_SIZE> _memory; 
    struct
    {
        uint16_t pc;
        uint16_t i;
        uint16_t sp;
        std::array<unsigned char, REG_AMOUNT> v;
    } _cpu;
    std::array<uint16_t, STACK_DEPTH> _stack;
    
    uint16_t _opcode;
    struct 
    {
        uint16_t nnn;
        uint8_t nn;
        uint8_t n;
        uint8_t x;
        uint8_t y;
    } _opcode_args;

    struct 
    {
        uint8_t _delay;
        uint8_t _sound;
    } _timer;   
    std::array<std::array<uint8_t, RES_WIDTH>, RES_HEIGHT> _display;
    std::array<uint16_t, KEY_AMOUNT> _keypad;
    template<typename T>
    using Inst_fun = void (T::*)();
    std::map<uint16_t, Inst_fun<Chip8>> _opcode_table;
    Graphics _graphics;

};

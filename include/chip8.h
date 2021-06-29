#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>

#include "graphics.h"

enum class Key_state : uint8_t {released = 0, pressed = 1};

template<typename T>
using Inst_fun = void (T::*)();

class Chip8 final
{
public:
    Chip8(const std::string& path); 
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
    void update_key(sf::Event& event, const uint8_t state);
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

    static constexpr bool debug = true;
    static constexpr uint16_t memory_size_ = 4096;
    static constexpr uint8_t res_width_ = 64;
    static constexpr uint8_t res_height_ = 32;
    static constexpr uint8_t stack_depth_ = 16;
    static constexpr uint8_t reg_amount_= 16;
    static constexpr uint8_t key_amount_ = 16;  
    static constexpr uint16_t program_start_address_ = 0x200;
    static constexpr uint8_t font_set_size_ = 80;
    static constexpr uint8_t max_val_= 0xFF;
    static constexpr uint8_t vf_ = 0xF;
    std::array<uint8_t, memory_size_> memory_;
    uint16_t pc_;
    uint16_t i_;
    std::array<uint16_t, stack_depth_> stack_;
    uint16_t sp_;
    std::array<unsigned char, reg_amount_> v_;
    uint16_t opcode_;
    struct 
    {
        uint16_t nnn_;
        uint8_t nn_;
        uint8_t n_;
        uint8_t x_;
        uint8_t y_;
    }opcode_args_;
    struct 
    {
        uint8_t delay_;
        uint8_t sound_;
    }timer_;   
    std::array<std::array<uint8_t, res_width_>, res_height_> display_;
    std::array<uint16_t, key_amount_> keypad_;
    std::map<uint16_t, Inst_fun<Chip8>> opcode_table_;
    Graphics graphics_;

};

#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <thread>

#include "chip8.h"


Chip8::Chip8(const std::string& path) : pc_(Chip8::program_start_address_), i_(0), sp_(0), opcode_(0), graphics_(res_width_, res_height_, 10, path)
{
    memory_.fill(0);
    keypad_.fill(0);
    stack_.fill(0);
    v_.fill(0);
    std::memset(&display_, 0, sizeof(display_));
    //std::memset(&opcode_args_, 0, sizeof(opcode_args_));
    std::memset(&timer_, 0, sizeof(timer_));    
    init_font();
    load_game(path);
    init_opcode_table();
    std::srand(std::time(nullptr));
}

void Chip8::run()
{
    sf::Event event;
	while (graphics_.window_.isOpen())
	{
		while (graphics_.window_.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
                    graphics_.window_.close();
				    break;
                case sf::Event::KeyPressed:
                    update_key(event, static_cast<uint8_t>(Key_state::pressed));
                    break;
                case sf::Event::KeyReleased:
                    update_key(event, static_cast<uint8_t>(Key_state::released));
                    break;
                default:
                    break;
			}
		}
        handle_opcode();
        update_timers();
        std::this_thread::sleep_for(std::chrono::microseconds(2000)); // delay
	}
}

void Chip8::update_key(sf::Event& event, uint8_t state)
{
	switch(event.key.code)
	{
        case sf::Keyboard::Num1:
            keypad_[1] = state;
        break;
        case sf::Keyboard::Num2:
            keypad_[2] = state;
            break;
        case sf::Keyboard::Num3:
            keypad_[3] = state;
            break;
        case sf::Keyboard::Num4:
            keypad_[12] = state;
            break;
        case sf::Keyboard::Q:
            keypad_[4] = state;
            break;
        case sf::Keyboard::W:
            keypad_[5] = state;
            break;
        case sf::Keyboard::E:
            keypad_[6] = state;
            break;
        case sf::Keyboard::R:
            keypad_[13] = state;
            break;
        case sf::Keyboard::A:
            keypad_[7] = state;
            break;
        case sf::Keyboard::S:
            keypad_[8] = state;
            break;
        case sf::Keyboard::D:
            keypad_[9] = state;
            break;
        case sf::Keyboard::F:
            keypad_[14] = state;
            break;
        case sf::Keyboard::Z:
            keypad_[10] = state;
            break;
        case sf::Keyboard::X:
            keypad_[0] = state;
            break;
        case sf::Keyboard::C:
            keypad_[11] = state;
            break;
        case sf::Keyboard::V:
            keypad_[15] = state;
            break;
        case sf::Keyboard::Escape:
            graphics_.window_.close();
            break;
        default:
            break;
	}
}


void Chip8::handle_opcode()
{
    opcode_ = memory_[pc_] << 8 | memory_[pc_ + 1];
    bool exec_inst = false;
    for (auto const& entry : opcode_table_)
    {
        if (((entry.first & opcode_) == opcode_) && ((entry.first | opcode_) == entry.first))
        {
            init_opcode_args();
            (*this.*entry.second)();
            exec_inst = true;
            break;
        }
    } 
    if (!exec_inst)
    {
        std::stringstream hex;
        hex << "0x" << std::hex << opcode_ << std::endl;
        throw std::runtime_error("Tried to execute illegal instruction: " + hex.str());
    }
    else
    {
        std::cout << "Executed Opcode " << "0x" << std::hex << opcode_ << std::endl;
    }
}

void Chip8::update_timers()
{
    if (timer_.delay_ > 0)
    {
        timer_.delay_--;
    }
    if (timer_.sound_ > 0)
    {
        if (timer_.sound_ == 1)
        {
            std::cout << "BEEP" << std::endl;
        }
        timer_.sound_--;  
    }
}


void Chip8::load_game(const std::string& path)
{
    std::ifstream game_file (path, std::ifstream::ate | std::ifstream::binary);
    if (!game_file)
    {
        throw std::invalid_argument("Cannot open ROM.");
    }
    if (game_file.tellg() > (memory_.size() - program_start_address_))
    {
        throw std::overflow_error("Game file is too big.");
    }
    game_file.seekg(std::ifstream::beg);
    game_file.read(reinterpret_cast<char*>(memory_.data() + program_start_address_), memory_size_);
}

void Chip8::init_opcode_args()
{
    opcode_args_.nnn_ = opcode_ & 0x0FFF;
    opcode_args_.nn_ = opcode_ & 0x00FF;
    opcode_args_.n_ = opcode_ & 0x000F;
    opcode_args_.x_ = (opcode_ & 0x0F00) >> 8;
    opcode_args_.y_ = (opcode_ & 0x00F0) >> 4;
}

void Chip8::init_font()
{
    std::array<uint8_t, Chip8::font_set_size_> font_set
    { {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F 
    } };
    std::copy(font_set.begin(), font_set.end(), memory_.begin());
}

void Chip8::init_opcode_table()
{
  opcode_table_[0x00E0] = &Chip8::inst_00E0;
  opcode_table_[0x00EE] = &Chip8::inst_00EE;
  opcode_table_[0x1FFF] = &Chip8::inst_1NNN;
  opcode_table_[0x2FFF] = &Chip8::inst_2NNN;
  opcode_table_[0x3FFF] = &Chip8::inst_3XNN;
  opcode_table_[0x4FFF] = &Chip8::inst_4XNN;
  opcode_table_[0x5FF0] = &Chip8::inst_5XY0;
  opcode_table_[0x6FFF] = &Chip8::inst_6XNN;
  opcode_table_[0x7FFF] = &Chip8::inst_7XNN;
  opcode_table_[0x8FF0] = &Chip8::inst_8XY0;
  opcode_table_[0x8FF1] = &Chip8::inst_8XY1;
  opcode_table_[0x8FF2] = &Chip8::inst_8XY2;
  opcode_table_[0x8FF3] = &Chip8::inst_8XY3;
  opcode_table_[0x8FF4] = &Chip8::inst_8XY4;
  opcode_table_[0x8FF5] = &Chip8::inst_8XY5;
  opcode_table_[0x8FF6] = &Chip8::inst_8XY6;
  opcode_table_[0x8FF7] = &Chip8::inst_8XY7;
  opcode_table_[0x8FFE] = &Chip8::inst_8XYE;
  opcode_table_[0x9FF0] = &Chip8::inst_9XY0; 
  opcode_table_[0xAFFF] = &Chip8::inst_ANNN;
  opcode_table_[0xBFFF] = &Chip8::inst_BNNN;
  opcode_table_[0xCFFF] = &Chip8::inst_CXNN;
  opcode_table_[0xDFFF] = &Chip8::inst_DXYN;
  opcode_table_[0xEF9E] = &Chip8::inst_EX9E;
  opcode_table_[0xEFA1] = &Chip8::inst_EXA1;
  opcode_table_[0xFF07] = &Chip8::inst_FX07;
  opcode_table_[0xFF0A] = &Chip8::inst_FX0A;
  opcode_table_[0xFF15] = &Chip8::inst_FX15;
  opcode_table_[0xFF18] = &Chip8::inst_FX18;
  opcode_table_[0xFF1E] = &Chip8::inst_FX1E;
  opcode_table_[0xFF29] = &Chip8::inst_FX29;
  opcode_table_[0xFF33] = &Chip8::inst_FX33;
  opcode_table_[0xFF55] = &Chip8::inst_FX55;
  opcode_table_[0xFF65] = &Chip8::inst_FX65;
}

void Chip8::inst_00E0()
{
    std::memset(&display_, 0, sizeof(display_));
    pc_ += 2;
}

void Chip8::inst_00EE()
{
    if (sp_ > 0)
    {
        pc_ = stack_[--sp_];
    }
    else
    {
        throw std::underflow_error("Stack underflow.");
    }
    pc_ += 2;
}

void Chip8::inst_1NNN()
{
    pc_ = opcode_args_.nnn_;
    pc_ += 2;
}

void Chip8::inst_2NNN()
{
    if (sp_ < (stack_.size() - 1))
    {
        stack_[sp_++] = pc_;
        pc_ = opcode_args_.nnn_;
    }
    else
    {
        throw std::overflow_error("Stack overflow.");
    }
    pc_ += 2;
}

void Chip8::inst_3XNN()
{
    if (v_[opcode_args_.x_] == opcode_args_.nn_)
    {
        pc_ += 2;
    }
    pc_ += 2;
}

void Chip8::inst_4XNN()
{
    if(v_[opcode_args_.x_] != opcode_args_.nn_)
    {
        pc_ += 2;
    }
    pc_ += 2;
}

void Chip8::inst_5XY0()
{
    if (v_[opcode_args_.x_] == v_[opcode_args_.y_])
    {
        pc_ += 2;
    }
    pc_ += 2;
}


void Chip8::inst_6XNN()
{
    v_[opcode_args_.x_] = opcode_args_.nn_;
    pc_ += 2;
}

void Chip8::inst_7XNN()
{
    v_[opcode_args_.x_] += opcode_args_.nn_;
    pc_ += 2;
}

void Chip8::inst_8XY0()
{
    v_[opcode_args_.x_] = v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY1()
{
    v_[opcode_args_.x_] |= v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY2()
{
    v_[opcode_args_.x_] &= v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY3()
{
    v_[opcode_args_.x_] ^= v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY4()
{
    v_[vf_] = (v_[opcode_args_.x_] + v_[opcode_args_.y_]) > 0x00FF ? 1 : 0;
    v_[opcode_args_.x_] += v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY5()
{
    v_[vf_] = v_[opcode_args_.x_] < v_[opcode_args_.y_] ? 0 : 1;
    v_[opcode_args_.x_] -= v_[opcode_args_.y_];
    pc_ += 2;
}

void Chip8::inst_8XY6()
{
    v_[vf_] = v_[opcode_args_.x_] & 0x1u;
    v_[opcode_args_.x_] >>= 1;
    pc_ += 2;
}

void Chip8::inst_8XY7()
{
    v_[vf_] = v_[opcode_args_.x_] > v_[opcode_args_.y_] ? 0 : 1;
    v_[opcode_args_.x_] = v_[opcode_args_.y_] - v_[opcode_args_.x_];
    pc_ += 2;
}

void Chip8::inst_8XYE()
{
    v_[vf_] = v_[opcode_args_.x_] >> 7;
    v_[opcode_args_.x_] <<= 1;
    pc_ += 2;
}   

void Chip8::inst_9XY0()
{
    if (v_[opcode_args_.x_] != v_[opcode_args_.y_])
    {
        pc_ += 2;
    }    
    pc_ += 2;
}

void Chip8::inst_ANNN()
{
    i_ = opcode_args_.nnn_;
    pc_ += 2;
}

void Chip8::inst_BNNN()
{
    i_ = v_[0] + opcode_args_.nnn_;
    pc_ += 2;
}

void Chip8::inst_CXNN()
{
    v_[opcode_args_.x_] = (std::rand() % (256)) & opcode_args_.nn_;
    pc_ += 2;
}

void Chip8::inst_DXYN()
{
    uint8_t coord_x = v_[opcode_args_.x_] % res_width_;
    uint8_t coord_y = v_[opcode_args_.y_] % res_height_;
    uint8_t sprite_height = opcode_args_.n_;
    v_[vf_] = 0; // we assume that theres no collision for now.
    for (uint8_t row = 0; row < sprite_height; row++)
    {
        uint8_t px_to_draw = memory_[i_ + row];
        for (uint8_t bit_pos {}; bit_pos < 8; bit_pos++)
        {
            uint8_t& curr_px = display_[coord_y + row][coord_x + bit_pos];
            uint8_t sprite_px = (px_to_draw >> (7 - bit_pos)) & 0x1u;
            if (curr_px && sprite_px)
            {
                v_[vf_] = 1;
            }
            curr_px ^= sprite_px;
        }
    }
    graphics_.draw_window<res_width_, res_height_>(display_);
    pc_ += 2;
}

void Chip8::inst_EX9E()
{
    if (keypad_[v_[opcode_args_.x_]] == static_cast<uint16_t>(Key_state::pressed))
    {
        pc_ += 2;
    }
    pc_ += 2;
}

void Chip8::inst_EXA1()
{
    if (keypad_[v_[opcode_args_.x_]] == static_cast<uint16_t>(Key_state::released))
    {
        pc_ += 2;
    }
    pc_ += 2;
}

void Chip8::inst_FX07()
{
    v_[opcode_args_.x_] = timer_.delay_;
    pc_ += 2;
}

void Chip8::inst_FX0A()
{
    for (int i = 0; i < keypad_.size(); i++)
    {
        if (keypad_[i])
        {
            v_[opcode_args_.x_] = i;
            return;
        }
    }
    pc_ += 2;
}

void Chip8::inst_FX15()
{
    timer_.delay_ = v_[opcode_args_.x_];
    pc_ += 2;
}

void Chip8::inst_FX18()
{
    timer_.sound_ = v_[opcode_args_.x_];
    pc_ += 2;
}

void Chip8::inst_FX1E()
{
    v_[vf_] = (i_ + v_[opcode_args_.x_]) > 0x00FF ? 1 : 0;
    i_ += v_[opcode_args_.x_];
    pc_ += 2;
}

void Chip8::inst_FX29()
{
    i_ = v_[opcode_args_.x_] * 5;
    pc_ += 2;
}

void Chip8::inst_FX33()
{
    memory_[i_] = v_[opcode_args_.x_] / 100;
    memory_[i_ + 1] = (v_[opcode_args_.x_] / 10) % 10;
    memory_[i_ + 2] = v_[opcode_args_.x_] % 10;
    pc_ += 2;
}

void Chip8::inst_FX55()
{
    for (int i = 0; i <= opcode_args_.x_; i++)
    {
        memory_[i_ + i] = v_[i];
    }
    pc_ += 2;
}

void Chip8::inst_FX65()
{
    for (int i = 0; i <= opcode_args_.x_; i++)
    {
        v_[i] = memory_[i_ + i];
    }
    pc_ += 2;
}
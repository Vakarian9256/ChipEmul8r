#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <thread>

#include "chip8.h"


Chip8::Chip8(const std::string& path) :  _opcode(0), _graphics(RES_WIDTH, RES_HEIGHT, SCALE_FACTOR, path)
{
    _memory.fill(0);
    _keypad.fill(0);
    _stack.fill(0);
    _cpu = {PROGRAM_START_ADDRESS, 0, 0};
    _cpu.v.fill(0);
    std::memset(&_display, 0, sizeof(_display));
    std::memset(&_timer, 0, sizeof(_timer));    
    init_font();
    load_game(path);
    init_opcode_table();
    std::srand(std::time(nullptr));
}

void Chip8::run()
{
	while (_graphics.window.isOpen())
	{
        sf::Event event;
		while (_graphics.window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
                    _graphics.window.close();
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

void Chip8::update_key(const sf::Event& event, uint8_t state)
{
	switch(event.key.code)
	{
        case sf::Keyboard::Num1:
            _keypad[1] = state;
        break;
        case sf::Keyboard::Num2:
            _keypad[2] = state;
            break;
        case sf::Keyboard::Num3:
            _keypad[3] = state;
            break;
        case sf::Keyboard::Num4:
            _keypad[12] = state;
            break;
        case sf::Keyboard::Q:
            _keypad[4] = state;
            break;
        case sf::Keyboard::W:
            _keypad[5] = state;
            break;
        case sf::Keyboard::E:
            _keypad[6] = state;
            break;
        case sf::Keyboard::R:
            _keypad[13] = state;
            break;
        case sf::Keyboard::A:
            _keypad[7] = state;
            break;
        case sf::Keyboard::S:
            _keypad[8] = state;
            break;
        case sf::Keyboard::D:
            _keypad[9] = state;
            break;
        case sf::Keyboard::F:
            _keypad[14] = state;
            break;
        case sf::Keyboard::Z:
            _keypad[10] = state;
            break;
        case sf::Keyboard::X:
            _keypad[0] = state;
            break;
        case sf::Keyboard::C:
            _keypad[11] = state;
            break;
        case sf::Keyboard::V:
            _keypad[15] = state;
            break;
        case sf::Keyboard::Escape:
            _graphics.window.close();
            break;
        default:
            break;
	}
}


void Chip8::handle_opcode()
{
    _opcode = _memory[_cpu.pc] << 8 | _memory[_cpu.pc + 1];
    const uint16_t curr_pc = _cpu.pc;
    bool exec_inst = false;
    for (auto const& entry : _opcode_table)
    {
        if (((entry.first & _opcode) == _opcode) && ((entry.first | _opcode) == entry.first))
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
        hex << "0x" << std::hex << _opcode << " at address 0x" << std::hex << curr_pc << std::endl;
        throw std::runtime_error("Tried to execute illegal instruction: " + hex.str());
    }
    else
    {
        std::cout << "Executed Opcode " << "0x" << std::hex << _opcode << " at address 0x" << std::hex << curr_pc << std::endl;
    }
}

void Chip8::update_timers()
{
    if (_timer._delay > 0)
    {
        _timer._delay--;
    }
    if (_timer._sound > 0)
    {
        if (_timer._sound == 1)
        {
            std::cout << '\a' << std::endl; // put in a sound alert
        }
        _timer._sound--;  
    }
}


void Chip8::load_game(const std::string& path)
{
    std::ifstream game_file (path, std::ifstream::ate | std::ifstream::binary);
    if (!game_file)
    {
        throw std::invalid_argument("Cannot open ROM.");
    }
    if (game_file.tellg() > (_memory.size() - PROGRAM_START_ADDRESS))
    {
        throw std::overflow_error("Game file is too big.");
    }
    game_file.seekg(std::ifstream::beg);
    game_file.read(reinterpret_cast<char*>(_memory.data() + PROGRAM_START_ADDRESS), MEMORY_SIZE);
}

void Chip8::init_opcode_args()
{
    _opcode_args.nnn = _opcode & 0x0FFF;
    _opcode_args.nn = _opcode & 0x00FF;
    _opcode_args.n = _opcode & 0x000F;
    _opcode_args.x = (_opcode & 0x0F00) >> 8;
    _opcode_args.y = (_opcode & 0x00F0) >> 4;
}

void Chip8::init_font()
{
    std::array<uint8_t, Chip8::FONT_SET_SIZE> font_set
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
    std::copy(font_set.begin(), font_set.end(), _memory.begin());
}

void Chip8::init_opcode_table()
{
  _opcode_table[0x00E0] = &Chip8::inst_00E0;
  _opcode_table[0x00EE] = &Chip8::inst_00EE;
  _opcode_table[0x1FFF] = &Chip8::inst_1NNN;
  _opcode_table[0x2FFF] = &Chip8::inst_2NNN;
  _opcode_table[0x3FFF] = &Chip8::inst_3XNN;
  _opcode_table[0x4FFF] = &Chip8::inst_4XNN;
  _opcode_table[0x5FF0] = &Chip8::inst_5XY0;
  _opcode_table[0x6FFF] = &Chip8::inst_6XNN;
  _opcode_table[0x7FFF] = &Chip8::inst_7XNN;
  _opcode_table[0x8FF0] = &Chip8::inst_8XY0;
  _opcode_table[0x8FF1] = &Chip8::inst_8XY1;
  _opcode_table[0x8FF2] = &Chip8::inst_8XY2;
  _opcode_table[0x8FF3] = &Chip8::inst_8XY3;
  _opcode_table[0x8FF4] = &Chip8::inst_8XY4;
  _opcode_table[0x8FF5] = &Chip8::inst_8XY5;
  _opcode_table[0x8FF6] = &Chip8::inst_8XY6;
  _opcode_table[0x8FF7] = &Chip8::inst_8XY7;
  _opcode_table[0x8FFE] = &Chip8::inst_8XYE;
  _opcode_table[0x9FF0] = &Chip8::inst_9XY0; 
  _opcode_table[0xAFFF] = &Chip8::inst_ANNN;
  _opcode_table[0xBFFF] = &Chip8::inst_BNNN;
  _opcode_table[0xCFFF] = &Chip8::inst_CXNN;
  _opcode_table[0xDFFF] = &Chip8::inst_DXYN;
  _opcode_table[0xEF9E] = &Chip8::inst_EX9E;
  _opcode_table[0xEFA1] = &Chip8::inst_EXA1;
  _opcode_table[0xFF07] = &Chip8::inst_FX07;
  _opcode_table[0xFF0A] = &Chip8::inst_FX0A;
  _opcode_table[0xFF15] = &Chip8::inst_FX15;
  _opcode_table[0xFF18] = &Chip8::inst_FX18;
  _opcode_table[0xFF1E] = &Chip8::inst_FX1E;
  _opcode_table[0xFF29] = &Chip8::inst_FX29;
  _opcode_table[0xFF33] = &Chip8::inst_FX33;
  _opcode_table[0xFF55] = &Chip8::inst_FX55;
  _opcode_table[0xFF65] = &Chip8::inst_FX65;
}

void Chip8::inst_00E0()
{
    std::memset(&_display, 0, sizeof(_display));
    _cpu.pc += 2;
}

void Chip8::inst_00EE()
{
    if (_cpu.sp > 0)
    {
        _cpu.pc = _stack[--_cpu.sp];
    }
    else
    {
        throw std::underflow_error("Stack underflow.");
    }
    _cpu.pc += 2;
}

void Chip8::inst_1NNN()
{
    _cpu.pc = _opcode_args.nnn;
}

void Chip8::inst_2NNN()
{
    if (_cpu.sp < (_stack.size() - 1))
    {
        _stack[_cpu.sp++] = _cpu.pc;
        _cpu.pc = _opcode_args.nnn;
    }
    else
    {
        throw std::overflow_error("Stack overflow.");
    }
}

void Chip8::inst_3XNN()
{
    if (_cpu.v[_opcode_args.x] == _opcode_args.nn)
    {
        _cpu.pc += 2;
    }
    _cpu.pc += 2;
}

void Chip8::inst_4XNN()
{
    if(_cpu.v[_opcode_args.x] != _opcode_args.nn)
    {
        _cpu.pc += 2;
    }
    _cpu.pc += 2;
}

void Chip8::inst_5XY0()
{
    if (_cpu.v[_opcode_args.x] == _cpu.v[_opcode_args.y])
    {
        _cpu.pc += 2;
    }
    _cpu.pc += 2;
}


void Chip8::inst_6XNN()
{
    _cpu.v[_opcode_args.x] = _opcode_args.nn;
    _cpu.pc += 2;
}

void Chip8::inst_7XNN()
{
    _cpu.v[_opcode_args.x] += _opcode_args.nn;
    _cpu.pc += 2;
}

void Chip8::inst_8XY0()
{
    _cpu.v[_opcode_args.x] = _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY1()
{
    _cpu.v[_opcode_args.x] |= _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY2()
{
    _cpu.v[_opcode_args.x] &= _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY3()
{
    _cpu.v[_opcode_args.x] ^= _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY4()
{
    _cpu.v[0xF] = (_cpu.v[_opcode_args.x] + _cpu.v[_opcode_args.y]) > 0x00FF ? 1 : 0;
    _cpu.v[_opcode_args.x] += _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY5()
{
    _cpu.v[0xF] = _cpu.v[_opcode_args.x] < _cpu.v[_opcode_args.y] ? 0 : 1;
    _cpu.v[_opcode_args.x] -= _cpu.v[_opcode_args.y];
    _cpu.pc += 2;
}

void Chip8::inst_8XY6()
{
    _cpu.v[0xF] = _cpu.v[_opcode_args.x] & 0x1u;
    _cpu.v[_opcode_args.x] >>= 1;
    _cpu.pc += 2;
}

void Chip8::inst_8XY7()
{
    _cpu.v[0xF] = _cpu.v[_opcode_args.x] > _cpu.v[_opcode_args.y] ? 0 : 1;
    _cpu.v[_opcode_args.x] = _cpu.v[_opcode_args.y] - _cpu.v[_opcode_args.x];
    _cpu.pc += 2;
}

void Chip8::inst_8XYE()
{
    _cpu.v[0xF] = _cpu.v[_opcode_args.x] >> 7;
    _cpu.v[_opcode_args.x] <<= 1;
    _cpu.pc += 2;
}   

void Chip8::inst_9XY0()
{
    if (_cpu.v[_opcode_args.x] != _cpu.v[_opcode_args.y])
    {
        _cpu.pc += 2;
    }    
    _cpu.pc += 2;
}

void Chip8::inst_ANNN()
{
    _cpu.i = _opcode_args.nnn;
    _cpu.pc += 2;
}

void Chip8::inst_BNNN()
{
    _cpu.pc = _cpu.v[0] + _opcode_args.nnn;
}

void Chip8::inst_CXNN()
{
    _cpu.v[_opcode_args.x] = (std::rand() % (MAX_VAL + 1)) & _opcode_args.nn;
    _cpu.pc += 2;
}

void Chip8::inst_DXYN()
{
    uint8_t coord_x = _cpu.v[_opcode_args.x];
    uint8_t coord_y = _cpu.v[_opcode_args.y];
    uint8_t sprite_height = _opcode_args.n;
    _cpu.v[0xF] = 0; // we assume that theres no collision for now.
    for (uint8_t row = 0; row < sprite_height; row++)
    {
        uint8_t px_to_draw = _memory[_cpu.i + row];
        for (uint8_t bit_pos {}; bit_pos < 8; bit_pos++)
        {
            uint8_t& curr_px = _display[(coord_y + row) % 32][(coord_x + bit_pos) % 64];
            if ((px_to_draw & (0x80 >> bit_pos)) != 0)
            {
                if (curr_px != 0)
                {
                    _cpu.v[0xF] = 1;
                }
                curr_px ^= 1;
            }
        }
    }
    _graphics.draw_window<RES_WIDTH, RES_HEIGHT>(_display);
    _cpu.pc += 2;
}

void Chip8::inst_EX9E()
{
    if (_keypad[_cpu.v[_opcode_args.x]] == static_cast<uint16_t>(Key_state::pressed))
    {
        _cpu.pc += 2;
    }
    _cpu.pc += 2;
}

void Chip8::inst_EXA1()
{
    if (_keypad[_cpu.v[_opcode_args.x]] == static_cast<uint16_t>(Key_state::released))
    {
        _cpu.pc += 2;
    }
    _cpu.pc += 2;
}

void Chip8::inst_FX07()
{
    _cpu.v[_opcode_args.x] = _timer._delay;
    _cpu.pc += 2;
}

void Chip8::inst_FX0A()
{
    for (int i = 0; i < _keypad.size(); i++)
    {
        if (_keypad[i])
        {
            _cpu.v[_opcode_args.x] = i;
            _cpu.pc += 2;
            return;
        }
    }
}

void Chip8::inst_FX15()
{
    _timer._delay = _cpu.v[_opcode_args.x];
    _cpu.pc += 2;
}

void Chip8::inst_FX18()
{
    _timer._sound = _cpu.v[_opcode_args.x];
    _cpu.pc += 2;
}

void Chip8::inst_FX1E()
{
    _cpu.v[0xF] = (_cpu.i + _cpu.v[_opcode_args.x]) > 0x00FF ? 1 : 0;
    _cpu.i += _cpu.v[_opcode_args.x];
    _cpu.pc += 2;
}

void Chip8::inst_FX29()
{
    _cpu.i = _cpu.v[_opcode_args.x] * 5;
    _cpu.pc += 2;
}

void Chip8::inst_FX33()
{
    _memory[_cpu.i] = _cpu.v[_opcode_args.x] / 100;
    _memory[_cpu.i + 1] = (_cpu.v[_opcode_args.x] / 10) % 10;
    _memory[_cpu.i + 2] = _cpu.v[_opcode_args.x] % 10;
    _cpu.pc += 2;
}

void Chip8::inst_FX55()
{
    for (int i = 0; i <= _opcode_args.x; i++)
    {
        _memory[_cpu.i + i] = _cpu.v[i];
    }
    _cpu.pc += 2;
}

void Chip8::inst_FX65()
{
    for (int i = 0; i <= _opcode_args.x; i++)
    {
        _cpu.v[i] = _memory[_cpu.i + i];
    }
    _cpu.pc += 2;
}
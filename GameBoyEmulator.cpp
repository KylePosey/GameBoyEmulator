#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>
#include "GameBoyEmulator.h"
#include "cpu.h"
#include "ppu.h"
#include "instructions.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"
#include <glad/glad.h>
#include <glfw3.h>
#include <KHR/khrplatform.h>
#include <iomanip>
#include <chrono>
#include <thread>

#include <io.h>

using namespace std;

int windowScale = 4;

uint8_t joypadState = 0xFF; // Initialize to all buttons unpressed (1)
uint8_t joypadStateOld = 0xFF;

std::vector<char> ReadAllBytes(char const* filename);
void handleInterrupts(CPU* cpu);
void updateTimer(CPU* cpu, int cycles);

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void processInput(GLFWwindow* window, CPU* cpu)
{
    // Start with all buttons unpressed
    uint8_t directionState = 0x0F; // Direction buttons (bits 0-3 set to 1)
    uint8_t actionState = 0x0F;    // Action buttons (bits 4-7 set to 1)

    // Check Direction keys (Right, Left, Up, Down)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        directionState &= ~0x01; // Right button (bit 0)
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        directionState &= ~0x02; // Left button (bit 1)
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        directionState &= ~0x04; // Up button (bit 2)
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        directionState &= ~0x08; // Down button (bit 3)
    }

    // Check Action keys (A button, Select button)
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        actionState &= ~0x01; // A button (bit 0 in action state)
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        actionState &= ~0x02; // Select button (bit 1 in action state)
    }

    // Grab the state of select bits 4 and 5 separately
    bool selectAction = !(cpu->ram[0xFF00] & 0x10); // Bit 4 (0x10)
    bool selectDirection = !(cpu->ram[0xFF00] & 0x20); // Bit 5 (0x20)

    // Start with all buttons unpressed in the new joypad state
    uint8_t newJoypadState = 0xFF;

    // Determine which set of buttons to send (directional or action)
    if (selectAction && !selectDirection) {
        // Action buttons selected (bit 4 is 0)
        cpu->ram[0xFF00] = actionState; // Set action button state
    }
    else if (selectDirection && !selectAction) {
        // Direction buttons selected (bit 5 is 0)
        cpu->ram[0xFF00] = directionState;// Set direction button state
    }

    if (newJoypadState != cpu->ram[0xFF00]) {
        cpu->ram[0xFF0F] |= 0x10; // Trigger Joypad interrupt if state changed
    }
}


void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
        else
        {
            std::cout << "SHADER COMPILATION SUCCESS\n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
        else
        {
            std::cout << "SHADER LINKING SUCCESS\n";
        }
    }
}

GLuint VAO, VBO, EBO, texture;
GLuint shaderProgram;

static const char* readFile(const char* filePath)
{
    std::ifstream file(filePath);
    if (file.is_open())
    {
        std::filebuf* pbuf = file.rdbuf();
        std::size_t size = pbuf->pubseekoff(0, file.end, file.in);
        pbuf->pubseekpos(0, file.in);
        char* buffer = new char[size];
        // get file data
        pbuf->sgetn(buffer, size);
        file.close();

        return buffer;
    }

    return nullptr;
}

void inithwreg(CPU* cpu)
{
    //cpu->ram[0xFF00] = 0xCF;
    //cpu->ram[0xFF00] = 0xF;
    cpu->ram[0xFF02] = 0x7E;
    cpu->ram[0xFF04] = 0xAB;
    cpu->ram[0xFF07] = 0xF8;
    cpu->ram[0xFF0F] = 0xE1;
    cpu->ram[0xFF10] = 0x80;
    cpu->ram[0xFF11] = 0xBF;
    cpu->ram[0xFF12] = 0xF3;
    cpu->ram[0xFF13] = 0xFF;
    cpu->ram[0xFF14] = 0xBF;
    cpu->ram[0xFF16] = 0x3F;
    cpu->ram[0xFF18] = 0xFF;
    cpu->ram[0xFF19] = 0xBF;
    cpu->ram[0xFF1A] = 0x7F;
    cpu->ram[0xFF1B] = 0xFF;
    cpu->ram[0xFF1C] = 0x9F;
    cpu->ram[0xFF1D] = 0xFF;
    cpu->ram[0xFF1E] = 0xBF;
    cpu->ram[0xFF20] = 0xFF;
    cpu->ram[0xFF23] = 0xBF;
    cpu->ram[0xFF24] = 0x77;
    cpu->ram[0xFF25] = 0xF3;
    cpu->ram[0xFF26] = 0xF1;
    cpu->ram[0xFF40] = 0x91;
    cpu->ram[0xFF41] = 0x85;
    cpu->ram[0xFF46] = 0xFF;
    cpu->ram[0xFF47] = 0xFC;

    cpu->ram[0xFF05] = 0x00; // TIMA - Timer Counter
    cpu->ram[0xFF06] = 0x00; // TMA - Timer Modulo
    cpu->ram[0xFF07] = 0x00; // TAC - Timer Control
    cpu->ram[0xFF10] = 0x80; // NR10 - Sound Mode 1 Sweep
    cpu->ram[0xFF11] = 0xBF; // NR11 - Sound Mode 1 Length/Wave pattern duty
    cpu->ram[0xFF12] = 0xF3; // NR12 - Sound Mode 1 Envelope
    cpu->ram[0xFF14] = 0xBF; // NR14 - Sound Mode 1 Frequency high byte
    cpu->ram[0xFF16] = 0x3F; // NR21 - Sound Mode 2 Length/Wave pattern duty
    cpu->ram[0xFF17] = 0x00; // NR22 - Sound Mode 2 Envelope
    cpu->ram[0xFF19] = 0xBF; // NR24 - Sound Mode 2 Frequency high byte
    cpu->ram[0xFF1A] = 0x7F; // NR30 - Sound Mode 3 Sound on/off
    cpu->ram[0xFF1B] = 0xFF; // NR31 - Sound Mode 3 Length
    cpu->ram[0xFF1C] = 0x9F; // NR32 - Sound Mode 3 Output level
    cpu->ram[0xFF1E] = 0xBF; // NR33 - Sound Mode 3 Frequency high byte
    cpu->ram[0xFF20] = 0xFF; // NR41 - Sound Mode 4 Length
    cpu->ram[0xFF21] = 0x00; // NR42 - Sound Mode 4 Envelope
    cpu->ram[0xFF22] = 0x00; // NR43 - Sound Mode 4 Polynomial counter
    cpu->ram[0xFF23] = 0xBF; // NR30 - Sound Mode 4 Frequency high byte
    cpu->ram[0xFF24] = 0x77; // NR50 - Channel control / On-Off / Volume
    cpu->ram[0xFF25] = 0xF3; // NR51 - Selection of Sound output terminal
    cpu->ram[0xFF26] = 0xF1; // NR52 - Sound on/off

    cpu->ram[0xFF40] = 0x91; // LCDC - LCD Control
    cpu->ram[0xFF42] = 0x00; // SCY - Scroll Y
    cpu->ram[0xFF43] = 0x00; // SCX - Scroll X
    cpu->ram[0xFF45] = 0x00; // LYC - LY Compare
    cpu->ram[0xFF47] = 0xFC; // BGP - BG Palette Data
    cpu->ram[0xFF48] = 0xFF; // OBP0 - Object Palette 0 Data
    cpu->ram[0xFF49] = 0xFF; // OBP1 - Object Palette 1 Data
    cpu->ram[0xFF4A] = 0x00; // WY - Window Y Position
    cpu->ram[0xFF4B] = 0x00; // WX - Window X Position

    //cpu->ram[0xFFFF] = 0x00; // IE - Interrupt Enable
}

void initShaders() {
    int vertexScale = windowScale / windowScale;
    // Vertex data for a fullscreen quad
    float vertices[] = {
        // positions     // texture coords
        -vertexScale,  vertexScale,   0.0f, 1.0f,
        -vertexScale, -vertexScale,   0.0f, 0.0f,
         vertexScale, -vertexScale,   1.0f, 0.0f,

         vertexScale,  vertexScale,   1.0f, 1.0f,
         vertexScale, -vertexScale,   1.0f, 0.0f,
        -vertexScale,  vertexScale,   0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // Setup Vertex Array Object, Vertex Buffer Object and Element Buffer Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Load and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = readFile("vertex_shader.glsl");
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = readFile("fragment_shader.glsl");
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void updateTexture(PPU& ppu, GLuint texture) {
    // Get pixel data from the PPU
    std::vector<uint8_t> colorbuffer(160 * 144 * 3 * 3);

    for (size_t i = 0; i < 160 * 144 * 3; ++i) {
        uint8_t color = ppu.framebuffer[i];
        colorbuffer[i * 3 + 0] = color;
        colorbuffer[i * 3 + 1] = color;
        colorbuffer[i * 3 + 2] = color;
    }

    // Update texture with new pixel data
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, colorbuffer.data());

    // Set texture parameters
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}

void load_ram(CPU& cpu, char const* filename, bool use_only_boot_rom, bool overwrite_game_rom) {
    // Read the ROM file into a vector of bytes
    std::vector<char> vec = ReadAllBytes(filename);

    // Ensure full_rom is resized to fit the entire ROM
    cpu.full_rom.resize(vec.size());
    std::copy(vec.begin(), vec.end(), cpu.full_rom.begin());

    // Initialize external RAM to zero or load from a save file if needed
    cpu.full_ram.resize(0x8000, 0);  // Up to 32KB of RAM

    if (!use_only_boot_rom) {
        // Load the boot ROM
        std::vector<char> bootstrap = ReadAllBytes("dmg_boot.bin");
        std::vector<uint8_t> bootstrap_array(bootstrap.begin(), bootstrap.end());

        for (size_t i = 0; i < bootstrap_array.size() && i < cpu.ram.size(); ++i) {
            cpu.ram[i] = bootstrap_array[i];
        }

        // Load the initial part of the ROM into memory at 0x0100
        for (size_t i = 0; i < cpu.full_rom.size() && (0x0100 + i) < cpu.ram.size(); ++i) {
            cpu.ram[0x0100 + i] = cpu.full_rom[i];
        }
    }
    else {
        // Directly load the ROM into the memory for testing purposes
        for (size_t i = 0; i < cpu.full_rom.size() && i < cpu.ram.size(); ++i) {
            cpu.ram[i] = cpu.full_rom[i];
        }
    }
}

void dump_ram(const CPU& cpu, const std::string& filename)
{
    // Open the file in binary mode
    std::ofstream ofs(filename, std::ios::binary | std::ios::out);

    if (!ofs)
    {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    // Write the entire RAM to the file
    ofs.write(reinterpret_cast<const char*>(cpu.ram.data()), cpu.ram.size());

    // Close the file
    ofs.close();

    if (ofs)
    {
        std::cout << "RAM successfully exported to " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not complete writing to the file." << std::endl;
    }
}

using namespace std::chrono;

std::chrono::steady_clock::time_point start_time = std::chrono::high_resolution_clock::now();
std::chrono::nanoseconds target_duration = std::chrono::nanoseconds(0);

int wait_time = 238;

void busy_wait(int ret)
{
    auto target_duration = std::chrono::nanoseconds(wait_time * ret);
    auto start_time = std::chrono::high_resolution_clock::now();

    while (std::chrono::high_resolution_clock::now() - start_time < target_duration) {
        // Busy-waiting
    }
}

bool step = false;

void step_pc()
{
    step = true;
}

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    printf("GLFW INIT SUCCESS\n");
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "GB Emulator", NULL, NULL);
    if (window == NULL)
    {
        printf("WINDOW CREATE FAILURE\n");
        glfwTerminate();
        return -1;
    }
    printf("WINDOW CREATE SUCCESS\n");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("GLAD INIT FAILURE\n");
        return -1;
    }
    printf("GLAD INIT SUCCESS\n");
    glViewport(0, (720 / 2) - ((144 * windowScale) / 2), 160 * windowScale, 144 * windowScale);
    //End of Gl init

    //Start Gameboy emulation init
    CPU cpu;
    PPU ppu;
    cpu.ppu = ppu;
    

    bool logging_enabled = false;
    bool runbootrom = false;
    const char* romname = "tetris.gb";
    load_ram(cpu, romname, !runbootrom, !runbootrom);

    if (!runbootrom)
    {
        cpu.a = 0x01;
        cpu.b = 0X00;
        cpu.c = 0x13;
        cpu.d = 0x00;
        cpu.e = 0xD8;
        cpu.h = 0x01;
        cpu.l = 0x4D;
        cpu.sp = 0xFFFE;
        cpu.pc = 0x0100;
        cpu.z_flag = true;
        cpu.n_flag = false;
        cpu.h_flag = true;
        cpu.c_flag = true;
        inithwreg(&cpu);
    }
    else
    {
        cpu.a = 0x00;
        cpu.b = 0x00;
        cpu.c = 0x00;
        cpu.d = 0x00;
        cpu.e = 0x00;
        cpu.h = 0x00;
        cpu.l = 0x00;
        cpu.sp = 0xFFFF;
        cpu.pc = 0x0100;
        cpu.z_flag = false;
        cpu.n_flag = false;
        cpu.h_flag = false;
        cpu.c_flag = false;
    }

    //dump_ram(cpu, "ram.bin");
    //InsertNintendoLogo(cpu);

    //Shaders
    initShaders();


    //60 FPS
    const double fpsLimit = 1.0 / 60.0;
    double lastUpdateTime = 0;
    double lastFrameTime = 0;
    int frame = 0;

    //Texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 160, 144, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); // Initial empty texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    std::ofstream logFile("logfile.txt");
    logFile.clear();

    byte h = cpu.h;
    byte l = cpu.l;

    static MemoryEditor mem_edit;

    int renders = 0;

    bool romhandoff = false;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = start_time;
    int render_count = 0;

    

    int last_ret = 0;
    double fps = 0;

    bool play = true;
    bool stop = false;

    while (!glfwWindowShouldClose(window))
    {
        //Setup current frame timing
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = now - last_frame_time;

        //Process input
        processInput(window, &cpu);

        uint16_t pc = cpu.pc;

        if (logFile.is_open())
        {
            logFile << "A:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.a) << " ";
            uint8_t flags = 0;
            if (cpu.c_flag) flags |= 0x10; // Set bit 4 (Carry flag)
            if (cpu.h_flag) flags |= 0x20; // Set bit 5 (Half-carry flag)
            if (cpu.n_flag) flags |= 0x40; // Set bit 6 (Add/Subtract flag)
            if (cpu.z_flag) flags |= 0x80; // Set bit 7 (Zero flag)
            logFile << "F:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(flags) << " ";
            logFile << "B:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.b) << " ";
            logFile << "C:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.c) << " ";
            logFile << "D:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.d) << " ";
            logFile << "E:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.e) << " ";
            logFile << "H:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.h) << " ";
            logFile << "L:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.l) << " ";
            logFile << "SP:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << static_cast<int>(cpu.sp) << " ";
            logFile << "PC:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << static_cast<int>(pc) << " ";
            logFile << "PCMEM:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.ram[pc]) << ",";
            logFile << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.ram[pc + 0x01]) << ",";
            logFile << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.ram[pc + 0x02]) << ",";
            logFile << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.ram[pc + 0x03]) << "\n";

        }

        int ret = 0;

        if (cpu.halt)
        {
            if (cpu.readMemory(0xFFFF) != 0x00 && cpu.readMemory(0xFF0F) != 0x00)
            {
                cpu.halt = false;
            }
        }

        int defCycles = 3;

        if (runbootrom)
        {
            if ((step || play || cpu.ram[0xFF50] != 0x01))
            {
                step = false;
                if (cpu.ram[0xFF50] == 0x00)
                {
                    byte i = cpu.full_rom[cpu.pc];
                    ret = decode_instruction(i, &cpu);
                    cpu.m_cycles += ret;
                }
                else
                {
                    if (cpu.ram[0xFF50] == 0x01 && !romhandoff)
                    {
                        printf("Rom Hand Off\n");
                        //wait_time = 50;
                        //play = false;
                        romhandoff = true;
                        std::vector<char> vec = ReadAllBytes(romname);
                        std::vector<uint8_t> vec_array(vec.begin(), vec.end());
                        for (size_t i = 0; i < vec_array.size(); ++i)
                        {
                            cpu.ram[i] = vec_array[i];
                        }
                        dump_ram(cpu, "ram2.bin");
                    }
                    byte i = cpu.ram[cpu.pc];
                    ret = decode_instruction(i, &cpu);
                    cpu.m_cycles += ret;
                }

                updateTimer(&cpu, ret);
                busy_wait(ret);

                if (cpu.deferredIME)
                {
                    cpu.deferredIMECycles++;
                    if (cpu.deferredIMECycles == defCycles)
                    {
                        cpu.IME_flag = true;
                        cpu.deferredIME = false;
                    }
                }
            }
        }
        else
        {
            if (step || play )
            {
                if (cpu.ram[0xFF50] == 0x01 && !romhandoff)
                {
                    play = false;
                    romhandoff = true;
                }

                step = false;
                byte i = cpu.ram[cpu.pc];
                ret = decode_instruction(i, &cpu);
                cpu.m_cycles += ret;

                updateTimer(&cpu, ret);
                busy_wait(ret);

                if (cpu.deferredIME)
                {
                    cpu.deferredIMECycles++;
                    if (cpu.deferredIMECycles == defCycles)
                    {
                        cpu.IME_flag = true;
                        cpu.deferredIME = false;
                    }
                }
            }
        }

        

        if (cpu.m_cycles >= 456)
        {
            if (cpu.ram[0xFF44] < 144)
            {
                cpu.writePixels();
            }

            if (cpu.ram[0xFF44] == 144)
            {
                cpu.ram[0xFF0F] |= 0x01;
            }

            if (cpu.ram[0xFF44] == 153)
            {
                start_time = high_resolution_clock::now();

                cpu.ram[0xFF44] = 0;
                updateTexture(cpu.ppu, texture);

                render_count++;

                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed_time = now - last_frame_time;

            }
            else
            {
                cpu.ram[0XFF44]++;
            }
            cpu.m_cycles = 0;

            //handleInterrupts(&cpu);
        }

        if (elapsed_time.count() >= 1.0 / 60.0)
        {
            fps = static_cast<double>(render_count) / elapsed_time.count();
            render_count = 0;
            last_frame_time = now;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Render
            glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
            //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            int listbox_items[] = { 8, 16, 32, 64, 128 };
            int listbox_item_current = 1;

            mem_edit.DrawWindow("RAM", &cpu.ram, 0xFFFF + 1);
            ImGui::Begin(" ");
            ImGui::SetWindowFontScale(1.5);
            ImGui::Text("FPS: %.2f", fps);
            ImGui::End();
            ImGui::Begin("CPU");
            ImGui::SetWindowFontScale(1.40);
            ImGui::Text("A:%02X  B:%02X  C:%02X  D:%02X  E:%02X  H:%02X  L:%02X", cpu.a, cpu.b, cpu.c, cpu.d, cpu.e, cpu.h, cpu.l);
            std::string flags = "";
            flags += cpu.z_flag ? 'Z' : '-';
            flags += cpu.n_flag ? 'N' : '-';
            flags += cpu.h_flag ? 'H' : '-';
            flags += cpu.c_flag ? 'C' : '-';
            ImGui::Text("Flags: %s", flags.c_str());
            ImGui::Text("I:%02X  PC:%04X  SP:%04X    IMM:%02X %02X", cpu.ram[cpu.pc], cpu.pc, cpu.sp, cpu.ram[pc + 1], cpu.ram[pc + 2]);
            if (ImGui::Button("Step"))
            {
                step = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Play"))
            {
                play = true;
                step = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                play = false;
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if (cpu.ram[0xff02] == 0x81)
        {
             char c = cpu.ram[0xff01];
             //printf("%c", c);
             cpu.ram[0xff02] = 0x0;
         }

        updateTimer(&cpu, ret);
        handleInterrupts(&cpu);
    }
    logFile.close();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void handleInterrupts(CPU* cpu)
{
    // Read the Interrupt Request (IF) and Interrupt Enable (IE) registers
    uint8_t interruptRequest = cpu->ram[0xFF0F];
    uint8_t interruptEnable = cpu->ram[0xFFFF];

    

    if (cpu->IME_flag)
    {
        if ((interruptRequest & 0x01) && (interruptEnable & 0x01))
        {
            // V-Blank Interrupt
            cpu->IME_flag = false; // Disable further interrupts
            cpu->writeMemory(0xFF0F, cpu->readMemory(0xFF0F) & ~0x01); // Clear interrupt flag
            cpu->sp -= 2;
            cpu->writeMemory(cpu->sp + 1, (cpu->pc >> 8) & 0xFF);
            cpu->writeMemory(cpu->sp, cpu->pc & 0xFF);
            cpu->pc = 0x0040; // Jump to V-Blank interrupt vector
        }

        else if ((interruptRequest & 0x04) && (interruptEnable & 0x04))  
        {
            printf("Timer inter\n");
            cpu->IME_flag = false; // Disable further interrupts
            cpu->writeMemory(0xFF0F, cpu->readMemory(0xFF0F) & ~0x04);
            // Push the current PC onto the stack
            cpu->sp -= 2;
            cpu->writeMemory(cpu->sp + 1, (cpu->pc >> 8) & 0xFF);
            cpu->writeMemory(cpu->sp, cpu->pc & 0xFF);
            cpu->pc = 0x0050;
        }
        else if ((interruptRequest & 0x10) && (interruptEnable & 0x10))
        {
            cpu->IME_flag = false; // Disable further interrupts
            cpu->writeMemory(0xFF0F, interruptRequest & ~0x10); // Clear the interrupt flag
            // Push the current PC onto the stack
            cpu->sp -= 2;
            cpu->writeMemory(cpu->sp + 1, (cpu->pc >> 8) & 0xFF);
            cpu->writeMemory(cpu->sp, cpu->pc & 0xFF);
            cpu->pc = 0x0060; // Joypad Interrupt vector
        }
    }    
}

void updateTimer(CPU* cpu, int cycles)
{
    // Update DIV register
    cpu->div_cycles += cycles;
    if (cpu->div_cycles >= 256)
    {
        cpu->div_cycles -= 256;
        cpu->ram[0xFF04]++; // Increment DIV
    }

    uint8_t tac = cpu->ram[0xFF07];

    if (tac & 0x04) // Timer enabled (bit 2)
    {
        // Determine the frequency based on TAC bits 1-0
        int freq = 0;
        switch (tac & 0x03)
        {
        case 0x00: freq = 1024; break;  // 4096 Hz
        case 0x01: freq = 16; break;    // 262144 Hz
        case 0x02: freq = 64; break;    // 65536 Hz
        case 0x03: freq = 256; break;   // 16384 Hz
        }

        cpu->timer_cycles += cycles;

        if (cpu->timer_cycles >= freq)
        {
            cpu->timer_cycles -= freq;
            cpu->ram[0xFF05]++;

            //printf("TIMA: %02X, TMA: %02X, TAC: %02X, Timer Cycles: %d\n", cpu->ram[0xFF05], cpu->ram[0xFF06], tac, cpu->timer_cycles);

            if (cpu->ram[0xFF05] == 0x00) // Overflow
            {
                cpu->ram[0xFF05] = cpu->ram[0xFF06]; // Reload TIMA with TMA
                cpu->ram[0xFF0F] |= 0x04; // Request Timer Interrupt (bit 2 in IF)

                //printf("Timer Overflow! Timer Interrupt Requested\n");
            }
        }
    }
}

void renderFrame(PPU* ppu) {
    // Implement rendering logic here.
    // For example, you might read the PPU's framebuffer data and use OpenGL to draw it.
}

std::vector<char> ReadAllBytes(char const* filename)
{
    ifstream ifs(filename, ios::binary | ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}
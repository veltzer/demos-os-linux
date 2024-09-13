#include <iostream>
#include <stdexcept>
#include <string>

class File {
private:
    FILE* m_file;
    std::string m_filename;

public:
    File(const std::string& filename, const std::string& mode) : m_filename(filename) {
        m_file = fopen(filename.c_str(), mode.c_str());
        if (m_file == nullptr) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::cout << "File opened: " << m_filename << std::endl;
    }

    ~File() {
        if (m_file) {
            fclose(m_file);
            std::cout << "File closed: " << m_filename << std::endl;
        }
    }

    // Prevent copying
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    void write(const std::string& data) {
        if (fputs(data.c_str(), m_file) == EOF) {
            throw std::runtime_error("Failed to write to file: " + m_filename);
        }
    }

    // Bonus: Additional file operations
    std::string read(size_t size) {
        char* buffer = new char[size + 1];
        size_t bytesRead = fread(buffer, 1, size, m_file);
        buffer[bytesRead] = '\0';
        std::string result(buffer);
        delete[] buffer;
        return result;
    }

    void seek(long offset, int origin) {
        if (fseek(m_file, offset, origin) != 0) {
            throw std::runtime_error("Failed to seek in file: " + m_filename);
        }
    }
};

void useFile(const std::string& filename) {
    try {
        File file(filename, "w");
        file.write("Hello, RAII!");
        // File is automatically closed when 'file' goes out of scope,
        // even if an exception is thrown
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    useFile("example.txt");

    // Bonus: Demonstrate additional operations
    try {
        File file("example.txt", "r+");
        std::cout << "File content: " << file.read(100) << std::endl;
        file.seek(0, SEEK_SET);
        file.write("RAII is awesome!");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

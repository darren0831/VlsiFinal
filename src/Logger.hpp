#ifndef VLSI_FINAL_PROJECT_LOGGER_HPP_
#define VLSI_FINAL_PROJECT_LOGGER_HPP_

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>

class Logger {
public:
    Logger(std::string filename) {
        if (filename == "") {
            enabled = false;
            fout = nullptr;
        } else if (filename == "STDOUT") {
            enabled = true;
            fout = stdout;
        } else {
            enabled = true;
            fout = fopen(filename.c_str(), "w");
            if (fout == nullptr) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                enabled = false;
            }
        }
    }

    Logger(const Logger& that) = delete;

    Logger(Logger&& that) = delete;

    ~Logger() {
        if (fout != nullptr && fout != stdout) {
            fclose(fout);
        }
    }

    Logger& operator=(const Logger& that) = delete;

    Logger& operator=(Logger&& that) = delete;

    void info(const char* fmt, ...) {
        if (!enabled) {
            return;
        }
        fprintf(fout, "[INFO] ");
        va_list args;
        va_start(args, fmt);
        vfprintf(fout, fmt, args);
        va_end(args);
    }

    void warning(const char* fmt, ...) {
        if (!enabled) {
            return;
        }
        fprintf(fout, "[WARN] ");
        va_list args;
        va_start(args, fmt);
        vfprintf(fout, fmt, args);
        va_end(args);
    }

    void error(const char* fmt, ...) {
        if (!enabled) {
            return;
        }
        fprintf(fout, "[ERROR] ");
        va_list args;
        va_start(args, fmt);
        vfprintf(fout, fmt, args);
        va_end(args);
    }

private:
    FILE* fout;
    bool enabled;
};

#endif // VLSI_FINAL_PROJECT_LOGGER_HPP_

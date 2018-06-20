#ifndef VLSI_FINAL_PROJECT_LOGGER_HPP_
#define VLSI_FINAL_PROJECT_LOGGER_HPP_

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <string>

/**
 * Simple logger implementation.
 *
 * Logger() for a logger that print to stdout
 * Logger(filename) for a logger that print to specific file
 * Logger(nullptr) for a disabled logger (for minimized code modification to disable message output)
 */
class Logger {
public:
    Logger() {
        enabled = true;
        fout = stdout;
    }

    Logger(const char* filename) {
        if (filename == nullptr) {
            enabled = false;
            fout = nullptr;
        } else {
            enabled = true;
            fout = fopen(filename, "w");

            if (fout == nullptr) {
                fprintf(stderr, "Error: %s: %s\n", filename, strerror(errno));
                enabled = false;
            }
        }
    }

    Logger(const Logger& that) = delete;

    Logger(Logger&& that) {
        fout = that.fout;
        enabled = that.enabled;
        that.fout = nullptr;
    }

    ~Logger() {
        if (fout != nullptr && fout != stdout) {
            fclose(fout);
        }
    }

    Logger& operator=(const Logger& that) = delete;

    Logger& operator=(Logger&& that) {
        if (this != &that) {
            fout = that.fout;
            enabled = that.enabled;
            that.fout = nullptr;
        }
        return *this;
    }

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

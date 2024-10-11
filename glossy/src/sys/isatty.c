#include <unistd.h>

__attribute__((weak)) int _isatty(int file) {
    return (file == STDOUT_FILENO || file == STDERR_FILENO);
}

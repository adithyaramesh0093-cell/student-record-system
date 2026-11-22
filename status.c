#include "status.h"

const char *status_message(Status status)
{
    switch (status) {
    case STATUS_OK:             return "Success";
    case STATUS_INVALID_INPUT:  return "Invalid input";
    case STATUS_DUPLICATE_ROLL: return "That roll number is already in use";
    case STATUS_NOT_FOUND:      return "Record not found";
    case STATUS_OUT_OF_MEMORY:  return "Out of memory";
    case STATUS_IO_ERROR:       return "File error";
    case STATUS_BAD_FORMAT:     return "The file is damaged or not a student file";
    }
    return "Unknown error";
}

/*
 * status.h - the single "result code" type used by the backend modules.
 *
 * Instead of printing errors from deep inside the code, backend functions
 * return a Status. The stud_*.c files decide how to show it to the user.
 */
#ifndef STATUS_H
#define STATUS_H

typedef enum {
    STATUS_OK = 0,
    STATUS_INVALID_INPUT,   /* a value failed validation                    */
    STATUS_DUPLICATE_ROLL,  /* a record with this roll number already exists */
    STATUS_NOT_FOUND,       /* no record with this roll number              */
    STATUS_OUT_OF_MEMORY,   /* malloc/calloc failed                         */
    STATUS_IO_ERROR,        /* file could not be read or written            */
    STATUS_BAD_FORMAT       /* student.dat exists but its contents are bad  */
} Status;

/* Human-readable text for a status code (never returns NULL). */
const char *status_message(Status status);

#endif /* STATUS_H */

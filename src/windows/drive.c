#include "windows/drive.h"

#include "common.h"

typedef enum
{
    WINDOWS_DRIVE_VOLUME,
    WINDOWS_DRIVE_UNC,
    WINDOWS_DRIVE_DEVICE_LITERAL,
    WINDOWS_DRIVE_DEVICE_NORMALIZED,
    WINDOWS_DRIVE_DEVICE_UNC,
} WindowsDriveKind;

// https://www.fileside.app/blog/2023-03-17_windows-file-paths/
typedef enum
{
    WINDOWS_DRIVE_STATE_START,
    WINDOWS_DRIVE_STATE_START_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT_SLASH,
    WINDOWS_DRIVE_STATE_START_OTHER,
    WINDOWS_DRIVE_STATE_UNC_SERVER,
    WINDOWS_DRIVE_STATE_UNC_SHARE,
    WINDOWS_DRIVE_STATE_DEVICE_LITERAL,
    WINDOWS_DRIVE_STATE_DEVICE_LITERAL_U,
    WINDOWS_DRIVE_STATE_DEVICE_LITERAL_UN,
    WINDOWS_DRIVE_STATE_DEVICE_LITERAL_UNC,
    WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED,
} WindowsDriveState;

typedef struct
{
    Py_ssize_t index;
    WindowsDriveKind kind;
} WindowsDrive;

WindowsDrive
_windows_drive_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data)
{
    WindowsDrive drive;
    WindowsDriveState state;
    for (drive.index = 0; drive.index < arg_size; ++drive.index)
    {
        Py_UCS4 character = PyUnicode_READ(arg_kind, arg_data, drive.index);
        switch (state)
        {
        case WINDOWS_DRIVE_STATE_START:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_START_SLASH;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_START_OTHER;
                break;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH;
                break;
            default:
                goto invalid;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_UNC_SHARE;
                break;
            case '?':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION;
                break;
            case '.':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_UNC_SHARE;
                break;
            case 'u':
            case 'U':
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL_U;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_UNC_SHARE;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_OTHER:
            switch (character)
            {
            case ':':
                drive.index += 1;
                drive.kind = WINDOWS_DRIVE_VOLUME;
                goto done;
            default:
                goto invalid;
            }
            break;
        case WINDOWS_DRIVE_STATE_UNC_SERVER:
            break;
        case WINDOWS_DRIVE_STATE_UNC_SHARE:
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_LITERAL:
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_LITERAL_U:
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_LITERAL_UN:
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_LITERAL_UNC:
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED:
            break;
        }
    }

invalid:
    drive.index = 0;
    drive.kind = 0;

done:
    return drive;
}

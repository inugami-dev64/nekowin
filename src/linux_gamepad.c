/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: linux_gamepad.h - linux gamepad input management source code
/// author: Karl-Mihkel Ott

#define LINUX_GAMEPAD_C
#include "gamepad.h"

#define NUM_DEVICES 32
#define NAME_MAX 1024
#define NUM_EVENTS 8
#define ISBITSET(bit, bytes) ((bytes[(bit) / 8]) & (1 << (bit % 8)))

// anonymous struct array containing information about controller inputs and their availability
static struct {
    bool keytbl[KEY_CNT - BTN_MISC];
    bool abstbl[ABS_CNT - ABS_X];
    uint32_t keyc;
    uint32_t axisc;
    uint32_t hatc;
    bool complete;      // does the device have every event bit required for complete controller mapping
} ctrltbls[NUM_DEVICES] = { 0 };


// callibartion values
// NOTE: defaulted to Xbox one S controller
static struct {
    struct {
        ClbRange x;
        ClbRange y;
    } leftjs;

    struct {
        ClbRange x;
        ClbRange y;
    } rightjs;

    ClbRange lt;
    ClbRange rt;
} clbvars[NUM_DEVICES] = { 0 };


// NOTE: according to wikipedia file descriptor 0 means STDIN_FILENO
// Here it is used as a termination number for iterating through device_fds
static int device_fds[NUM_DEVICES] = { 0 };


static void _neko_CheckIfCompleteConfig(uint32_t _id) {
    // complete configuration should contain at least following axis event bits:
    //   Left joystick: ABS_X and ABS_Y
    //   Right joystick: ABS_RX and ABS_RY
    //   Left trigger: ABS_Z
    //   Right trigger: ABS_RZ
    //   DPAD left and right: ABS_HAT0X
    //   DPAD up and down: ABS_HAT0Y
    // That is 6 axes and 2 hats in total 
    const bool abscfg = ctrltbls[_id].hatc < 2 || ctrltbls[_id].axisc < 6 ||
                        !ISBITSET(ABS_X, ctrltbls[_id].abstbl) || !ISBITSET(ABS_Y, ctrltbls[_id].abstbl) || 
                        !ISBITSET(ABS_RX, ctrltbls[_id].abstbl) || !ISBITSET(ABS_RY, ctrltbls[_id].abstbl) ||
                        !ISBITSET(ABS_Z, ctrltbls[_id].abstbl) || !ISBITSET(ABS_RZ, ctrltbls[_id].abstbl) ||
                        !ISBITSET(ABS_HAT0X, ctrltbls[_id].abstbl) || !ISBITSET(ABS_HAT0Y, ctrltbls[_id].abstbl);

    // complete configuration should contain at least following key / btn event bits
    //   Button A: BTN_A
    //   Button B: BTN_B
    //   Button X: BTN_X
    //   Button Y: BTN_Y
    //   Left bumper: BTN_TL
    //   Right bumper: BTN_TR
    //   Back button: BTN_SELECT
    //   Start butto: BTN_START
    //   Left stick: BTN_THUMBL
    //   Right stick: BTN_THUMBR
    // That is 10 key events in total
    const bool keycfg = ctrltbls[_id].keyc < 10 ||
                        !ISBITSET(BTN_A, ctrltbls[_id].keytbl) || !ISBITSET(BTN_B, ctrltbls[_id].keytbl) ||
                        !ISBITSET(BTN_X, ctrltbls[_id].keytbl) || !ISBITSET(BTN_Y, ctrltbls[_id].keytbl) ||
                        !ISBITSET(BTN_TL, ctrltbls[_id].keytbl) || !ISBITSET(BTN_TR, ctrltbls[_id].keytbl) ||
                        !ISBITSET(BTN_SELECT, ctrltbls[_id].keytbl) || !ISBITSET(BTN_START, ctrltbls[_id].keytbl) ||
                        !ISBITSET(BTN_THUMBL, ctrltbls[_id].keytbl) || !ISBITSET(BTN_THUMBR, ctrltbls[_id].keytbl);
    

    if(abscfg && keycfg) {
        ctrltbls[_id].complete = true;
    } else {
        ctrltbls[_id].complete = false;
    }
}


// default callibarition values are taken from Xbox One S controller
static void _neko_InitDefaultCallibration(uint32_t _id) {
    clbvars[_id].leftjs.x.min = INT16_MIN;
    clbvars[_id].leftjs.x.max = INT16_MAX;
    clbvars[_id].leftjs.y.min = INT16_MIN;
    clbvars[_id].leftjs.y.max = INT16_MAX;
    clbvars[_id].rightjs.x.min = INT16_MIN;
    clbvars[_id].rightjs.x.max = INT16_MAX;
    clbvars[_id].rightjs.y.min = INT16_MIN;
    clbvars[_id].rightjs.y.max = INT16_MAX;
    clbvars[_id].lt.min = 0;
    clbvars[_id].lt.max = 1024;
    clbvars[_id].rt.min = 0;
    clbvars[_id].rt.max = 1024;
}


static void _neko_HandleAxisEvent(neko_Gamepad *_gamepad, struct input_event *_e, uint32_t _id) {
    switch(_e->code) {
        case ABS_X:
            {
                const float avg = (float)(clbvars[_id].leftjs.x.min + clbvars[_id].leftjs.x.max) / 2.0f;
                _gamepad->left_thumb.x = (int16_t) ((float)_e->value * INT16_MAX / ((float)clbvars[_id].leftjs.x.max - avg));
            }
            break;

        case ABS_Y:
            {
                const float avg = (float)(clbvars[_id].leftjs.y.min + clbvars[_id].leftjs.y.max) / 2.0f;
                _gamepad->left_thumb.y = (int16_t) ((float) _e->value * INT16_MAX / ((float)clbvars[_id].leftjs.y.max - avg));
            }
            break;

        case ABS_RX:
            {
                const float avg = (float)(clbvars[_id].rightjs.x.min + clbvars[_id].rightjs.x.max) / 2.0f;
                _gamepad->right_thumb.x = (int16_t) ((float) _e->value * INT16_MAX / ((float)clbvars[_id].rightjs.x.max - avg));
            }
            break;
        
        case ABS_RY:
            {
                const float avg = (float)(clbvars[_id].rightjs.y.min + clbvars[_id].rightjs.y.max) / 2.0f;
                _gamepad->right_thumb.y = (int16_t) ((float) _e->value * INT16_MAX / ((float)clbvars[_id].rightjs.y.max - avg));
            }
            break;

        case ABS_Z:
            {
                const float avg = (float)(clbvars[_id].lt.min + clbvars[_id].lt.max) / 2.0f;
                _gamepad->left_trigger = (uint8_t) ((float) _e->value * INT8_MAX / ((float)clbvars[_id].lt.max - avg));
            }
            break;

        case ABS_RZ:
            {
                const float avg = (float)(clbvars[_id].rt.min + clbvars[_id].rt.max) / 2.0f;
                _gamepad->right_trigger = (uint8_t) ((float) _e->value * INT8_MAX / ((float)clbvars[_id].rt.max - avg));
            }
            break;

        case ABS_HAT0X:
            if(_e->value < 0) {
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_LEFT;
            } else if(_e->value > 0) {
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_RIGHT;
            }
            break;

        case ABS_HAT0Y:
            if(_e->value < 0) {
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_UP;
            } else if(_e->value > 0) {
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_DOWN;
            }
            break;

        default:
            break;
    }
}


static void _neko_HandleKeyEvent(neko_Gamepad *_gamepad, struct input_event *_e) {
    if(_e->value) {
        switch(_e->code) {
            case BTN_A:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_A;
                break;

            case BTN_B:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_B;
                break;

            case BTN_X:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_X;
                break;

            case BTN_Y:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_Y;
                break;

            case BTN_TL:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_LEFT_BUMPER;
                break;

            case BTN_TR:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_RIGHT_BUMPER;
                break;

            case BTN_SELECT:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_BACK;
                break;

            case BTN_START:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_START;
                break;

            case BTN_THUMBL:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_LEFT_STICK;
                break;

            case BTN_THUMBR:
                _gamepad->buttons |= NEKO_GAMEPAD_BTN_RIGHT_STICK;
                break;

            default:
                break;
        }
    }
}


static bool _neko_OpenGamepadDevice(const char *_path, uint32_t _id) {
    // attempt to open an input device, if it fails then return false
    int fd = open(_path, O_RDONLY | O_NONBLOCK);
    if(fd < 0) {
        return false;
    }

    // zero initialise current ctrltbls element
    memset(&ctrltbls[_id], 0, sizeof(ctrltbls[_id]));

    // query information about device capabilities
    // NOTE: it might be necessary in the future to have sound event (EV_SND) query made in order to check if the device supports sound output
    char ev_bits[(EV_CNT + 7) / 8] = { 0 };
    char key_bits[(KEY_CNT + 7) / 8] = { 0 };
    char abs_bits[(ABS_CNT + 7) / 8] = { 0 };
    if(ioctl(fd, EVIOCGBIT(EV_SYN, sizeof(ev_bits)), ev_bits) < 0 ||
       ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(key_bits)), key_bits) < 0 ||
       ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_bits)), abs_bits) < 0) 
    {
        close(fd);
        return false;
    }

    // check if the input device supports events that would be expected from a gamepad interface
    if(!ISBITSET(EV_KEY, ev_bits) || !ISBITSET(EV_ABS, ev_bits)) {
        close(fd);
        return false;
    }

    // check how many buttons are present
    for(int cde = BTN_MISC; cde < KEY_CNT; cde++) {
        if(!ISBITSET(cde, key_bits))
            continue;

        ctrltbls[_id].keytbl[cde - BTN_MISC] = true;
        ctrltbls[_id].keyc++;
    }

    // check how many axes (or hats) are present on the controller device
    for(int cde = ABS_X; cde < ABS_CNT; cde++) {
        if(!ISBITSET(cde, abs_bits))
            continue;
        
        // check if the current ABS code is representing hat codes (DPAD for Xbox-like controllers)
        ctrltbls[_id].abstbl[cde - ABS_X] = true;
        if(cde >= ABS_HAT0X && cde <= ABS_HAT3Y) {
            ctrltbls[_id].hatc++;
        } else {
            ctrltbls[_id].axisc++;
        }
    }

    _neko_CheckIfCompleteConfig(_id);
    _neko_InitDefaultCallibration(_id);
    device_fds[_id] = fd;
    return true;
}   


uint32_t neko_GetConnectedControllerCount() {
    uint32_t count = 0;

    DIR *d = opendir("/dev/input");
    struct dirent *dir;
    if(d) {
        while((dir = readdir(d)) != NULL) {
            char pth[NAME_MAX] = { 0 };
            sprintf(pth, "/dev/input/%s", dir->d_name);
            if(!strncmp("event", dir->d_name, 5) && _neko_OpenGamepadDevice(pth, count)) {
                count++;
            }
        }
        closedir(d);
    }

    return count;
}


const char *neko_GetControllerName(uint32_t _id) {
    static char buf[NAME_MAX + 1] = { 0 };
    if(ioctl(device_fds[_id], EVIOCGNAME(sizeof(buf)), buf) < 0) {
        perror("invalid controller id");
    }

    return buf;
}


void neko_UpdateController(uint32_t _id, neko_Gamepad *_gamepad) {
    struct input_event e = { 0 };
    _gamepad->buttons = 0;

    // NOTE: no error handling
    while(read(device_fds[_id], &e, sizeof(e)) == sizeof(e)) {
        switch(e.type) {
            case EV_ABS:
                _neko_HandleAxisEvent(_gamepad, &e, _id);
                break;

            case EV_KEY:
                _neko_HandleKeyEvent(_gamepad, &e);
                break;
        }
    }

    // error device not found
    if(errno == ENODEV) {
        device_fds[_id] = 0;
    }
}


bool neko_CheckGamepadDisconnect(uint32_t _id) {
    if(!device_fds[_id]) {
        memset(&ctrltbls[_id], 0, sizeof(ctrltbls[_id]));
        memset(&clbvars[_id], 0, sizeof(clbvars[_id]));

        if(_id != NUM_DEVICES - 1) {
            memmove(device_fds + _id, device_fds + _id + 1, (NUM_DEVICES - _id - 1) * sizeof(int));
            memmove(ctrltbls + _id, ctrltbls + _id + 1, (NUM_DEVICES - _id - 1) * sizeof(ctrltbls[0]));
            memmove(clbvars + _id, clbvars + _id + 1, (NUM_DEVICES - _id - 1) * sizeof(clbvars[0]));
        }
        return true;
    }

    return false;
}

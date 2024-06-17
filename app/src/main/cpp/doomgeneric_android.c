#include <time.h>

#include "AndroidRenderer.h"

#include "doomkeys.h"
#include "doomgeneric.h"
#include "doomstat.h"

#define KEYQUEUE_SIZE 16

static int screen_x, screen_y;

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static bool pointer_touched_in(int x, int y, int x2, int y2, int *id)
{
    bool touched = false;
    for (int i = 0; i < 8; ++i)
    {
        touched = (x < button_x[i] && button_x[i] < x2) && (y < button_y[i] && button_y[i] < y2);
        *id = i;
        if (touched) break;
    }
    return touched;
}

static void addKeyToQueue(int pressed, unsigned char key)
{
    unsigned short keyData = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

static void VirtualButton(int x, int y, int id, unsigned char keycode)
{
    static bool pressed[2] = { false, false };
    int lw = x + 200;
    int lh = y + 200;

    if (pressed[id])
        RenderCircle(x, y, 100, 0x4c4c4cff);
    else
        RenderCircle(x, y, 100, 0x808080ff);

    int idx;
    if (pointer_touched_in(x, y, lw, lh, &idx) && !pressed[id])
    {
        addKeyToQueue(1, keycode);
        pressed[id] = true;
    }
    else if (!pointer_touched_in(x, y, lw, lh, &idx) && pressed[id])
    {
        addKeyToQueue(0, keycode);
        pressed[id] = false;
    }
}

static void VirtualJoystick(void)
{
    // make center of joystick do nothing
    static bool forward = false;
    static bool backward = false;
    static bool left = false;
    static bool right = false;

// Define positions for the joysticks
    int leftJoystickX = 250; // Example: manually set the X coordinate for the left joystick
    int leftJoystickY = 300; // Example: manually set the Y coordinate for the left joystick

    int rightJoystickX = 200; // Example: manually set the X coordinate for the right joystick
    int rightJoystickY = 200; // Example: manually set the Y coordinate for the right joystick

    // Render the left joystick
    RenderCircle(leftJoystickX, leftJoystickY, 80, 0x4c4c4cff);
    int id;
    if (pointer_touched_in(leftJoystickX, leftJoystickY, leftJoystickX + 200, leftJoystickY + 200, &id))
    {
        RenderCircle(motion_x[id] - 80, motion_y[id] - 120, 80, 0x808080ff);

        // Handle forward movement for left joystick
        if (motion_y[id] < leftJoystickY - 50)
        {
            if (!forward)
            {
                addKeyToQueue(1, KEY_UPARROW);
                forward = true;
            }
        }
        else if (forward)
        {
            addKeyToQueue(0, KEY_UPARROW);
            forward = false;
        }

        // Handle backward movement for left joystick
        if (motion_y[id] > leftJoystickY + 50)
        {
            if (!backward)
            {
                addKeyToQueue(1, KEY_DOWNARROW);
                backward = true;
            }
        }
        else if (backward)
        {
            addKeyToQueue(0, KEY_DOWNARROW);
            backward = false;
        }

        // Handle left movement for left joystick
        if (motion_x[id] < leftJoystickX - 50)
        {
            if (!left)
            {
                addKeyToQueue(1, KEY_LEFTARROW);
                left = true;
            }
        }
        else if (left)
        {
            addKeyToQueue(0, KEY_LEFTARROW);
            left = false;
        }

        // Handle right movement for left joystick
        if (motion_x[id] > leftJoystickX + 50)
        {
            if (!right)
            {
                addKeyToQueue(1, KEY_RIGHTARROW);
                right = true;
            }
        }
        else if (right)
        {
            addKeyToQueue(0, KEY_RIGHTARROW);
            right = false;
        }
    }
    else
    {
        RenderCircle(leftJoystickX, leftJoystickY, 80, 0x808080ff); // Non-active state

        // Reset all keys if not touching the left joystick
        if (forward)
        {
            addKeyToQueue(0, KEY_UPARROW);
            forward = false;
        }
        if (backward)
        {
            addKeyToQueue(0, KEY_DOWNARROW);
            backward = false;
        }
        if (right)
        {
            addKeyToQueue(0, KEY_RIGHTARROW);
            right = false;
        }
        if (left)
        {
            addKeyToQueue(0, KEY_LEFTARROW);
            left = false;
        }
    }

    // Render the right joystick
    RenderCircle(rightJoystickX, rightJoystickY, 100, 0x4c4c4cff);
    // Assuming similar touch handling and reset code for the right joystick
    // ...
}

void DG_Init(void)
{
    AndroidMakeFullscreen();
    SetupApplication();
    GetScreenDimensions(&screen_x, &screen_y);
    DOOMGENERIC_RESY = screen_y;
    DOOMGENERIC_RESX = DOOMGENERIC_RESY / 10 * 16;
}

void DG_DrawFrame(void)
{
    ClearFrame();
    RenderImage(DG_ScreenBuffer, screen_x / 2 - DOOMGENERIC_RESX / 2,
                screen_y / 2 - DOOMGENERIC_RESY / 2, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    VirtualJoystick();

    if (menuactive)
        VirtualButton(screen_x - 400, screen_y - 300, 0, KEY_ENTER);
    else
        VirtualButton(screen_x - 400, screen_y - 300, 0, KEY_FIRE);
    VirtualButton(screen_x - 250, screen_y - 500, 1, KEY_USE);

    HandleInput();
    SwapBuffers();
}

void DG_SleepMs(uint32_t ms)
{
    struct timespec req = {
            .tv_sec = 0,
            .tv_nsec = (long)ms * 1000000
    };
    nanosleep(&req, NULL);
}

uint32_t DG_GetTicksMs(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000); /* return milliseconds */
}

int DG_GetKey(int *pressed, unsigned char *doomKey)
{
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
        return 0; //key queue is empty

    unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xFF;

    return 1;
}

void DG_SetWindowTitle(const char *title)
{
    (void)title;
}

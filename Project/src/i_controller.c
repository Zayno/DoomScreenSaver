

#include "c_console.h"
#include "doomstat.h"
#include "i_controller.h"
#include "m_config.h"
#include "m_misc.h"

static SDL_GameController   *controller;
static bool                 controllerrumbles;

int                         controllerbuttons = 0;
short                       controllerthumbLX = 0;
short                       controllerthumbLY = 0;
short                       controllerthumbRX = 0;
short                       controllerthumbRY = 0;
float                       controllerhorizontalsensitivity;
float                       controllerverticalsensitivity;
short                       controllerleftdeadzone;
short                       controllerrightdeadzone;

int                         barrelrumbletics = 0;
int                         damagerumbletics = 0;
int                         pickuprumbletics = 0;
int                         weaponrumbletics = 0;
int                         idlechainsawrumblestrength;
int                         restoredrumblestrength;

char                        *selectbutton = "A";

static char *GetControllerName(void)
{
    const char  *name = SDL_GameControllerName(controller);

    if (name)
        return M_StringJoin("A controller called \"", name, "\" is connected.", NULL);
    else
        return "A controller is connected.";
}

#if SDL_VERSION_ATLEAST(2, 12, 0)
static char *GetControllerType(void)
{
    SDL_GameControllerType  type = SDL_GameControllerGetType(controller);

    if (type == SDL_CONTROLLER_TYPE_XBOX360)
        return "An " ITALICS("Xbox 360") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_XBOXONE)
        return "An " ITALICS("Xbox One") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_PS3)
    {
        selectbutton = "X";
        return "A " ITALICS("PlayStation 3 DualShock") " controller is connected.";
    }
    else if (type == SDL_CONTROLLER_TYPE_PS4)
    {
        selectbutton = "X";
        return "A " ITALICS("PlayStation 4 DualShock") " controller is connected.";
    }
    else if (type == SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO)
        return "A " ITALICS("Nintendo Switch Pro") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_VIRTUAL)
        return "A virtual controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_PS5)
    {
        selectbutton = "X";
        return "A " ITALICS("PlayStation 5 DualSense") " controller is connected.";
    }
    else if (type == SDL_CONTROLLER_TYPE_AMAZON_LUNA)
        return "An " ITALICS("Amazon Luna") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_GOOGLE_STADIA)
        return "A " ITALICS("Google Stadia") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_NVIDIA_SHIELD)
        return "An " ITALICS("Nvidia Shield") " controller is connected.";
    else if (type == SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT)
        return "A " ITALICS("Nintendo Switch's") " left joycon is connected.";
    else if (type == SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT)
        return "A " ITALICS("Nintendo Switch's") " right joycon is connected.";
    else if (type == SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR)
        return "A " ITALICS("Nintendo Switch's") " left and right joycons are connected.";
    else
        return GetControllerName();
}
#endif

void I_InitController(void)
{
    if (controller)
        return;

#if defined(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE)
    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1", SDL_HINT_OVERRIDE);
#endif

#if defined(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE)
    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1", SDL_HINT_OVERRIDE);
#endif

    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1", SDL_HINT_OVERRIDE);

    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_RAWINPUT, "0", SDL_HINT_OVERRIDE);

    SDL_EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYHATMOTION, SDL_IGNORE);
    SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);

#if SDL_VERSION_ATLEAST(2, 24, 0)
    SDL_EventState(SDL_JOYBATTERYUPDATED, SDL_IGNORE);
#endif

    SDL_EventState(SDL_CONTROLLERAXISMOTION, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERBUTTONUP, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERDEVICEREMAPPED, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERTOUCHPADDOWN, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERTOUCHPADMOTION, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERTOUCHPADUP, SDL_IGNORE);
    SDL_EventState(SDL_CONTROLLERSENSORUPDATE, SDL_IGNORE);

    for (int i = 0, numjoysticks = SDL_NumJoysticks(); i < numjoysticks; i++)
        if (SDL_IsGameController(i) && (controller = SDL_GameControllerOpen(i)))
        {
#if SDL_VERSION_ATLEAST(2, 12, 0)
            C_Output(GetControllerType());
#else
            C_Output(GetControllerName());
#endif

#if SDL_VERSION_ATLEAST(2, 18, 0)
            if (SDL_GameControllerHasRumble(controller))
                controllerrumbles = true;
            else
#endif
                if (joy_rumble_barrels || joy_rumble_damage || joy_rumble_fall || joy_rumble_pickup || joy_rumble_weapons)
                    C_Warning(1, "This controller doesn't rumble!");

            I_SetControllerLeftDeadZone();
            I_SetControllerRightDeadZone();
            I_SetControllerHorizontalSensitivity();
            I_SetControllerVerticalSensitivity();

#if SDL_VERSION_ATLEAST(2, 14, 0)
            SDL_GameControllerSetLED(controller, 255, 0, 0);
#endif

            return;
        }
}

void I_ShutdownController(void)
{
    if (!controller)
        return;

    C_Warning(1, "The controller was disconnected!");

#if SDL_VERSION_ATLEAST(2, 14, 0)
    SDL_GameControllerSetLED(controller, 0, 0, 255);
#endif

    SDL_GameControllerClose(controller);
    controller = NULL;
}

void I_ControllerRumble(const short low, const short high)
{
    if (!controllerrumbles || !usingcontroller)
        return;

    SDL_GameControllerRumble(controller, MIN(low, USHRT_MAX), MIN(high, USHRT_MAX), UINT_MAX);
}


void I_StopControllerRumble(void)
{
    if (!controllerrumbles)
        return;

    SDL_GameControllerRumble(controller, 0, 0, 0);
}

void I_SetControllerHorizontalSensitivity(void)
{
    controllerhorizontalsensitivity = 2.0f * joy_sensitivity_horizontal / joy_sensitivity_horizontal_max;
}

void I_SetControllerVerticalSensitivity(void)
{
    controllerverticalsensitivity = 2.0f * joy_sensitivity_vertical / joy_sensitivity_vertical_max;
}

void I_SetControllerLeftDeadZone(void)
{
    controllerleftdeadzone = (short)(joy_deadzone_left * SDL_JOYSTICK_AXIS_MAX / 100.0f);
}

void I_SetControllerRightDeadZone(void)
{
    controllerrightdeadzone = (short)(joy_deadzone_right * SDL_JOYSTICK_AXIS_MAX / 100.0f);
}

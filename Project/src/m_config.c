

#include <string.h>

#include "c_cmds.h"
#include "c_console.h"
#include "d_iwad.h"
#include "d_main.h"
#include "doomstat.h"
#include "g_game.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "z_zone.h"
#include "version.h"

bool        alwaysrun = alwaysrun_default;
int         am_allmapcdwallcolor = am_allmapcdwallcolor_default;
int         am_allmapfdwallcolor = am_allmapfdwallcolor_default;
int         am_allmapwallcolor = am_allmapwallcolor_default;
bool        am_antialiasing = am_antialiasing_default;
bool        am_author = am_author_default;
int         am_backcolor = am_backcolor_default;
int         am_bloodsplatcolor = am_bloodsplatcolor_default;
int         am_bluedoorcolor = am_bluedoorcolor_default;
int         am_bluekeycolor = am_bluekeycolor_default;
int         am_cdwallcolor = am_cdwallcolor_default;
int         am_corpsecolor = am_corpsecolor_default;
bool        am_correctaspectratio = am_correctaspectratio_default;
int         am_crosshaircolor = am_crosshaircolor_default;
int         am_display = am_display_default;
bool        am_dynamic = am_dynamic_default;
bool        am_external = am_external_default;
int         am_fdwallcolor = am_fdwallcolor_default;
bool        am_followmode = am_followmode_default;
bool        am_grid = am_grid_default;
int         am_gridcolor = am_gridcolor_default;
char        *am_gridsize = am_gridsize_default;
int         am_markcolor = am_markcolor_default;
bool        am_mousepanning = am_mousepanning_default;
bool        am_path = am_path_default;
int         am_pathcolor = am_pathcolor_default;
int         am_pathlength = am_pathlength_default;
int         am_playercolor = am_playercolor_default;
bool        am_playerstats = am_playerstats_default;
int         am_playerstatscolor = am_playerstatscolor_default;
int         am_reddoorcolor = am_reddoorcolor_default;
int         am_redkeycolor = am_redkeycolor_default;
bool        am_rotatemode = am_rotatemode_default;
int         am_teleportercolor = am_teleportercolor_default;
int         am_thingcolor = am_thingcolor_default;
int         am_tswallcolor = am_tswallcolor_default;
int         am_wallcolor = am_wallcolor_default;
int         am_yellowdoorcolor = am_yellowdoorcolor_default;
int         am_yellowkeycolor = am_yellowkeycolor_default;
bool        animatedstats = animatedstats_default;
bool        autoaim = autoaim_default;
bool        autoload = autoload_default;
bool        autosave = autosave_default;
bool        autoswitch = autoswitch_default;
bool        autotilt = autotilt_default;
bool        autouse = autouse_default;
bool        centerweapon = centerweapon_default;
int         con_edgecolor = con_edgecolor_default;
int         con_timestampformat = con_timestampformat_default;
bool        con_timestamps = con_timestamps_default;
int         con_warninglevel = con_warninglevel_default;
int         crosshair = crosshair_default;
int         crosshaircolor = crosshaircolor_default;
int         english = english_default;
int         episode = episode_default;
int         expansion = expansion_default;
int         facebackcolor = facebackcolor_default;
bool        fade = fade_default;
bool        flashkeys = flashkeys_default;
bool        freelook = freelook_default;
bool        groupmessages = groupmessages_default;
bool        infighting = infighting_default;
bool        infiniteheight = infiniteheight_default;
char        *wadfolder = wadfolder_default;
bool        joy_analog = joy_analog_default;
float       joy_deadzone_left = joy_deadzone_left_default;
float       joy_deadzone_right = joy_deadzone_right_default;
bool        joy_invertyaxis = joy_invertyaxis_default;
int         joy_rumble_barrels = joy_rumble_barrels_default;
int         joy_rumble_damage = joy_rumble_damage_default;
bool        joy_rumble_fall = joy_rumble_fall_default;
bool        joy_rumble_pickup = joy_rumble_pickup_default;
int         joy_rumble_weapons = joy_rumble_weapons_default;
float       joy_sensitivity_horizontal = joy_sensitivity_horizontal_default;
float       joy_sensitivity_vertical = joy_sensitivity_vertical_default;
bool        joy_swapthumbsticks = joy_swapthumbsticks_default;
int         joy_thumbsticks = joy_thumbsticks_default;
bool        m_acceleration = m_acceleration_default;
bool        m_invertyaxis = m_invertyaxis_default;
bool        m_novertical = m_novertical_default;
bool        m_pointer = m_pointer_default;
float       m_sensitivity = m_sensitivity_default;
bool        melt = melt_default;
bool        menuhighlight = menuhighlight_default;
bool        menushadow = menushadow_default;
bool        menuspin = menuspin_default;
bool        messages = messages_default;
int         movebob = movebob_default;
bool        negativehealth = negativehealth_default;
bool        obituaries = obituaries_default;
int         playergender = playergender_default;
char        *playername = playername_default;
bool        r_althudfont = r_althudfont_default;
bool        r_antialiasing = r_antialiasing_default;
int         r_berserkeffect = r_berserkeffect_default;
int         r_blood = r_blood_default;
bool        r_blood_gibs = r_blood_gibs_default;
bool        r_blood_melee = r_blood_melee_default;
int         r_bloodsplats_max = r_bloodsplats_max_default;
int         r_bloodsplats_total;
bool        r_bloodsplats_translucency = r_bloodsplats_translucency_default;
bool        r_brightmaps = r_brightmaps_default;
bool        r_corpses_color = r_corpses_color_default;
bool        r_corpses_gib = r_corpses_gib_default;
bool        r_corpses_mirrored = r_corpses_mirrored_default;
bool        r_corpses_moreblood = r_corpses_moreblood_default;
bool        r_corpses_nudge = r_corpses_nudge_default;
bool        r_corpses_slide = r_corpses_slide_default;
bool        r_corpses_smearblood = r_corpses_smearblood_default;
bool        r_damageeffect = r_damageeffect_default;
int         r_detail = r_detail_default;
bool        r_diskicon = r_diskicon_default;
bool        r_ditheredlighting = r_ditheredlighting_default;
int         r_extralighting = r_extralighting_default;
bool        r_fixmaperrors = r_fixmaperrors_default;
bool        r_fixspriteoffsets = r_fixspriteoffsets_default;
bool        r_floatbob = r_floatbob_default;
int         r_fov = r_fov_default;
float       r_gamma = r_gamma_default;
bool        r_graduallighting = r_graduallighting_default;
bool        r_homindicator = r_homindicator_default;
bool        r_hud = r_hud_default;
bool        r_hud_translucency = r_hud_translucency_default;
bool        r_linearskies = r_linearskies_default;
bool        r_liquid_bob = r_liquid_bob_default;
bool        r_liquid_bobsprites = r_liquid_bobsprites_default;
bool        r_liquid_clipsprites = r_liquid_clipsprites_default;
bool        r_liquid_current = r_liquid_current_default;
bool        r_liquid_lowerview = r_liquid_lowerview_default;
bool        r_liquid_swirl = r_liquid_swirl_default;
char        *r_lowpixelsize = r_lowpixelsize_default;
bool        r_mirroredweapons = r_mirroredweapons_default;
bool        r_percolumnlighting = r_percolumnlighting_default;
bool        r_pickupeffect = r_pickupeffect_default;
bool        r_playersprites = r_playersprites_default;
bool        r_radsuiteffect = r_radsuiteffect_default;
bool        r_randomstartframes = r_randomstartframes_default;
bool        r_rockettrails = r_rockettrails_default;
bool        r_rockettrails_translucency = r_rockettrails_translucency_default;
int         r_screensize = r_screensize_default;
bool        r_shadows = r_shadows_default;
bool        r_shadows_translucency = r_shadows_translucency_default;
bool        r_shake_barrels = r_shake_barrels_default;
bool        r_shake_berserk = r_shake_berserk_default;
bool        r_shake_damage = r_shake_damage_default;
bool        r_sprites_translucency = r_sprites_translucency_default;
bool        r_textures = r_textures_default;
bool        r_textures_translucency = r_textures_translucency_default;
int         s_channels = s_channels_default;
bool        s_fullsfx = s_fullsfx_default;
bool        s_lowermenumusic = s_lowermenumusic_default;
bool        s_musicinbackground = s_musicinbackground_default;
int         s_musicvolume = s_musicvolume_default;
bool        s_randommusic = s_randommusic_default;
bool        s_randompitch = s_randompitch_default;
int         s_sfxvolume = s_sfxvolume_default;
bool        s_stereo = s_stereo_default;
int         savegame = savegame_default;
bool        secretmessages = secretmessages_default;
int         skilllevel = skilllevel_default;
int         stillbob = stillbob_default;
int         sucktime = sucktime_default;
bool        tossdrop = tossdrop_default;
int         turbo = turbo_default;
int         units = units_default;
char        *version = version_default;
int         vid_aspectratio = vid_aspectratio_default;
int         vid_blue = vid_blue_default;
bool        vid_borderlesswindow = vid_borderlesswindow_default;
int         vid_brightness = vid_brightness_default;
int         vid_contrast = vid_contrast_default;
int         vid_display = vid_display_default;
bool        vid_fullscreen = vid_fullscreen_default;
int         vid_green = vid_green_default;
int         vid_motionblur = vid_motionblur_default;
bool        vid_pillarboxes = vid_pillarboxes_default;
int         vid_red = vid_red_default;
int         vid_saturation = vid_saturation_default;
char        *vid_scaleapi = vid_scaleapi_default;
char        *vid_scalefilter = vid_scalefilter_default;
char        *vid_screenresolution = vid_screenresolution_default;
int         vid_vsync = vid_vsync_default;
bool        vid_widescreen = vid_widescreen_default;
char        *vid_windowpos = vid_windowpos_default;
char        *vid_windowsize = vid_windowsize_default;
char        *wad = wad_default;
int         weaponbob = weaponbob_default;
bool        weaponbounce = weaponbounce_default;
bool        weaponrecoil = weaponrecoil_default;

uint64_t    stat_automapopened = 0;
uint64_t    stat_barrelsexploded = 0;
uint64_t    stat_cheatsentered = 0;
uint64_t    stat_damageinflicted = 0;
uint64_t    stat_damagereceived = 0;
uint64_t    stat_deaths = 0;
uint64_t    stat_distancetraveled = 0;
uint64_t    stat_firstrun = 0;
uint64_t    stat_gamesloaded = 0;
uint64_t    stat_gamessaved = 0;
uint64_t    stat_itemspickedup = 0;
uint64_t    stat_itemspickedup_ammo[NUMAMMO] = { 0 };
uint64_t    stat_itemspickedup_ammo_fuel = 0;
uint64_t    stat_itemspickedup_armor = 0;
uint64_t    stat_itemspickedup_health = 0;
uint64_t    stat_itemspickedup_keys = 0;
uint64_t    stat_itemspickedup_powerups = 0;
uint64_t    stat_mapsfinished = 0;
uint64_t    stat_mapsstarted = 0;
uint64_t    stat_monstersgibbed = 0;
uint64_t    stat_monsterskilled_total = 0;
uint64_t    stat_monsterskilled_infighting = 0;
uint64_t    stat_monsterskilled[NUMMOBJTYPES] = { 0 };
uint64_t    stat_monstersrespawned = 0;
uint64_t    stat_monstersresurrected = 0;
uint64_t    stat_monsterstelefragged = 0;
uint64_t    stat_runs = 0;
uint64_t    stat_secretsfound = 0;
uint64_t    stat_shotsfired[NUMWEAPONS] = { 0 };
uint64_t    stat_shotsfired_incinerator = 0;
uint64_t    stat_shotsfired_calamityblade = 0;
uint64_t    stat_shotssuccessful[NUMWEAPONS] = { 0 };
uint64_t    stat_shotssuccessful_incinerator = 0;
uint64_t    stat_shotssuccessful_calamityblade = 0;
uint64_t    stat_skilllevel_imtooyoungtodie = 0;
uint64_t    stat_skilllevel_heynottoorough = 0;
uint64_t    stat_skilllevel_hurtmeplenty = 0;
uint64_t    stat_skilllevel_ultraviolence = 0;
uint64_t    stat_skilllevel_nightmare = 0;
uint64_t    stat_suicides = 0;
uint64_t    stat_timeplayed = 0;

static bool cvarsloaded;
static int  widestcvar;

#define COMMENT(text)                                   { text "\n", "",     NULL,  DEFAULT_OTHER,         NOVALUEALIAS }
#define BLANKLINE                                       { "",        "",     NULL,  DEFAULT_OTHER,         NOVALUEALIAS }
#define CVAR_BOOL(name1, name2, cvar, alias)            { #name1,    #name2, &cvar, DEFAULT_BOOL,          alias        }
#define CVAR_INT(name1, name2, cvar, alias)             { #name1,    #name2, &cvar, DEFAULT_INT32,         alias        }
#define CVAR_INT_PERCENT(name1, name2, cvar, alias)     { #name1,    #name2, &cvar, DEFAULT_INT32_PERCENT, alias        }
#define CVAR_FLOAT(name1, name2, cvar, alias)           { #name1,    #name2, &cvar, DEFAULT_FLOAT,         alias        }
#define CVAR_FLOAT_PERCENT(name1, name2, cvar, alias)   { #name1,    #name2, &cvar, DEFAULT_FLOAT_PERCENT, alias        }
#define CVAR_STRING(name1, name2, cvar, alias)          { #name1,    #name2, &cvar, DEFAULT_STRING,        alias        }
#define CVAR_OTHER(name1, name2, cvar, alias)           { #name1,    #name2, &cvar, DEFAULT_OTHER,         alias        }
#define STAT_INT_UNSIGNED(name1, name2, cvar, alias)    { #name1,    #name2, &cvar, DEFAULT_UINT64,        alias        }

static default_t cvars[] =
{
    COMMENT("; CVARs"),
    CVAR_BOOL         (alwaysrun,                        alwaysrun,                             alwaysrun,                             BOOLVALUEALIAS      ),
    CVAR_INT          (am_allmapcdwallcolor,             am_allmapcdwallcolour,                 am_allmapcdwallcolor,                  NOVALUEALIAS        ),
    CVAR_INT          (am_allmapfdwallcolor,             am_allmapfdwallcolour,                 am_allmapfdwallcolor,                  NOVALUEALIAS        ),
    CVAR_INT          (am_allmapwallcolor,               am_allmapwallcolour,                   am_allmapwallcolor,                    NOVALUEALIAS        ),
    CVAR_BOOL         (am_antialiasing,                  am_antialiasing,                       am_antialiasing,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (am_author,                        am_author,                             am_author,                             BOOLVALUEALIAS      ),
    CVAR_INT          (am_backcolor,                     am_backcolour,                         am_backcolor,                          NOVALUEALIAS        ),
    CVAR_INT          (am_bloodsplatcolor,               am_bloodsplatcolour,                   am_bloodsplatcolor,                    NOVALUEALIAS        ),
    CVAR_INT          (am_bluedoorcolor,                 am_bluedoorcolour,                     am_bluedoorcolor,                      NOVALUEALIAS        ),
    CVAR_INT          (am_bluekeycolor,                  am_bluekeycolour,                      am_bluekeycolor,                       NOVALUEALIAS        ),
    CVAR_INT          (am_cdwallcolor,                   am_cdwallcolour,                       am_cdwallcolor,                        NOVALUEALIAS        ),
    CVAR_INT          (am_corpsecolor,                   am_corpsecolour,                       am_corpsecolor,                        NOVALUEALIAS        ),
    CVAR_BOOL         (am_correctaspectratio,            am_correctaspectratio,                 am_correctaspectratio,                 BOOLVALUEALIAS      ),
    CVAR_INT          (am_crosshaircolor,                am_crosshaircolour,                    am_crosshaircolor,                     NOVALUEALIAS        ),
    CVAR_INT          (am_display,                       am_display,                            am_display,                            NOVALUEALIAS        ),
    CVAR_BOOL         (am_dynamic,                       am_dynamic,                            am_dynamic,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (am_external,                      am_external,                           am_external,                           BOOLVALUEALIAS      ),
    CVAR_INT          (am_fdwallcolor,                   am_fdwallcolour,                       am_fdwallcolor,                        NOVALUEALIAS        ),
    CVAR_BOOL         (am_grid,                          am_grid,                               am_grid,                               BOOLVALUEALIAS      ),
    CVAR_INT          (am_gridcolor,                     am_gridcolour,                         am_gridcolor,                          NOVALUEALIAS        ),
    CVAR_OTHER        (am_gridsize,                      am_gridsize,                           am_gridsize,                           NOVALUEALIAS        ),
    CVAR_INT          (am_markcolor,                     am_markcolour,                         am_markcolor,                          NOVALUEALIAS        ),
    CVAR_BOOL         (am_mousepanning,                  am_mousepanning,                       am_mousepanning,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (am_path,                          am_path,                               am_path,                               BOOLVALUEALIAS      ),
    CVAR_INT          (am_pathcolor,                     am_pathcolour,                         am_pathcolor,                          NOVALUEALIAS        ),
    CVAR_INT          (am_pathlength,                    am_pathlength,                         am_pathlength,                         PATHLENGTHVALUEALIAS),
    CVAR_INT          (am_playercolor,                   am_playercolour,                       am_playercolor,                        NOVALUEALIAS        ),
    CVAR_BOOL         (am_playerstats,                   am_playerstats,                        am_playerstats,                        BOOLVALUEALIAS      ),
    CVAR_INT          (am_playerstatscolor,              am_playerstatscolour,                  am_playerstatscolor,                   NOVALUEALIAS        ),
    CVAR_INT          (am_reddoorcolor,                  am_reddoorcolour,                      am_reddoorcolor,                       NOVALUEALIAS        ),
    CVAR_INT          (am_redkeycolor,                   am_redkeycolour,                       am_redkeycolor,                        NOVALUEALIAS        ),
    CVAR_BOOL         (am_rotatemode,                    am_rotatemode,                         am_rotatemode,                         BOOLVALUEALIAS      ),
    CVAR_INT          (am_teleportercolor,               am_teleportercolour,                   am_teleportercolor,                    NOVALUEALIAS        ),
    CVAR_INT          (am_thingcolor,                    am_thingcolour,                        am_thingcolor,                         NOVALUEALIAS        ),
    CVAR_INT          (am_tswallcolor,                   am_tswallcolour,                       am_tswallcolor,                        NOVALUEALIAS        ),
    CVAR_INT          (am_wallcolor,                     am_wallcolour,                         am_wallcolor,                          NOVALUEALIAS        ),
    CVAR_INT          (am_yellowdoorcolor,               am_yellowdoorcolour,                   am_yellowdoorcolor,                    NOVALUEALIAS        ),
    CVAR_INT          (am_yellowkeycolor,                am_yellowkeycolour,                    am_yellowkeycolor,                     NOVALUEALIAS        ),
    CVAR_BOOL         (animatedstats,                    animatedstats,                         animatedstats,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (autoaim,                          autoaim,                               autoaim,                               BOOLVALUEALIAS      ),
    CVAR_BOOL         (autoload,                         autoload,                              autoload,                              BOOLVALUEALIAS      ),
    CVAR_BOOL         (autosave,                         autosave,                              autosave,                              BOOLVALUEALIAS      ),
    CVAR_BOOL         (autoswitch,                       autoswitch,                            autoswitch,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (autotilt,                         autotilt,                              autotilt,                              BOOLVALUEALIAS      ),
    CVAR_BOOL         (autouse,                          autouse,                               autouse,                               BOOLVALUEALIAS      ),
    CVAR_BOOL         (centerweapon,                     centreweapon,                          centerweapon,                          BOOLVALUEALIAS      ),
    CVAR_INT          (con_edgecolor,                    con_edgecolour,                        con_edgecolor,                         EDGECOLORVALUEALIAS),
    CVAR_INT          (con_timestampformat,              con_timestampformat,                   con_timestampformat,                   TIMESTAMPVALUEALIAS),
    CVAR_BOOL         (con_timestamps,                   con_timestamps,                        con_timestamps,                        BOOLVALUEALIAS      ),
    CVAR_INT          (con_warninglevel,                 con_warninglevel,                      con_warninglevel,                      NOVALUEALIAS        ),
    CVAR_INT          (crosshair,                        crosshair,                             crosshair,                             CROSSHAIRVALUEALIAS),
    CVAR_INT          (crosshaircolor,                   crosshaircolour,                       crosshaircolor,                        NOVALUEALIAS        ),
    CVAR_BOOL         (english,                          english,                               english,                               ENGLISHVALUEALIAS   ),
    CVAR_INT          (episode,                          episode,                               episode,                               NOVALUEALIAS        ),
    CVAR_INT          (expansion,                        expansion,                             expansion,                             NOVALUEALIAS        ),
    CVAR_INT          (facebackcolor,                    facebackcolour,                        facebackcolor,                         NOVALUEALIAS        ),
    CVAR_BOOL         (fade,                             fade,                                  fade,                                  BOOLVALUEALIAS      ),
    CVAR_BOOL         (flashkeys,                        flashkeys,                             flashkeys,                             BOOLVALUEALIAS      ),
    CVAR_BOOL         (freelook,                         mouselook,                             freelook,                              BOOLVALUEALIAS      ),
    CVAR_BOOL         (groupmessages,                    groupmessages,                         groupmessages,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (infighting,                       infighting,                            infighting,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (infiniteheight,                   infiniteheight,                        infiniteheight,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (joy_analog,                       gp_analog,                             joy_analog,                            BOOLVALUEALIAS      ),
    CVAR_FLOAT_PERCENT(joy_deadzone_left,                gp_deadzone_left,                      joy_deadzone_left,                     NOVALUEALIAS        ),
    CVAR_FLOAT_PERCENT(joy_deadzone_right,               gp_deadzone_right,                     joy_deadzone_right,                    NOVALUEALIAS        ),
    CVAR_BOOL         (joy_invertyaxis,                  gp_invertyaxis,                        joy_invertyaxis,                       BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (joy_rumble_barrels,               gp_vibrate_barrels,                    joy_rumble_barrels,                    NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (joy_rumble_damage,                gp_vibrate_damage,                     joy_rumble_damage,                     NOVALUEALIAS        ),
    CVAR_BOOL         (joy_rumble_fall,                  joy_rumble_fall,                       joy_rumble_fall,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (joy_rumble_pickup,                joy_rumble_pickup,                     joy_rumble_pickup,                     BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (joy_rumble_weapons,               gp_vibrate_weapons,                    joy_rumble_weapons,                    NOVALUEALIAS        ),
    CVAR_FLOAT        (joy_sensitivity_horizontal,       gp_sensitivity_horizontal,             joy_sensitivity_horizontal,            NOVALUEALIAS        ),
    CVAR_FLOAT        (joy_sensitivity_vertical,         gp_sensitivity_vertical,               joy_sensitivity_vertical,              NOVALUEALIAS        ),
    CVAR_BOOL         (joy_swapthumbsticks,              gp_swapthumbsticks,                    joy_swapthumbsticks,                   BOOLVALUEALIAS      ),
    CVAR_INT          (joy_thumbsticks,                  gp_thumbsticks,                        joy_thumbsticks,                       NOVALUEALIAS        ),
    CVAR_BOOL         (m_acceleration,                   m_acceleration,                        m_acceleration,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (m_invertyaxis,                    m_invertyaxis,                         m_invertyaxis,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (m_novertical,                     m_novertical,                          m_novertical,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (m_pointer,                        m_pointer,                             m_pointer,                             BOOLVALUEALIAS      ),
    CVAR_FLOAT        (m_sensitivity,                    m_sensitivity,                         m_sensitivity,                         NOVALUEALIAS        ),
    CVAR_BOOL         (melt,                             wipe,                                  melt,                                  BOOLVALUEALIAS      ),
    CVAR_BOOL         (menuhighlight,                    menuhighlight,                         menuhighlight,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (menushadow,                       menushadow,                            menushadow,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (menuspin,                         menuspin,                              menuspin,                              BOOLVALUEALIAS      ),
    CVAR_BOOL         (messages,                         messages,                              messages,                              BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (movebob,                          movebob,                               movebob,                               NOVALUEALIAS        ),
    CVAR_BOOL         (negativehealth,                   negativehealth,                        negativehealth,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (obituaries,                       con_obituaries,                        obituaries,                            BOOLVALUEALIAS      ),
    CVAR_INT          (playergender,                     playergender,                          playergender,                          GENDERVALUEALIAS    ),
    CVAR_STRING       (playername,                       playername,                            playername,                            NOVALUEALIAS        ),
    CVAR_BOOL         (r_althudfont,                     r_althudfont,                          r_althudfont,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_antialiasing,                   r_supersampling,                       r_antialiasing,                        BOOLVALUEALIAS      ),
    CVAR_INT          (r_berserkeffect,                  r_berserkeffect,                       r_berserkeffect,                       NOVALUEALIAS        ),
    CVAR_INT          (r_blood,                          r_blood,                               r_blood,                               BLOODVALUEALIAS     ),
    CVAR_BOOL         (r_blood_gibs,                     r_blood_gibs,                          r_blood_gibs,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_blood_melee,                    r_blood_melee,                         r_blood_melee,                         BOOLVALUEALIAS      ),
    CVAR_INT          (r_bloodsplats_max,                r_bloodsplats_max,                     r_bloodsplats_max,                     NOVALUEALIAS        ),
    CVAR_BOOL         (r_bloodsplats_translucency,       r_bloodsplats_translucency,            r_bloodsplats_translucency,            BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_brightmaps,                     r_brightmaps,                          r_brightmaps,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_color,                  r_corpses_colour,                      r_corpses_color,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_gib,                    r_corpses_gib,                         r_corpses_gib,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_mirrored,               r_corpses_mirrored,                    r_corpses_mirrored,                    BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_moreblood,              r_corpses_moreblood,                   r_corpses_moreblood,                   BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_nudge,                  r_corpses_nudge,                       r_corpses_nudge,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_slide,                  r_corpses_slide,                       r_corpses_slide,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_corpses_smearblood,             r_corpses_smearblood,                  r_corpses_smearblood,                  BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_damageeffect,                   r_damageeffect,                        r_damageeffect,                        BOOLVALUEALIAS      ),
    CVAR_INT          (r_detail,                         r_detail,                              r_detail,                              DETAILVALUEALIAS    ),
    CVAR_BOOL         (r_diskicon,                       r_diskicon,                            r_diskicon,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_ditheredlighting,               r_ditheredlighting,                    r_ditheredlighting,                    BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (r_extralighting,                  r_levelbrightness,                     r_extralighting,                       NOVALUEALIAS        ),
    CVAR_BOOL         (r_fixmaperrors,                   r_fixmaperrors,                        r_fixmaperrors,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_fixspriteoffsets,               r_fixspriteoffsets,                    r_fixspriteoffsets,                    BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_floatbob,                       r_floatbob,                            r_floatbob,                            BOOLVALUEALIAS      ),
    CVAR_INT          (r_fov,                            r_fov,                                 r_fov,                                 NOVALUEALIAS        ),
    CVAR_FLOAT        (r_gamma,                          r_gamma,                               r_gamma,                               GAMMAVALUEALIAS     ),
    CVAR_BOOL         (r_graduallighting,                r_graduallighting,                     r_graduallighting,                     BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_homindicator,                   r_homindicator,                        r_homindicator,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_hud,                            r_hud,                                 r_hud,                                 BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_hud_translucency,               r_hud_translucency,                    r_hud_translucency,                    BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_linearskies,                    r_linearskies,                         r_linearskies,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_bob,                     r_liquid_bob,                          r_liquid_bob,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_bobsprites,              r_liquid_bobsprites,                   r_liquid_bobsprites,                   BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_clipsprites,             r_liquid_clipsprites,                  r_liquid_clipsprites,                  BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_current,                 r_liquid_current,                      r_liquid_current,                      BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_lowerview,               r_liquid_lowerview,                    r_liquid_lowerview,                    BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_liquid_swirl,                   r_liquid_swirl,                        r_liquid_swirl,                        BOOLVALUEALIAS      ),
    CVAR_OTHER        (r_lowpixelsize,                   r_lowpixelsize,                        r_lowpixelsize,                        NOVALUEALIAS        ),
    CVAR_BOOL         (r_mirroredweapons,                r_mirroredweapons,                     r_mirroredweapons,                     BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_percolumnlighting,              r_percolumnlighting,                   r_percolumnlighting,                   BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_pickupeffect,                   r_pickupeffect,                        r_pickupeffect,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_playersprites,                  r_playersprites,                       r_playersprites,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_radsuiteffect,                  r_radsuiteffect,                       r_radsuiteffect,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_randomstartframes,              r_randomstartframes,                   r_randomstartframes,                   BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_rockettrails,                   r_rockettrails,                        r_rockettrails,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_rockettrails_translucency,      r_rockettrails_translucency,           r_rockettrails_translucency,           BOOLVALUEALIAS      ),
    CVAR_INT          (r_screensize,                     r_screensize,                          r_screensize,                          NOVALUEALIAS        ),
    CVAR_BOOL         (r_shadows,                        r_shadows,                             r_shadows,                             BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_shadows_translucency,           r_shadows_translucency,                r_shadows_translucency,                BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_shake_barrels,                  r_shake_barrels,                       r_shake_barrels,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_shake_berserk,                  r_shake_berserk,                       r_shake_berserk,                       BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_shake_damage,                   r_shake_damage,                        r_shake_damage,                        BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_sprites_translucency,           r_translucency,                        r_sprites_translucency,                BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_textures,                       r_textures,                            r_textures,                            BOOLVALUEALIAS      ),
    CVAR_BOOL         (r_textures_translucency,          r_textures_translucency,               r_textures_translucency,               BOOLVALUEALIAS      ),
    CVAR_INT          (s_channels,                       s_channels,                            s_channels,                            NOVALUEALIAS        ),
    CVAR_BOOL         (s_fullsfx,                        s_fullsfx,                             s_fullsfx,                             BOOLVALUEALIAS      ),
    CVAR_BOOL         (s_lowermenumusic,                 s_lowermenumusic,                      s_lowermenumusic,                      BOOLVALUEALIAS      ),
    CVAR_BOOL         (s_musicinbackground,              s_musicinbackground,                   s_musicinbackground,                   BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (s_musicvolume,                    s_musicvolume,                         s_musicvolume,                         NOVALUEALIAS        ),
    CVAR_BOOL         (s_randommusic,                    s_randommusic,                         s_randommusic,                         BOOLVALUEALIAS      ),
    CVAR_BOOL         (s_randompitch,                    s_randompitch,                         s_randompitch,                         BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (s_sfxvolume,                      s_sfxvolume,                           s_sfxvolume,                           NOVALUEALIAS        ),
    CVAR_BOOL         (s_stereo,                         s_stereo,                              s_stereo,                              BOOLVALUEALIAS      ),
    CVAR_INT          (savegame,                         savegame,                              savegame,                              NOVALUEALIAS        ),
    CVAR_BOOL         (secretmessages,                   secretmessages,                        secretmessages,                        BOOLVALUEALIAS      ),
    CVAR_INT          (skilllevel,                       skilllevel,                            skilllevel,                            NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (stillbob,                         stillbob,                              stillbob,                              NOVALUEALIAS        ),
    CVAR_INT          (sucktime,                         sucktime,                              sucktime,                              SUCKSVALUEALIAS     ),
    CVAR_BOOL         (tossdrop,                         tossdrop,                              tossdrop,                              BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (turbo,                            turbo,                                 turbo,                                 NOVALUEALIAS        ),
    CVAR_INT          (units,                            units,                                 units,                                 UNITSVALUEALIAS     ),
    CVAR_STRING       (version,                          version,                               version,                               NOVALUEALIAS        ),
    CVAR_INT          (vid_aspectratio,                  vid_aspectratio,                       vid_aspectratio,                       RATIOVALUEALIAS     ),
    CVAR_INT_PERCENT  (vid_blue,                         vid_blue,                              vid_blue,                              NOVALUEALIAS        ),
    CVAR_BOOL         (vid_borderlesswindow,             vid_borderlesswindow,                  vid_borderlesswindow,                  BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (vid_brightness,                   vid_brightness,                        vid_brightness,                        NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (vid_contrast,                     vid_contrast,                          vid_contrast,                          NOVALUEALIAS        ),
    CVAR_INT          (vid_display,                      vid_display,                           vid_display,                           NOVALUEALIAS        ),
    CVAR_BOOL         (vid_fullscreen,                   vid_fullscreen,                        vid_fullscreen,                        BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (vid_green,                        vid_green,                             vid_green,                             NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (vid_motionblur,                   vid_motionblur,                        vid_motionblur,                        NOVALUEALIAS        ),
    CVAR_BOOL         (vid_pillarboxes,                  vid_pillarboxes,                       vid_pillarboxes,                       BOOLVALUEALIAS      ),
    CVAR_INT_PERCENT  (vid_red,                          vid_red,                               vid_red,                               NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (vid_saturation,                   vid_saturation,                        vid_saturation,                        NOVALUEALIAS        ),
    CVAR_STRING       (vid_scaleapi,                     vid_scaleapi,                          vid_scaleapi,                          NOVALUEALIAS        ),
    CVAR_STRING       (vid_scalefilter,                  vid_scalefilter,                       vid_scalefilter,                       NOVALUEALIAS        ),
    CVAR_OTHER        (vid_screenresolution,             vid_screenresolution,                  vid_screenresolution,                  NOVALUEALIAS        ),
    CVAR_INT          (vid_vsync,                        vid_vsync,                             vid_vsync,                             VSYNCVALUEALIAS     ),
    CVAR_BOOL         (vid_widescreen,                   vid_widescreen,                        vid_widescreen,                        BOOLVALUEALIAS      ),
    CVAR_OTHER        (vid_windowpos,                    vid_windowposition,                    vid_windowpos,                         NOVALUEALIAS        ),
    CVAR_OTHER        (vid_windowsize,                   vid_windowsize,                        vid_windowsize,                        NOVALUEALIAS        ),
    CVAR_STRING       (wad,                              wad,                                   wad,                                   NOVALUEALIAS        ),
    CVAR_STRING       (wadfolder,                        iwadfolder,                            wadfolder,                             NOVALUEALIAS        ),
    CVAR_INT_PERCENT  (weaponbob,                        weaponbob,                             weaponbob,                             NOVALUEALIAS        ),
    CVAR_BOOL         (weaponbounce,                     weaponbounce,                          weaponbounce,                          BOOLVALUEALIAS      ),
    CVAR_BOOL         (weaponrecoil,                     weaponrecoil,                          weaponrecoil,                          BOOLVALUEALIAS      ),
    BLANKLINE,
    COMMENT("; player stats"),
    STAT_INT_UNSIGNED (automapopened,                    stat_automapopened,                    stat_automapopened,                    NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (barrelsexploded,                  stat_barrelsexploded,                  stat_barrelsexploded,                  NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (cheatsentered,                    stat_cheats,                           stat_cheatsentered,                    NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (damageinflicted,                  stat_damageinflicted,                  stat_damageinflicted,                  NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (damagereceived,                   stat_damagereceived,                   stat_damagereceived,                   NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (deaths,                           stat_deaths,                           stat_deaths,                           NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (distancetraveled,                 stat_distancetraveled,                 stat_distancetraveled,                 NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (firstrun,                         stat_firstrun,                         stat_firstrun,                         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (gamesloaded,                      stat_gamesloaded,                      stat_gamesloaded,                      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (gamessaved,                       stat_gamessaved,                       stat_gamessaved,                       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup,                    stat_itemspickedup,                    stat_itemspickedup,                    NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_ammo_bullets,       stat_itemspickedup_ammo_bullets,       stat_itemspickedup_ammo[am_clip],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_ammo_shells,        stat_itemspickedup_ammo_shells,        stat_itemspickedup_ammo[am_shell],     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_ammo_rockets,       stat_itemspickedup_ammo_rockets,       stat_itemspickedup_ammo[am_misl],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_ammo_cells,         stat_itemspickedup_ammo_cells,         stat_itemspickedup_ammo[am_cell],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_ammo_fuel,          stat_itemspickedup_ammo_fuel,          stat_itemspickedup_ammo_fuel,          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_armor,              stat_itemspickedup_armor,              stat_itemspickedup_armor,              NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_health,             stat_itemspickedup_health,             stat_itemspickedup_health,             NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_keys,               stat_itemspickedup_keys,               stat_itemspickedup_keys,               NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (itemspickedup_powerups,           stat_itemspickedup_powerups,           stat_itemspickedup_powerups,           NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (mapsfinished,                     mapscompleted,                         stat_mapsfinished,                     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (mapsstarted,                      stat_mapsstarted,                      stat_mapsstarted,                      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monstersgibbed,                   stat_monstersgibbed,                   stat_monstersgibbed,                   NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled,                   stat_monsterskilled,                   stat_monsterskilled_total,             NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_infighting,        stat_monsterskilled_infighting,        stat_monsterskilled_infighting,        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_arachnotrons,      stat_monsterskilled_arachnotrons,      stat_monsterskilled[MT_BABY],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_archviles,         stat_monsterskilled_archviles,         stat_monsterskilled[MT_VILE],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_banshees,          stat_monsterskilled_banshees,          stat_monsterskilled[MT_BANSHEE],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_baronsofhell,      stat_monsterskilled_baronsofhell,      stat_monsterskilled[MT_BRUISER],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_cacodemons,        stat_monsterskilled_cacodemons,        stat_monsterskilled[MT_HEAD],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_chaingunners,      stat_monsterskilled_heavyweapondudes,  stat_monsterskilled[MT_CHAINGUY],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_cyberdemons,       stat_monsterskilled_cyberdemons,       stat_monsterskilled[MT_CYBORG],        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_ghouls,            stat_monsterskilled_ghouls,            stat_monsterskilled[MT_GHOUL],         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_hellknights,       stat_monsterskilled_hellknights,       stat_monsterskilled[MT_KNIGHT],        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_imps,              stat_monsterskilled_imps,              stat_monsterskilled[MT_TROOP],         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_lostsouls,         stat_monsterskilled_lostsouls,         stat_monsterskilled[MT_SKULL],         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_mancubi,           stat_monsterskilled_mancubi,           stat_monsterskilled[MT_FATSO],         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_mindweavers,       stat_monsterskilled_mindweavers,       stat_monsterskilled[MT_MINDWEAVER],    NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_painelementals,    stat_monsterskilled_painelementals,    stat_monsterskilled[MT_PAIN],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_pinkydemons,       stat_monsterskilled_demons,            stat_monsterskilled[MT_SERGEANT],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_revenants,         stat_monsterskilled_revenants,         stat_monsterskilled[MT_UNDEAD],        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_shocktroopers,     stat_monsterskilled_shocktroopers,     stat_monsterskilled[MT_SHOCKTROOPER],  NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_shotgunguys,       stat_monsterskilled_shotgunguys,       stat_monsterskilled[MT_SHOTGUY],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_spectres,          stat_monsterskilled_spectres,          stat_monsterskilled[MT_SHADOWS],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_spidermasterminds, stat_monsterskilled_spidermasterminds, stat_monsterskilled[MT_SPIDER],        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_tyrants,           stat_monsterskilled_tyrants,           stat_monsterskilled[MT_TYRANT],        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_vassagos,          stat_monsterskilled_vassagos,          stat_monsterskilled[MT_VASSAGO],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterskilled_zombiemen,         stat_monsterskilled_zombiemen,         stat_monsterskilled[MT_POSSESSED],     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monstersrespawned,                stat_monstersrespawned,                stat_monstersrespawned,                NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monstersresurrected,              stat_monstersresurrected,              stat_monstersresurrected,              NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (monsterstelefragged,              stat_monsterstelefragged,              stat_monsterstelefragged,              NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (runs,                             stat_runs,                             stat_runs,                             NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (secretsfound,                     stat_secretsrevealed,                  stat_secretsfound,                     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_fists,                 stat_shotsfired_fists,                 stat_shotsfired[wp_fist],              NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_chainsaw,              stat_shotsfired_chainsaw,              stat_shotsfired[wp_chainsaw],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_pistol,                stat_shotsfired_pistol,                stat_shotsfired[wp_pistol],            NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_shotgun,               stat_shotsfired_shotgun,               stat_shotsfired[wp_shotgun],           NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_supershotgun,          stat_shotsfired_supershotgun,          stat_shotsfired[wp_supershotgun],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_chaingun,              stat_shotsfired_chaingun,              stat_shotsfired[wp_chaingun],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_rocketlauncher,        stat_shotsfired_rocketlauncher,        stat_shotsfired[wp_missile],           NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_plasmarifle,           stat_shotsfired_plasmarifle,           stat_shotsfired[wp_plasma],            NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_bfg9000,               stat_shotsfired_bfg9000,               stat_shotsfired[wp_bfg],               NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_incinerator,           stat_shotsfired_incinerator,           stat_shotsfired_incinerator,           NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotsfired_calamityblade,         stat_shotsfired_calamityblade,         stat_shotsfired_calamityblade,         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_fists,            stat_shotssuccessful_fist,             stat_shotssuccessful[wp_fist],         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_chainsaw,         stat_shotssuccessful_chainsaw,         stat_shotssuccessful[wp_chainsaw],     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_pistol,           stat_shotssuccessful_pistol,           stat_shotssuccessful[wp_pistol],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_shotgun,          stat_shotssuccessful_shotgun,          stat_shotssuccessful[wp_shotgun],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_supershotgun,     stat_shotssuccessful_supershotgun,     stat_shotssuccessful[wp_supershotgun], NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_chaingun,         stat_shotssuccessful_chaingun,         stat_shotssuccessful[wp_chaingun],     NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_rocketlauncher,   stat_shotssuccessful_rocketlauncher,   stat_shotssuccessful[wp_missile],      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_plasmarifle,      stat_shotssuccessful_plasmarifle,      stat_shotssuccessful[wp_plasma],       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_bfg9000,          stat_shotssuccessful_bfg9000,          stat_shotssuccessful[wp_bfg],          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_incinerator,      stat_shotssuccessful_incinerator,      stat_shotssuccessful_incinerator,      NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (shotssuccessful_calamityblade,    stat_shotssuccessful_calamityblade,    stat_shotssuccessful_calamityblade,    NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (skilllevel_imtooyoungtodie,       stat_skilllevel_imtooyoungtodie,       stat_skilllevel_imtooyoungtodie,       NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (skilllevel_heynottoorough,        stat_skilllevel_heynottoorough,        stat_skilllevel_heynottoorough,        NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (skilllevel_hurtmeplenty,          stat_skilllevel_hurtmeplenty,          stat_skilllevel_hurtmeplenty,          NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (skilllevel_ultraviolence,         stat_skilllevel_ultraviolence,         stat_skilllevel_ultraviolence,         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (skilllevel_nightmare,             stat_skilllevel_nightmare,             stat_skilllevel_nightmare,             NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (suicides,                         stat_suicides,                         stat_suicides,                         NOVALUEALIAS        ),
    STAT_INT_UNSIGNED (timeplayed,                       stat_time,                             stat_timeplayed,                       NOVALUEALIAS        )
};

valuealias_t valuealiases[] =
{
    { "none",           armortype_none,               ARMORTYPEVALUEALIAS  },
    { "green",          armortype_green,              ARMORTYPEVALUEALIAS  },
    { "blue",           armortype_blue,               ARMORTYPEVALUEALIAS  },
    { "none",           r_blood_none,                 BLOODVALUEALIAS      },
    { "red",            r_blood_red,                  BLOODVALUEALIAS      },
    { "all",            r_blood_all,                  BLOODVALUEALIAS      },
    { "green",          r_blood_green,                BLOODVALUEALIAS      },
    { "nofuzz",         r_blood_nofuzz,               BLOODVALUEALIAS      },
    { "off",            0,                            BOOLVALUEALIAS       },
    { "0",              0,                            BOOLVALUEALIAS       },
    { "false",          0,                            BOOLVALUEALIAS       },
    { "no",             0,                            BOOLVALUEALIAS       },
    { "on",             1,                            BOOLVALUEALIAS       },
    { "1",              1,                            BOOLVALUEALIAS       },
    { "true",           1,                            BOOLVALUEALIAS       },
    { "yes",            1,                            BOOLVALUEALIAS       },
    { "off",            0,                            CAPVALUEALIAS        },
    { "none",           crosshair_none,               CROSSHAIRVALUEALIAS  },
    { "off",            crosshair_none,               CROSSHAIRVALUEALIAS  },
    { "cross",          crosshair_cross,              CROSSHAIRVALUEALIAS  },
    { "on",             crosshair_cross,              CROSSHAIRVALUEALIAS  },
    { "angle",          crosshair_angle,              CROSSHAIRVALUEALIAS  },
    { "dot",            crosshair_dot,                CROSSHAIRVALUEALIAS  },
    { "bigcross",       crosshair_bigcross,           CROSSHAIRVALUEALIAS  },
    { "circle",         crosshair_circle,             CROSSHAIRVALUEALIAS  },
    { "bigcircle",      crosshair_bigcircle,          CROSSHAIRVALUEALIAS  },
    { "chevron",        crosshair_chevron,            CROSSHAIRVALUEALIAS  },
    { "chevrons",       crosshair_chevrons,           CROSSHAIRVALUEALIAS  },
    { "arcs",           crosshair_arcs,               CROSSHAIRVALUEALIAS  },
    { "low",            r_detail_low,                 DETAILVALUEALIAS     },
    { "high",           r_detail_high,                DETAILVALUEALIAS     },
    { "auto",           con_edgecolor_auto,           EDGECOLORVALUEALIAS  },
    { "american",       english_american,             ENGLISHVALUEALIAS    },
    { "british",        english_british,              ENGLISHVALUEALIAS    },
    { "off",            1,                            GAMMAVALUEALIAS      },
    { "other",          playergender_other,           GENDERVALUEALIAS     },
    { "male",           playergender_male,            GENDERVALUEALIAS     },
    { "female",         playergender_female,          GENDERVALUEALIAS     },
    { "short",          am_pathlength_short,          PATHLENGTHVALUEALIAS },
    { "medium",         am_pathlength_medium,         PATHLENGTHVALUEALIAS },
    { "long",           am_pathlength_long,           PATHLENGTHVALUEALIAS },
    { "endless",        am_pathlength_endless,        PATHLENGTHVALUEALIAS },
    { "auto",           vid_aspectratio_auto,         RATIOVALUEALIAS      },
    { "16:9",           vid_aspectratio_16_9,         RATIOVALUEALIAS      },
    { "16:10",          vid_aspectratio_16_10,        RATIOVALUEALIAS      },
    { "21:9",           vid_aspectratio_21_9,         RATIOVALUEALIAS      },
    { "32:9",           vid_aspectratio_32_9,         RATIOVALUEALIAS      },
    { "off",            0,                            SUCKSVALUEALIAS      },
    { "imperial",       units_imperial,               UNITSVALUEALIAS      },
    { "metric",         units_metric,                 UNITSVALUEALIAS      },
    { "military",       con_timestampformat_military, TIMESTAMPVALUEALIAS  },
    { "standard",       con_timestampformat_standard, TIMESTAMPVALUEALIAS  },
    { "adaptive",       vid_vsync_adaptive,           VSYNCVALUEALIAS      },
    { "off",            vid_vsync_off,                VSYNCVALUEALIAS      },
    { "on",             vid_vsync_on,                 VSYNCVALUEALIAS      },
    { "fists",          wp_fist,                      WEAPONVALUEALIAS     },
    { "pistol",         wp_pistol,                    WEAPONVALUEALIAS     },
    { "shotgun",        wp_shotgun,                   WEAPONVALUEALIAS     },
    { "chaingun",       wp_chaingun,                  WEAPONVALUEALIAS     },
    { "rocketlauncher", wp_missile,                   WEAPONVALUEALIAS     },
    { "plasmarifle",    wp_plasma,                    WEAPONVALUEALIAS     },
    { "incinerator",    wp_incinerator,               WEAPONVALUEALIAS     },
    { "bfg9000",        wp_bfg,                       WEAPONVALUEALIAS     },
    { "calamityblade",  wp_calamityblade,             WEAPONVALUEALIAS     },
    { "chainsaw",       wp_chainsaw,                  WEAPONVALUEALIAS     },
    { "supershotgun",   wp_supershotgun,              WEAPONVALUEALIAS     },
    { "",               0,                            NOVALUEALIAS         }
};

static void SaveBind(FILE *file, const char *control, const char *action)
{
    if (action[0] == '+')
    {
        if (strlen(control) == 1)
            fprintf(file, "bind '%s' %s\n", (control[0] == '=' ? "+" : control), action);
        else
            fprintf(file, "bind %s %s\n", control, action);
    }
    else
    {
        if (strlen(control) == 1)
            fprintf(file, "bind '%s' \"%s\"\n", (control[0] == '=' ? "+" : control), action);
        else
            fprintf(file, "bind %s \"%s\"\n", control, action);
    }
}

static void SaveBindByValue(FILE *file, char *action, int value, controltype_t type)
{
    for (int i = 0; controls[i].type; i++)
        if (controls[i].type == type && controls[i].value == value)
        {
            SaveBind(file, controls[i].control, action);
            break;
        }
}

// Parses bool values in the configuration file
static int ParseBoolParameter(const char *cvar, const char *value, const int valuealiastype)
{
    int index;
    int defaultnumber;

    for (int i = 0; *valuealiases[i].text; i++)
        if (M_StringCompare(value, valuealiases[i].text) && valuealiastype == valuealiases[i].type)
            return valuealiases[i].value;

    index = C_GetIndex(cvar);
    defaultnumber = (int)consolecmds[index].defaultnumber;

    C_Warning(0, "The " BOLD("%s") " CVAR in " BOLD(DOOMRETRO_CONFIGFILE) " is invalid and has been reset "
        "to its default of " BOLD("%s") ".", consolecmds[index].name, (defaultnumber ? "on" : "off"));

    return defaultnumber;
}

// Parses integer values in the configuration file
static int ParseIntParameter(const char *cvar, const char *strparm, const int valuealiastype)
{
    int parm;
    int index;

    for (int i = 0; *valuealiases[i].text; i++)
        if (M_StringCompare(strparm, valuealiases[i].text) && valuealiastype == valuealiases[i].type)
            return valuealiases[i].value;

    index = C_GetIndex(cvar);

    if (sscanf(strparm, "%10i", &parm) == 1
        && parm >= consolecmds[index].minimumvalue && parm <= consolecmds[index].maximumvalue)
        return parm;
    else
    {
        int     defaultnumber = (int)consolecmds[index].defaultnumber;
        char    *temp = C_LookupAliasFromValue(defaultnumber, consolecmds[index].aliases);

        C_Warning(0, "The " BOLD("%s") " CVAR in " BOLD(DOOMRETRO_CONFIGFILE) " is "
            "invalid and has been reset to its default of " BOLD("%s%s") ".",
            consolecmds[index].name, temp, (consolecmds[index].flags == CF_PERCENT ? "%%" : ""));
        free(temp);

        return defaultnumber;
    }
}

// Parses float values in the configuration file
static float ParseFloatParameter(const char *cvar, const char *strparm, const int valuealiastype)
{
    float   parm;
    int     index;

    for (int i = 0; *valuealiases[i].text; i++)
        if (M_StringCompare(strparm, valuealiases[i].text) && valuealiastype == valuealiases[i].type)
            return (float)valuealiases[i].value;

    index = C_GetIndex(cvar);

    if (M_StringCompare(cvar, stringize(r_gamma))
        && sscanf(strparm, "%10f", &parm) == 1
        && parm >= r_gamma_min && parm <= r_gamma_max)
        return parm;
    else if (sscanf(strparm, "%10f", &parm) == 1
        && parm >= (float)consolecmds[index].minimumvalue && parm <= (float)consolecmds[index].maximumvalue)
        return parm;
    else
    {
        float   defaultnumber = consolecmds[index].defaultnumber;
        char    *temp = striptrailingzero(*(float *)consolecmds[index].variable, 1);

        C_Warning(0, "The " BOLD("%s") " CVAR in " BOLD(DOOMRETRO_CONFIGFILE) " is "
            "invalid and has been reset to its default of " BOLD("%s%s") ".",
            consolecmds[index].name, temp, (consolecmds[index].flags == CF_PERCENT ? "%%" : ""));
        free(temp);

        return defaultnumber;
    }
}

static bool M_EarlierVersion(char *version1, char *version2)
{
    int major1 = 0;
    int minor1 = 0;
    int patch1 = 0;
    int major2 = 0;
    int minor2 = 0;
    int patch2 = 0;

    if (sscanf(version1, "%i.%i.%i", &major1, &minor1, &patch1) == 3)
    {
        if (sscanf(version2, "%i.%i.%i", &major2, &minor2, &patch2) == 3)
            return (major1 < major2
                || (major1 == major2 && minor1 < minor2)
                || (major1 == major2 && minor1 == minor2 && patch1 < patch2));
        else if (sscanf(version2, "%i.%i", &major2, &minor2) == 2)
            return (major1 < major2
                || (major1 == major2 && minor1 <= minor2));
    }
    else if (sscanf(version1, "%i.%i", &major1, &minor1) == 2)
    {
        if (sscanf(version2, "%i.%i.%i", &major2, &minor2, &patch2) == 3)
            return (major1 < major2
                || (major1 == major2 && minor1 <= minor2));
        else if (sscanf(version2, "%i.%i", &major2, &minor2) == 2)
            return (major1 < major2
                || (major1 == major2 && minor1 < minor2));
    }

    return false;
}

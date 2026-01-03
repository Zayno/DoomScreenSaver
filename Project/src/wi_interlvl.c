

#include "c_console.h"
#include "cJSON/cJSON.h"
#include "doomtype.h"
#include "m_array.h"
#include "w_wad.h"
#include "wi_interlvl.h"

static bool ParseCondition(cJSON *json, interlevelcond_t *out)
{
    cJSON   *condition;
    cJSON   *param;

    if (!cJSON_IsNumber((condition = cJSON_GetObjectItemCaseSensitive(json, "condition"))))
        return false;

    out->condition = condition->valueint;

    if (!cJSON_IsNumber((param = cJSON_GetObjectItemCaseSensitive(json, "param"))))
        return false;

    out->param = param->valueint;
    return true;
}

static bool ParseFrame(cJSON *json, interlevelframe_t *out)
{
    cJSON   *imagelump;
    cJSON   *type;
    cJSON   *duration;
    cJSON   *maxduration;

    if (!cJSON_IsString((imagelump = cJSON_GetObjectItemCaseSensitive(json, "image"))))
        return false;

    out->imagelump = M_StringDuplicate(imagelump->valuestring);

    if (!cJSON_IsNumber((type = cJSON_GetObjectItemCaseSensitive(json, "type"))))
        return false;

    out->type = type->valueint;

    if (!cJSON_IsNumber((duration = cJSON_GetObjectItemCaseSensitive(json, "duration"))))
        return false;

    out->duration = (int)(duration->valuedouble * TICRATE);

    if (!cJSON_IsNumber((maxduration = cJSON_GetObjectItemCaseSensitive(json, "maxduration"))))
        return false;

    out->maxduration = (int)(maxduration->valuedouble * TICRATE);
    return true;
}

static bool ParseAnim(cJSON *json, interlevelanim_t *out)
{
    cJSON               *js_frames;
    cJSON               *js_frame = NULL;
    cJSON               *xpos;
    cJSON               *ypos;
    cJSON               *js_conditions;
    cJSON               *js_condition = NULL;
    interlevelframe_t   *frames = NULL;
    interlevelcond_t    *conditions = NULL;

    js_frames = cJSON_GetObjectItemCaseSensitive(json, "frames");

    cJSON_ArrayForEach(js_frame, js_frames)
    {
        interlevelframe_t   frame = { 0 };

        if (ParseFrame(js_frame, &frame))
            array_push(frames, frame);
    }

    out->frames = frames;

    if (!cJSON_IsNumber((xpos = cJSON_GetObjectItemCaseSensitive(json, "x")))
        || !cJSON_IsNumber((ypos = cJSON_GetObjectItemCaseSensitive(json, "y"))))
        return false;

    out->xpos = xpos->valueint;
    out->ypos = ypos->valueint;

    js_conditions = cJSON_GetObjectItemCaseSensitive(json, "conditions");

    cJSON_ArrayForEach(js_condition, js_conditions)
    {
        interlevelcond_t    condition = { 0 };

        if (ParseCondition(js_condition, &condition))
            array_push(conditions, condition);
    }

    out->conditions = conditions;
    return true;
}

static void ParseLevelLayer(cJSON *json, interlevellayer_t *out)
{
    cJSON               *js_anims;
    cJSON               *js_anim = NULL;
    cJSON               *js_conditions;
    cJSON               *js_condition = NULL;
    interlevelanim_t    *anims = NULL;
    interlevelcond_t    *conditions = NULL;

    js_anims = cJSON_GetObjectItemCaseSensitive(json, "anims");

    cJSON_ArrayForEach(js_anim, js_anims)
    {
        interlevelanim_t    anim = { 0 };

        if (ParseAnim(js_anim, &anim))
            array_push(anims, anim);
    }

    out->anims = anims;
    js_conditions = cJSON_GetObjectItemCaseSensitive(json, "conditions");

    cJSON_ArrayForEach(js_condition, js_conditions)
    {
        interlevelcond_t    condition = { 0 };

        if (ParseCondition(js_condition, &condition))
            array_push(conditions, condition);
    }

    out->conditions = conditions;
}

interlevel_t *WI_ParseInterlevel(const char *lumpname)
{
    int                 lumpnum = W_CheckNumForName(lumpname);
    interlevel_t        *out;
    cJSON               *json;
    cJSON               *data;
    cJSON               *music;
    cJSON               *backgroundimage;
    cJSON               *js_layers;
    cJSON               *js_layer = NULL;
    interlevellayer_t   *layers = NULL;

    if (!(json = cJSON_ParseWithLength(W_CacheLumpNum(lumpnum), W_LumpLength(lumpnum)))
        || !cJSON_IsObject((data = cJSON_GetObjectItemCaseSensitive(json, "data")))
        || !cJSON_IsString((music = cJSON_GetObjectItemCaseSensitive(data, "music")))
        || !cJSON_IsString((backgroundimage = cJSON_GetObjectItemCaseSensitive(data, "backgroundimage"))))
    {
        cJSON_Delete(json);
        C_Warning(1, "The " BOLD("%s") " lump in " BOLD("%s") " couldn't be parsed.",
            lumpname, leafname(lumpinfo[lumpnum]->wadfile->path));
        return NULL;
    }

    if (!(out = calloc(1, sizeof(*out))))
        return NULL;

    out->musiclump = M_StringDuplicate(music->valuestring);
    out->backgroundlump = M_StringDuplicate(backgroundimage->valuestring);

    js_layers = cJSON_GetObjectItemCaseSensitive(data, "layers");

    cJSON_ArrayForEach(js_layer, js_layers)
    {
        interlevellayer_t   layer = { 0 };

        ParseLevelLayer(js_layer, &layer);
        array_push(layers, layer);
    }

    out->layers = layers;

    cJSON_Delete(json);
    return out;
}

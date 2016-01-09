#include <stdio.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

typedef struct music_control
{
    double volume;
    char *file;
    GstElement *pipeline;
}music_control;

music_control ctr = {5, "/root/workspace/exercise/languages/C/game/tetris/Korobeiniki.ogg", NULL};
/* music_control ctr = {5, "/usr/share/iptux/sound/msg.ogg", NULL}; */

void NewDecodedPad(GstElement * volume, GstPad * pad)
{
    GstCaps *caps;
    GstStructure *str;
    GstPad *spad;

    caps = gst_pad_get_caps(pad);
    str = gst_caps_get_structure(caps, 0);
    if (strcasestr(gst_structure_get_name(str), "audio") && (spad = gst_element_get_compatible_pad(volume, pad, caps)))
        gst_pad_link(pad, spad);
    gst_caps_unref(caps);
}

void play_background_music(void)
{
    GstElement *filesrc, *decode, *volume, *convert, *sink;
    GstBus *bus;

    gst_init(NULL, NULL);

    ctr.pipeline = gst_pipeline_new("sound-system");

    filesrc = gst_element_factory_make("filesrc", "source");
    decode = gst_element_factory_make("decodebin", "decode");
    volume = gst_element_factory_make("volume", "volume");
    convert = gst_element_factory_make("audioconvert", "convert");
    sink = gst_element_factory_make("autoaudiosink", "output");

    gst_bin_add_many(GST_BIN(ctr.pipeline), filesrc, decode, volume, convert, sink, NULL);
    gst_element_link_many(filesrc, decode, NULL);
    gst_element_link_many(volume, convert, sink, NULL);
    g_signal_connect_swapped(decode, "new-decoded-pad", G_CALLBACK(NewDecodedPad), volume);

    bus = gst_pipeline_get_bus(GST_PIPELINE(ctr.pipeline));
    gst_bus_add_signal_watch(GST_BUS(bus));
    /* g_signal_connect_swapped(bus, "message::error", G_CALLBACK(ErrorMessageOccur), this); */
    /* g_signal_connect_swapped(bus, "message::eos", G_CALLBACK(EosMessageOccur), this); */
    gst_object_unref(bus);

    g_object_set(volume, "volume", ctr.volume, NULL);
    g_object_set(filesrc, "location", ctr.file, NULL);
    gst_element_set_state(ctr.pipeline, GST_STATE_PLAYING);

    /* gst_element_set_state(ctr.pipeline, GST_STATE_NULL); */
    /* gst_object_unref(GST_OBJECT(ctr.pipeline)); */
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    play_background_music();
    gtk_main();

    return 0;
}


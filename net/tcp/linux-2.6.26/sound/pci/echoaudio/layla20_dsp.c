/****************************************************************************

   Copyright Echo Digital Audio Corporation (c) 1998 - 2004
   All rights reserved
   www.echoaudio.com

   This file is part of Echo Digital Audio's generic driver library.

   Echo Digital Audio's generic driver library is free software;
   you can redistribute it and/or modify it under the terms of
   the GNU General Public License as published by the Free Software
   Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA  02111-1307, USA.

   *************************************************************************

 Translation from C++ and adaptation for use in ALSA-Driver
 were made by Giuliano Pochini <pochini@shiny.it>

****************************************************************************/


static int read_dsp(struct echoaudio *chip, u32 *data);
static int set_professional_spdif(struct echoaudio *chip, char prof);
static int load_asic_generic(struct echoaudio *chip, u32 cmd,
                 const struct firmware *asic);
static int check_asic_status(struct echoaudio *chip);
static int update_flags(struct echoaudio *chip);


static int init_hw(struct echoaudio *chip, u16 device_id, u16 subdevice_id)
{
    int err;

    DE_INIT(("init_hw() - Layla20\n"));
    snd_assert((subdevice_id & 0xfff0) == LAYLA20, return -ENODEV);

    if ((err = init_dsp_comm_page(chip))) {
        DE_INIT(("init_hw - could not initialize DSP comm page\n"));
        return err;
    }

    chip->device_id = device_id;
    chip->subdevice_id = subdevice_id;
    chip->bad_board = TRUE;
    chip->has_midi = TRUE;
    chip->dsp_code_to_load = &card_fw[FW_LAYLA20_DSP];
    chip->input_clock_types =
        ECHO_CLOCK_BIT_INTERNAL | ECHO_CLOCK_BIT_SPDIF |
        ECHO_CLOCK_BIT_WORD | ECHO_CLOCK_BIT_SUPER;
    chip->output_clock_types =
        ECHO_CLOCK_BIT_WORD | ECHO_CLOCK_BIT_SUPER;

    if ((err = load_firmware(chip)) < 0)
        return err;
    chip->bad_board = FALSE;

    if ((err = init_line_levels(chip)) < 0)
        return err;

    err = set_professional_spdif(chip, TRUE);

    DE_INIT(("init_hw done\n"));
    return err;
}



static u32 detect_input_clocks(const struct echoaudio *chip)
{
    u32 clocks_from_dsp, clock_bits;

    /* Map the DSP clock detect bits to the generic driver clock detect bits */
    clocks_from_dsp = le32_to_cpu(chip->comm_page->status_clocks);

    clock_bits = ECHO_CLOCK_BIT_INTERNAL;

    if (clocks_from_dsp & GLDM_CLOCK_DETECT_BIT_SPDIF)
        clock_bits |= ECHO_CLOCK_BIT_SPDIF;

    if (clocks_from_dsp & GLDM_CLOCK_DETECT_BIT_WORD) {
        if (clocks_from_dsp & GLDM_CLOCK_DETECT_BIT_SUPER)
            clock_bits |= ECHO_CLOCK_BIT_SUPER;
        else
            clock_bits |= ECHO_CLOCK_BIT_WORD;
    }

    return clock_bits;
}



/* ASIC status check - some cards have one or two ASICs that need to be
loaded.  Once that load is complete, this function is called to see if
the load was successful.
If this load fails, it does not necessarily mean that the hardware is
defective - the external box may be disconnected or turned off.
This routine sometimes fails for Layla20; for Layla20, the loop runs
5 times and succeeds if it wins on three of the loops. */
static int check_asic_status(struct echoaudio *chip)
{
    u32 asic_status;
    int goodcnt, i;

    chip->asic_loaded = FALSE;
    for (i = goodcnt = 0; i < 5; i++) {
        send_vector(chip, DSP_VC_TEST_ASIC);

        /* The DSP will return a value to indicate whether or not
           the ASIC is currently loaded */
        if (read_dsp(chip, &asic_status) < 0) {
            DE_ACT(("check_asic_status: failed on read_dsp\n"));
            return -EIO;
        }

        if (asic_status == ASIC_ALREADY_LOADED) {
            if (++goodcnt == 3) {
                chip->asic_loaded = TRUE;
                return 0;
            }
        }
    }
    return -EIO;
}



/* Layla20 has an ASIC in the external box */
static int load_asic(struct echoaudio *chip)
{
    int err;

    if (chip->asic_loaded)
        return 0;

    err = load_asic_generic(chip, DSP_FNC_LOAD_LAYLA_ASIC,
                &card_fw[FW_LAYLA20_ASIC]);
    if (err < 0)
        return err;

    /* Check if ASIC is alive and well. */
    return check_asic_status(chip);
}



static int set_sample_rate(struct echoaudio *chip, u32 rate)
{
    snd_assert(rate >= 8000 && rate <= 50000, return -EINVAL);

    /* Only set the clock for internal mode. Do not return failure,
       simply treat it as a non-event. */
    if (chip->input_clock != ECHO_CLOCK_INTERNAL) {
        DE_ACT(("set_sample_rate: Cannot set sample rate - "
            "clock not set to CLK_CLOCKININTERNAL\n"));
        chip->comm_page->sample_rate = cpu_to_le32(rate);
        chip->sample_rate = rate;
        return 0;
    }

    if (wait_handshake(chip))
        return -EIO;

    DE_ACT(("set_sample_rate(%d)\n", rate));
    chip->sample_rate = rate;
    chip->comm_page->sample_rate = cpu_to_le32(rate);
    clear_handshake(chip);
    return send_vector(chip, DSP_VC_SET_LAYLA_SAMPLE_RATE);
}



static int set_input_clock(struct echoaudio *chip, u16 clock_source)
{
    u16 clock;
    u32 rate;

    DE_ACT(("set_input_clock:\n"));
    rate = 0;
    switch (clock_source) {
    case ECHO_CLOCK_INTERNAL:
        DE_ACT(("Set Layla20 clock to INTERNAL\n"));
        rate = chip->sample_rate;
        clock = LAYLA20_CLOCK_INTERNAL;
        break;
    case ECHO_CLOCK_SPDIF:
        DE_ACT(("Set Layla20 clock to SPDIF\n"));
        clock = LAYLA20_CLOCK_SPDIF;
        break;
    case ECHO_CLOCK_WORD:
        DE_ACT(("Set Layla20 clock to WORD\n"));
        clock = LAYLA20_CLOCK_WORD;
        break;
    case ECHO_CLOCK_SUPER:
        DE_ACT(("Set Layla20 clock to SUPER\n"));
        clock = LAYLA20_CLOCK_SUPER;
        break;
    default:
        DE_ACT(("Input clock 0x%x not supported for Layla24\n",
            clock_source));
        return -EINVAL;
    }
    chip->input_clock = clock_source;

    chip->comm_page->input_clock = cpu_to_le16(clock);
    clear_handshake(chip);
    send_vector(chip, DSP_VC_UPDATE_CLOCKS);

    if (rate)
        set_sample_rate(chip, rate);

    return 0;
}



static int set_output_clock(struct echoaudio *chip, u16 clock)
{
    DE_ACT(("set_output_clock: %d\n", clock));
    switch (clock) {
    case ECHO_CLOCK_SUPER:
        clock = LAYLA20_OUTPUT_CLOCK_SUPER;
        break;
    case ECHO_CLOCK_WORD:
        clock = LAYLA20_OUTPUT_CLOCK_WORD;
        break;
    default:
        DE_ACT(("set_output_clock wrong clock\n"));
        return -EINVAL;
    }

    if (wait_handshake(chip))
        return -EIO;

    chip->comm_page->output_clock = cpu_to_le16(clock);
    chip->output_clock = clock;
    clear_handshake(chip);
    return send_vector(chip, DSP_VC_UPDATE_CLOCKS);
}



/* Set input bus gain (one unit is 0.5dB !) */
static int set_input_gain(struct echoaudio *chip, u16 input, int gain)
{
    snd_assert(input < num_busses_in(chip), return -EINVAL);

    if (wait_handshake(chip))
        return -EIO;

    chip->input_gain[input] = gain;
    gain += GL20_INPUT_GAIN_MAGIC_NUMBER;
    chip->comm_page->line_in_level[input] = gain;
    return 0;
}



/* Tell the DSP to reread the flags from the comm page */
static int update_flags(struct echoaudio *chip)
{
    if (wait_handshake(chip))
        return -EIO;
    clear_handshake(chip);
    return send_vector(chip, DSP_VC_UPDATE_FLAGS);
}



static int set_professional_spdif(struct echoaudio *chip, char prof)
{
    DE_ACT(("set_professional_spdif %d\n", prof));
    if (prof)
        chip->comm_page->flags |=
            __constant_cpu_to_le32(DSP_FLAG_PROFESSIONAL_SPDIF);
    else
        chip->comm_page->flags &=
            ~__constant_cpu_to_le32(DSP_FLAG_PROFESSIONAL_SPDIF);
    chip->professional_spdif = prof;
    return update_flags(chip);
}

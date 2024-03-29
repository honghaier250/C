/*
 *  linux/arch/arm/mach-integrator/clock.h
 *
 *  Copyright (C) 2004 ARM Limited.
 *  Written by Deep Blue Solutions Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
struct module;
struct icst525_params;

struct clk {
    struct list_head    node;
    unsigned long        rate;
    struct module        *owner;
    const char        *name;
    const struct icst525_params *params;
    void            *data;
    void            (*setvco)(struct clk *, struct icst525_vco vco);
};

int clk_register(struct clk *clk);
void clk_unregister(struct clk *clk);

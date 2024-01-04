#include <assert.h>
#include <z80.h>
#include <strings.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include "ef9345rom.h"
#include "ef9345.h"
#include "emulator.h"

Z80Context z80_ctx;
byte memory[0x10000];
byte vram[0x4000];
byte ioport[0x100];
struct ef9345 *ef_ctx;

uint32_t palette[8] = {
    0x000000, // Black
    0xFF0000, // Red
    0x00FF00, // Green
    0xFFFF00, // Yellow
    0x0000FF, // Blue
    0xFF00FF, // Violet
    0x00FFFF, // Cyan
    0xFFFFFF  // White
};

void emulator_destroy()
{
	if(ef_ctx)
		ef9345_free(ef_ctx);
}

byte cpu_memory_read(size_t param, ushort address)
{
    //TODO: Gestion des extensions mémoires
	if(address >= 0x8000)
        return 0;

    return memory[address];
}

void cpu_memory_write(size_t param, ushort address, byte data)
{
	//TODO: Gestion des extensions mémoires
	if(address >= 0x8000)
		return;

    memory[address] = data;
}

byte cpu_io_read(size_t param, ushort address)
{
    address &= 0xFF;

    switch(address) {
        case 0xcf:
            return (byte) ef9345_read(ef_ctx, ioport[0x8f]);
        
        case 0x8f: //TODO: Vérifier si c'est conforme
            return ioport[0x8f];

        case 0x80: case 0x81: case 0x82:
        case 0x83: case 0x84: case 0x85:
        case 0x86: case 0x87: case 0x88:
            return ioport[address];

        default:
            printf("[IO port] Tried to read on unknown port %4x\n", address);
            break;
    }
    return 0;
}

void cpu_io_write(size_t param, ushort address, byte data)
{
    address &= 0xFF;
    ioport[address] = data;
    switch(address) {
	    case 0xcf:
            ef9345_write(ef_ctx, ioport[0x8f], data);
            return;
        case 0x8f: return;
	    default:
            printf("[IO port] Tried to write 0x%2x on unknown port %4x\n", data, address);
            return; 
    }
}


void emulator_init(int argc, char* argv[])
{
    int i;
    FILE *rom;
    size_t rom_file_length;
    bool patch_rom = false;

    z80_ctx.memRead = cpu_memory_read;
    z80_ctx.memWrite = cpu_memory_write;

    z80_ctx.ioRead = cpu_io_read;
    z80_ctx.ioWrite = cpu_io_write;

    bzero(memory, 0x10000 * sizeof(byte));
    bzero(ioport, 0x100 * sizeof(byte));

    if(argc > 1) {
        rom = fopen(argv[1], "rb");
        assert(rom != NULL);

        fseek(rom, 0, SEEK_END);
        rom_file_length = ftell(rom);
        assert(rom_file_length == 0x4000);
        rewind(rom);

        assert(fread(memory, 0x4000, 1, rom));

        fclose(rom);
    }

    if(patch_rom) {
        memory[0x3aa9] = 0xed; memory[0x3aaa] = 0xfe;   //ecriture cassette
        memory[0x3af3] = 0xed; memory[0x3af4] = 0xfe;   //test vitesse k7
        memory[0x3b48] = 0xed; memory[0x3b49] = 0xfe;   //lecture octet cassette
        memory[0x1e00] = 0xaf; memory[0x1e01] = 0x1f;   //fin cassette Basicode
        memory[0x3a81] = 0xc9;                          //signaux de synchro
    }
    
    //initialisation ports
    for(i = 0x07; i < 0x09; i++)    //joysticks au repos
        ioport[i] = 0xff;
    for(i = 0x80; i < 0x88; i++)    //touches relachees
        ioport[i] = 0xff;

    Z80RESET(&z80_ctx);
    ef_ctx = ef9345_create(EF9345, vram, vrom, 0x3FFF);
    ef9345_set_colourmap(ef_ctx, palette);
    ef9345_reset(ef_ctx);
}

//TODO: Move it somewhere else
unsigned long get_seconds()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    return 1000000L * tv.tv_sec + tv.tv_usec;
}

gpointer emulator_run(gpointer data)
{
#ifdef DEBUG
    char buffer[1024];
#endif
	(void) data;
	

    unsigned long last_refresh_time, last_update_time, update_time;

    last_refresh_time = last_update_time = get_seconds();
    while(true) {
        update_time = get_seconds();

        int prev_pc;
        for(int i = 0; i < 80000; i ++) {
            prev_pc = z80_ctx.PC;
            Z80ExecuteTStates(&z80_ctx, 1);
            if(prev_pc == z80_ctx.PC)
                continue;
#ifdef DEBUG
            Z80Debug(&z80_ctx, NULL, buffer);
            printf("PC=%4x;AF=%4x;HL=%4x;BC=%4x;DE=%4x;Inst:%s\n",
                z80_ctx.PC,
                z80_ctx.R1.wr.AF,
                z80_ctx.R1.wr.HL,
                z80_ctx.R1.wr.BC,
                z80_ctx.R1.wr.DE,
                buffer
            );
#endif
        }

        Z80INT(&z80_ctx, 0x38);
        ef9345_cycles(ef_ctx, (update_time - last_update_time) * 1000L);

        // TODO: Un-hardcode this value
        // 16667 us = 1/(60 fps)
        if(last_refresh_time - update_time >= 16667) {
            emulator_refresh_screen(ef_ctx);
            last_refresh_time = update_time;
        }

        last_update_time = update_time;
    }

    return NULL;
}



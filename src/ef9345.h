/* 
 * license:GPL-2.0+
 * copyright-holders:Daniel Coulom,Sandro Ronco
 */
/*********************************************************************

    ef9345.h

    Thomson EF9345 video controller

*********************************************************************/

#ifndef _EF9345_H
#define _EF9345_H


/**************************************************************************
   TYPE DEFINITIONS
**************************************************************************/

#include <stdint.h>

#define	EF9345	0x01
#define TS9347	0x02

struct ef9345 {
	uint8_t *m_videoram;
	uint16_t vram_mask;
	/* Font ROM */
	uint8_t *m_charset;
	/* Busy flag */
	uint8_t m_bf;
	/* 40 or 80 chars for line */
	uint8_t m_char_mode;
	/* Accented characters */
	uint8_t m_acc_char[0x2000];
	/* Registers R0-R7 */
	uint8_t reg[8];
	/* Status register */
	uint8_t m_state;
	/* Indirect registers */
	uint8_t m_tgs,m_mat,m_pat,m_dor,m_ror;
	/* Border color */
	uint8_t m_border[80];
	/* Current memory block */
	uint16_t m_block;
	/* Index of ram charset */
	uint16_t m_ram_base[4];
	/* Cursor status */
	uint8_t m_blink;
	/* Last chars dial (for determinate the zoom position) */
	uint8_t m_last_dial[40];
	/* Background color latch */
	uint8_t m_latchc0;
	/* Hided attribute latch */
	uint8_t m_latchm;
	/* Insert attribute latch */
	uint8_t m_latchi;
	/* Underline attribute latch */
	uint8_t m_latchu;

	/* 336 for 40 col, 312 for 80 col */
	uint32_t raster[336][492];
	uint32_t *m_palette;

	unsigned m_variant;
	unsigned trace;
	/* In ns*/
	unsigned busy_ticks;
	unsigned long flash;
};

uint8_t ef9345_read(struct ef9345 *ef, uint8_t offset);
void ef9345_write(struct ef9345 *ef, uint8_t offset, uint8_t data);
void ef9345_reset(struct ef9345 *ef);
struct ef9345 *ef9345_create(unsigned variant, uint8_t *vram, uint8_t *vrom, uint16_t vram_mask);
void ef9345_free(struct ef9345 *ef);
void ef9345_trace(struct ef9345 *ef, unsigned trace);

void ef9345_set_colourmap(struct ef9345 *ef, uint32_t *cmap);
uint32_t *ef9345_get_raster(struct ef9345 *ef);
void ef9345_rasterize(struct ef9345 *ef);

void ef9345_cycles(struct ef9345 *ef, unsigned long nsec);

#endif	/* _EF9345_H */


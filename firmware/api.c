/*
 * Author: Xiangfu Liu <xiangfu@openmobilefree.net>
 * Bitcoin:	1CanaaniJzgps8EV6Sfmpb7T8RutpaeyFn
 *
 * This is free and unencumbered software released into the public domain.
 * For details see the UNLICENSE file at the root of the source tree.
 */

#include <stdint.h>
#include <stddef.h>

#include "minilibc.h"
#include "system_config.h"
#include "defines.h"
#include "miner.h"
#include "io.h"
#include "api.h"

#include "api_test_data.c"	/* The test data array */

static struct lm32_api *api = (struct lm32_api *)API_BASE;

static uint32_t g_asic_freq[3] = {200, 200, 200};
static uint32_t g_asic_freq_avg = 0;
static uint32_t g_freq_array[][6] = {
	{100, 1, 16, 1, 16, 4},
	{170, 1, 27, 1, 27, 4},
	{200, 1, 16, 1, 16, 2},
	{220, 1, 17, 1, 17, 2},
	{230, 1, 18, 1, 18, 2},
	{300, 1, 24, 1, 24, 2},
	{340, 1, 27, 1, 27, 2},
	{350, 1, 28, 1, 28, 2},
	{360, 1, 43, 3, 43, 1},
	{370, 1, 59, 4, 59, 1},
	{380, 1, 61, 4, 61, 1},
	{390, 1, 47, 3, 47, 1},
	{395, 1, 63, 4, 63, 1},
	{400, 1, 16, 1, 16, 1},
	{410, 1, 49, 3, 49, 1},
	{411, 1, 33, 1, 33, 2},
	{415, 1, 50, 3, 50, 1},
	{420, 1, 50, 3, 50, 1},
	{425, 1, 17, 1, 17, 1},
	{430, 1, 52, 3, 52, 1},
	{440, 1, 53, 3, 53, 1},
	{450, 1, 18, 1, 18, 1},
	{460, 1, 55, 3, 55, 1},
	{470, 1, 56, 3, 56, 1},
	{480, 1, 58, 3, 58, 1},
	{490, 1, 59, 3, 59, 1},
	{500, 1, 20, 1, 20, 1},
	{510, 1, 61, 3, 61, 1},
	{520, 1, 62, 3, 62, 1},
	{530, 1, 21, 1, 21, 1},
	{540, 1, 43, 2, 43, 1},
	{550, 1, 22, 1, 22, 1},
	{560, 1, 45, 2, 45, 1},
	{570, 1, 23, 1, 23, 1},
	{580, 1, 23, 1, 23, 1},
	{590, 1, 47, 2, 47, 1},
	{600, 1, 24, 1, 24, 1},
	{610, 1, 49, 2, 49, 1},
	{620, 1, 25, 1, 25, 1},
	{630, 1, 25, 1, 25, 1},
	{640, 1, 51, 2, 51, 1},
	{650, 1, 26, 1, 26, 1},
	{660, 1, 53, 2, 53, 1},
	{670, 1, 27, 1, 27, 1},
	{680, 1, 27, 1, 27, 1},
	{690, 1, 55, 2, 55, 1},
	{700, 1, 28, 1, 28, 1},
	{710, 1, 57, 2, 57, 1},
	{720, 1, 29, 1, 29, 1},
	{730, 1, 29, 1, 29, 1},
	{740, 1, 59, 2, 59, 1},
	{750, 1, 30, 1, 30, 1},
	{760, 1, 61, 2, 61, 1},
	{770, 1, 31, 1, 31, 1},
	{780, 1, 31, 1, 31, 1},
	{790, 1, 63, 2, 63, 1},
	{800, 1, 32, 1, 32, 1},
	{810, 1, 32, 1, 32, 1},
	{820, 1, 33, 1, 33, 1},
	{830, 1, 33, 1, 33, 1},
	{840, 1, 34, 1, 34, 1},
	{850, 1, 34, 1, 34, 1},
	{860, 1, 34, 1, 34, 1},
	{870, 1, 35, 1, 35, 1},
	{880, 1, 35, 1, 35, 1},
	{890, 1, 36, 1, 36, 1},
	{900, 1, 36, 1, 36, 1},
	{910, 1, 36, 1, 36, 1},
	{920, 1, 37, 1, 37, 1},
	{930, 1, 37, 1, 37, 1},
	{940, 1, 38, 1, 38, 1},
	{950, 1, 38, 1, 38, 1},
	{960, 1, 38, 1, 38, 1},
	{970, 1, 39, 1, 39, 1},
	{980, 1, 39, 1, 39, 1},
	{990, 1, 40, 1, 40, 1},
	{1000, 1, 40, 1, 40, 1}
};

static uint32_t api_set_cpm(uint32_t NR, uint32_t NF, uint32_t OD, uint32_t NB, uint32_t div)
{
	uint32_t div_loc = 0;
	uint32_t NR_sub;
	uint32_t NF_sub;
	uint32_t OD_sub;
	uint32_t NB_sub;

	NR_sub = NR - 1;
	NF_sub = NF - 1;
	OD_sub = OD - 1;
	NB_sub = NB - 1;

	if(div == 1  ) div_loc = 0;
	if(div == 2  ) div_loc = 1;
	if(div == 4  ) div_loc = 2;
	if(div == 8  ) div_loc = 3;
	if(div == 16 ) div_loc = 4;
	if(div == 32 ) div_loc = 5;
	if(div == 64 ) div_loc = 6;
	if(div == 128) div_loc = 7;

	return 0x7 | (div_loc << 7) | (1 << 10) |
		(NR_sub << 11) | (NF_sub << 15) | (OD_sub << 21) | (NB_sub << 25);
}

static inline void api_set_num(uint32_t ch_num, uint32_t chip_num)
{
	uint32_t tmp;
	tmp = readl(&api->sck) & 0xff;
	tmp = (ch_num << 16) | ((chip_num*23) << 24) | tmp;
	writel(tmp, &api->sck);
}

static inline void api_set_sck(uint32_t spi_speed)
{
	uint32_t tmp;
	tmp = (readl(&api->sck) & 0xffff0000) | spi_speed;
	writel(tmp, &api->sck);
}

static void api_set_tx_fifo(uint32_t * data)
{
	int i;
	for (i = 0; i < 23; i++)
		writel(data[i], &api->tx);
}

static inline void api_wait_done(uint32_t ch_num, uint32_t chip_num)
{
	while (api_get_rx_cnt() != (ch_num * chip_num * LM32_API_RET_LEN))
		;
}

static inline uint32_t api_gen_test_work(uint32_t i, uint32_t * data)
{
	uint32_t tmp = 0;
	int j;

	for (j = 0; j < 18; j++) {
		data[j] = test_data[i%16][j];
	}

	tmp = data[0];
        data[0] = data[0] ^ (i & 0xfffffff0);//nonce

	data[18] = 0x0;//nonce2
	data[19] = i;//nonce2
	data[20] = 0x1;//cpm2
	data[21] = 0x1;//cpm1
	data[22] = 0x1;//cpm0

	return tmp + 0x18000;
}

static uint32_t api_verify_nonce(uint32_t ch_num, uint32_t chip_num, uint32_t verify_on, uint32_t target_nonce, uint32_t result[MINER_COUNT][ASIC_COUNT])
{
	uint32_t i, j;
	uint32_t rx_data[LM32_API_RET_LEN];
	uint32_t pass_cal_num = 0;
	static uint32_t last_minerid = 0xff;
	static uint8_t chip_id;
	uint8_t channel_id;

	for (i = 0; i < ch_num; i++) {
		for (j = 0; j < chip_num; j++) {
			api_get_rx_fifo(rx_data);
			channel_id = rx_data[10] & 0xff;
			if (last_minerid != channel_id) {
				chip_id = 0;
				last_minerid = channel_id;
			} else
				chip_id++;

			if (verify_on && ((rx_data[2] == target_nonce) || (rx_data[3] == target_nonce))) {
				pass_cal_num++;
			} else {
				if (verify_on) {
#ifdef DEBUG_VERBOSE
					debug32("channel id: %d,chip id: %d, TN:%08x, RX[0]:%08x, RX[1]:%08x, RX[2]:%08x, RX[3]:%08x\n", channel_id, chip_id, target_nonce, rx_data[0], rx_data[1], rx_data[2], rx_data[3]);
#endif
					if (result)
					    result[channel_id][chip_id]++;
				}
			}
		}
	}
	return pass_cal_num;
}

extern void delay(uint32_t ms);
static inline void api_flush()
{
	writel(0x2, &api->state);
	delay(1);
}

static void api_change_cpm(uint32_t ch_num, uint32_t chip_num,
		    uint32_t NR0, uint32_t NF0, uint32_t OD0, uint32_t NB0, uint32_t div0,
		    uint32_t NR1, uint32_t NF1, uint32_t OD1, uint32_t NB1, uint32_t div1,
		    uint32_t NR2, uint32_t NF2, uint32_t OD2, uint32_t NB2, uint32_t div2)
{
	uint32_t tx_data[23];
	uint32_t i, k;

	/* random numbers, keep different with nonce2 */
	tx_data[18] = 0xa8bc6de9;
	tx_data[19] = 0x35416784;
	tx_data[20] = api_set_cpm(NR0, NF0, OD0, NB0, div0) | 0x40;
	tx_data[21] = api_set_cpm(NR1, NF1, OD1, NB1, div1) | 0x40;
	tx_data[22] = api_set_cpm(NR2, NF2, OD2, NB2, div2) | 0x40;
	for (k = 0; k < ch_num; k++){
		while((512 - api_get_tx_cnt()) < (chip_num * 23))
			;
		for(i = 0; i < chip_num; i++)
			api_set_tx_fifo(tx_data);
	}

	api_wait_done(ch_num, chip_num);

	api_verify_nonce(ch_num, chip_num, 0, 0, NULL);
	delay(1);
}

void api_initial(uint32_t ch_num, uint32_t chip_num, uint32_t spi_speed)
{
	api_set_num(ch_num, chip_num);
	api_set_sck(spi_speed);
}

void api_set_timeout(uint32_t timeout)
{
	writel(timeout, &api->timeout);
}

uint32_t api_get_tx_cnt()
{
	return (readl(&api->state) >> 2) & 0x3ff;
}

uint32_t api_get_rx_cnt()
{
	return (readl(&api->state) >> 20) & 0x3ff;
}

void api_get_rx_fifo(uint32_t * data)
{
	int i;
	for (i = 0; i < LM32_API_RET_LEN; i++)
		data[i] = readl(&api->rx);
}

/*
 * when you want to know the health of a3222(within three diff zone), you may use this func like this:
 * cal_core_num = 248;
 * add_step = 16;
 * pass_zone_num: [0] for the best 4/9 core; [1] for the mid 4/9 core; [2] for the worst 1/9;
 * freq: worst -> best;
 * api_asic_test(ch_num, chip_num, cal_core_num, add_step, pass_zone_num, freq);
 */
static uint32_t api_asic_test(uint32_t ch_num, uint32_t chip_num,
		       uint32_t cal_core_num, uint32_t add_step,
		       uint32_t *pass_zone_num, uint32_t freq[], uint32_t result[MINER_COUNT][ASIC_COUNT])
{
	uint32_t i, j, k;
	uint32_t tx_data[23];
	uint32_t target_nonce;
	uint32_t pass_cal_num = 0;
	uint32_t verify_on = 0;
	uint32_t tmp;

	if (pass_zone_num) {
		pass_zone_num[0] = 0;
		pass_zone_num[1] = 0;
		pass_zone_num[2] = 0;
	}

	set_asic_freq(freq);

	if (result) {
		api_set_timeout(0x10);
		api_flush();
		memset(result, 0, MINER_COUNT * ASIC_COUNT * sizeof(uint32_t));
	}

	for (j = 0; j < cal_core_num + 2 * add_step; j += add_step) {
		api_gen_test_work(j, tx_data);
		for (k = 0; k < ch_num; k++) {
			while ((512 - api_get_tx_cnt()) < (chip_num * 23))
				;
			for (i = 0; i < chip_num; i++)
				api_set_tx_fifo(tx_data);
		}

		api_wait_done(ch_num, chip_num);

		target_nonce = api_gen_test_work((j - 2 * add_step) % 16, tx_data);
		verify_on = j >= 2 ? 1 : 0;
		tmp = api_verify_nonce(ch_num, chip_num, verify_on, target_nonce, result);
		pass_cal_num += tmp;

		if (pass_zone_num && verify_on) {
			if ((j - 2) < (28 * 4 - 4) * 16)
				pass_zone_num[0] += tmp;
			else if ((j - 2) < (28 * 4 - 4 + 28 * 4) * 16)
				pass_zone_num[1] += tmp;
			else
				pass_zone_num[2] += tmp;
		}
	}

	return pass_cal_num;
}

int api_send_work(struct work *w)
{
#if 0
	int i, j;

	writel(0, &api->tx);

	i = 0;

	for (j = 1; j < 18; j++)
		writel(test_data[i%16][j], &api->tx);
#else
	uint32_t tmp;
	int i;

	writel(0, &api->tx);

	memcpy((uint8_t *)(&tmp), w->a2, 4);
	writel(tmp, &api->tx);

	for (i = 0; i <= 28; i += 4) {
		memcpy((uint8_t *)(&tmp), w->data + i, 4);
		writel(tmp, &api->tx);
	}

	memcpy((uint8_t *)(&tmp), w->e0, 4);
	writel(tmp, &api->tx);
	memcpy((uint8_t *)(&tmp), w->e1, 4);
	writel(tmp, &api->tx);
	memcpy((uint8_t *)(&tmp), w->e2, 4);
	writel(tmp, &api->tx);
	memcpy((uint8_t *)(&tmp), w->a0, 4);
	writel(tmp, &api->tx);
	memcpy((uint8_t *)(&tmp), w->a1, 4);
	writel(tmp, &api->tx);

	for (i = 0; i <= 8; i += 4) {
		memcpy((uint8_t *)(&tmp), w->data + 32 + i, 4);
		writel(tmp, &api->tx);
	}

	memcpy((uint8_t *)(&tmp), &w->memo, 4);
	writel(tmp, &api->tx);

	memcpy((uint8_t *)(&tmp), &w->nonce2, 4);
	writel(tmp, &api->tx);
#endif

	/* The chip configure information */
	writel(1, &api->tx);
	writel(1, &api->tx);
	writel(1, &api->tx);

	return 0;
}

void set_asic_freq(uint32_t value[])
{
	int i, j, freq_index[3];
	uint32_t max_freq = 0;

	for (i = 0; i < 3; i++)
		g_asic_freq[i] = value[i];

	max_freq = g_asic_freq[0];
	for (i = 1; i < 3; i++) {
		if (max_freq < g_asic_freq[i])
			max_freq = g_asic_freq[i];
	}

	/* The timeout value:
	 * 2^32÷(0.1GHz×1000000000×3968÷65)×100000000 = 0x4318c63 */
	api_set_timeout(ASIC_TIMEOUT_100M / max_freq * 50);
	api_flush();

	for (j = 0; j < 3; j++) {
		i = 0;
		while (g_freq_array[i][0] != 1000) {
			if (g_asic_freq[j] >= g_freq_array[i][0] && g_asic_freq[j] < g_freq_array[i+1][0])
				break;
			i++;
		}

		freq_index[j] = i;
	}

	api_change_cpm(MINER_COUNT, ASIC_COUNT,
		       g_freq_array[freq_index[0]][1], g_freq_array[freq_index[0]][2], g_freq_array[freq_index[0]][3],
		       g_freq_array[freq_index[0]][4], g_freq_array[freq_index[0]][5],

		       g_freq_array[freq_index[1]][1], g_freq_array[freq_index[1]][2], g_freq_array[freq_index[1]][3],
		       g_freq_array[freq_index[1]][4], g_freq_array[freq_index[1]][5],

		       g_freq_array[freq_index[2]][1], g_freq_array[freq_index[2]][2], g_freq_array[freq_index[2]][3],
		       g_freq_array[freq_index[2]][4], g_freq_array[freq_index[2]][5]);

	g_asic_freq_avg = (g_asic_freq[0] + g_asic_freq[1] * 4 + g_asic_freq[2] * 4) / 9;
}

uint32_t get_asic_freq()
{
	return g_asic_freq_avg;
}

extern uint32_t send_pkg(int type, uint8_t *buf, uint32_t len, int block);
int api_asic_testcores(uint32_t cal_core_num, uint32_t freq[], uint32_t ret)
{
	int i = 0, j = 0;
	uint8_t txdat[20];
	uint32_t result[MINER_COUNT][ASIC_COUNT];
	uint32_t tmp;
	uint32_t all = MINER_COUNT * ASIC_COUNT * cal_core_num;

	tmp = api_asic_test(MINER_COUNT, ASIC_COUNT, cal_core_num, 1, NULL, freq, result);

	for (i = 0; i < MINER_COUNT; i++) {
		txdat[0] = i;
		debug32("%d: ", i);
		for (j = 0; j < ASIC_COUNT; j++) {
			txdat[1 + j * ASIC_COUNT] = (result[i][j] >> 24) & 0xff;
			txdat[2 + j * ASIC_COUNT] = (result[i][j] >> 16) & 0xff;
			txdat[3 + j * ASIC_COUNT] = (result[i][j] >> 8) & 0xff;
			txdat[4 + j * ASIC_COUNT] = result[i][j] & 0xff;
			debug32("%d ", result[i][j]);
		}
		debug32("\n");
		if (ret)
			send_pkg(AVA2_P_TEST_RET, txdat, ASIC_COUNT * 4 + 1, 0);
	}
	if (ret) {
		txdat[0] = (tmp >> 24) & 0xff;
		txdat[1] = (tmp >> 16) & 0xff;
		txdat[2] = (tmp >> 8) & 0xff;
		txdat[3] = tmp & 0xff;
		txdat[4] = (all >> 24) & 0xff;
		txdat[5] = (all >> 16) & 0xff;
		txdat[6] = (all >> 8) & 0xff;
		txdat[7] = all & 0xff;
		send_pkg(AVA2_P_TEST_RET, txdat, 8, 0);
	}

	debug32("E/A: %d/%d\n", all - tmp, all);
	return all - tmp;
}


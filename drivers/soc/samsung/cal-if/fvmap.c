#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <soc/samsung/cal-if.h>

#include "fvmap.h"
#include "cmucal.h"
#include "vclk.h"
#include "ra.h"

#define FVMAP_SIZE		(SZ_8K)
#define STEP_UV			(6250)
#define step			(6250)
void __iomem *fvmap_base;
void __iomem *sram_fvmap_base;

static int init_margin_table[MAX_MARGIN_ID] = {0};
static int volt_offset_percent = 0;

static int __init get_all_volt(char *str)
{
	int i, volt;

	get_option(&str, &volt);

	for (i = MARGIN_MIF; i <= MARGIN_G3D; i++) {
		if (init_margin_table[i] == 0)
			init_margin_table[i] = volt;
	}

	return 0;
}
early_param("all", get_all_volt);

static int __init get_mif_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_MIF] = volt;

	return 0;
}
early_param("mif", get_mif_volt);

static int __init get_int_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INT] = volt;

	return 0;
}
early_param("int", get_int_volt);

static int __init get_big_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_BIG] = volt;

	return 0;
}
early_param("big", get_big_volt);

static int __init get_lit_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_LIT] = volt;

	return 0;
}
early_param("lit", get_lit_volt);

static int __init get_g3d_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3D] = volt;

	return 0;
}
early_param("g3d", get_g3d_volt);

static int __init get_intcam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_INTCAM] = volt;

	return 0;
}
early_param("intcam", get_intcam_volt);

static int __init get_cam_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CAM] = volt;

	return 0;
}
early_param("cam", get_cam_volt);

static int __init get_disp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_DISP] = volt;

	return 0;
}
early_param("disp", get_disp_volt);

static int __init get_g3dm_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_G3DM] = volt;

	return 0;
}
early_param("g3dm", get_g3dm_volt);

static int __init get_cp_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_CP] = volt;

	return 0;
}
early_param("cp", get_cp_volt);

static int __init get_fsys0_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_FSYS0] = volt;

	return 0;
}
early_param("fsys0", get_fsys0_volt);

static int __init get_aud_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_AUD] = volt;

	return 0;
}
early_param("aud", get_aud_volt);

static int __init get_iva_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_IVA] = volt;

	return 0;
}
early_param("iva", get_iva_volt);

static int __init get_score_volt(char *str)
{
	int volt;

	get_option(&str, &volt);
	init_margin_table[MARGIN_SCORE] = volt;

	return 0;
}
early_param("score", get_score_volt);

static int __init get_percent_margin_volt(char *str)
{
	int percent;

	get_option(&str, &percent);
	volt_offset_percent = percent;

	return 0;
}
early_param("volt_offset_percent", get_percent_margin_volt);

int fvmap_set_raw_voltage_table(unsigned int id, int uV)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int num_of_lv;
	int idx, i;

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		fv_table->table[i].volt += uV;

	return 0;
}

int fvmap_get_voltage_table(unsigned int id, unsigned int *table)
{
	struct fvmap_header *fvmap_header = fvmap_base;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;

	if (!IS_ACPM_VCLK(id))
		return 0;

	idx = GET_IDX(id);

	fvmap_header = fvmap_base;
	fv_table = fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
	{	if(num_of_lv==18){
			if(fv_table->table[0].volt==1000000)//2002 not work
				fv_table->table[0].volt=1193750;
			if(fv_table->table[1].volt==1000000)//1898
				fv_table->table[1].volt=1193750;
			if(fv_table->table[2].volt==1000000)//1794
				fv_table->table[2].volt=1043750;
			//if(fv_table->table[3].volt==1000000)
				//fv_table->table[3].volt=1043750;
		}
		if(num_of_lv==17){
			if(fv_table->table[0].volt==2496000)
				fv_table->table[0].volt=1053000;//1143750
			if(fv_table->table[1].volt==2392000)//not work
				fv_table->table[1].volt=1053000;//1143750
		}
		table[i] = fv_table->table[i].volt;
		pr_info("  num_of_lv : %u, table[i].volt : %u,Topser99_voltage_table\n",
				fv_table->table[i].volt,num_of_lv);
	}
	return num_of_lv;

}

int fvmap_get_raw_voltage_table(unsigned int id)
{
	struct fvmap_header *fvmap_header;
	struct rate_volt_header *fv_table;
	int idx, i;
	int num_of_lv;
	unsigned int table[20];

	idx = GET_IDX(id);

	fvmap_header = sram_fvmap_base;
	fv_table = sram_fvmap_base + fvmap_header[idx].o_ratevolt;
	num_of_lv = fvmap_header[idx].num_of_lv;

	for (i = 0; i < num_of_lv; i++)
		table[i] = fv_table->table[i].volt;

	for (i = 0; i < num_of_lv; i++)
		printk("dvfs id : %d  %d Khz : %d uv\n", ACPM_VCLK_TYPE | id, fv_table->table[i].rate, table[i]);

	return 0;
}

static void check_percent_margin(struct rate_volt_header *head, unsigned int num_of_lv)
{
	int org_volt;
	int percent_volt;
	int i;

	if (!volt_offset_percent)
		return;

	for (i = 0; i < num_of_lv; i++) {
		org_volt = head->table[i].volt;
		percent_volt = org_volt * volt_offset_percent / 100;
		head->table[i].volt = org_volt + rounddown(percent_volt, STEP_UV);
	}
}

static void fvmap_copy_from_sram(void __iomem *map_base, void __iomem *sram_base)
{
	struct fvmap_header *fvmap_header, *header;
	struct rate_volt_header *old, *new;
	struct clocks *clks;
	struct pll_header *plls;
	struct vclk *vclk;
	struct cmucal_clk *clk_node;
	unsigned int paddr_offset, fvaddr_offset;
	int size, margin;
	int i, j;

	fvmap_header = map_base;
	header = sram_base;

	size = cmucal_get_list_size(ACPM_VCLK_TYPE);

	for (i = 0; i < size; i++) {
		/* load fvmap info */
		fvmap_header[i].dvfs_type = header[i].dvfs_type;
		fvmap_header[i].num_of_lv = header[i].num_of_lv;
		fvmap_header[i].num_of_members = header[i].num_of_members;
		fvmap_header[i].num_of_pll = header[i].num_of_pll;
		fvmap_header[i].num_of_mux = header[i].num_of_mux;
		fvmap_header[i].num_of_div = header[i].num_of_div;
		fvmap_header[i].gearratio = header[i].gearratio;
		fvmap_header[i].init_lv = header[i].init_lv;
		fvmap_header[i].num_of_gate = header[i].num_of_gate;
		fvmap_header[i].reserved[0] = header[i].reserved[0];
		fvmap_header[i].reserved[1] = header[i].reserved[1];
		fvmap_header[i].block_addr[0] = header[i].block_addr[0];
		fvmap_header[i].block_addr[1] = header[i].block_addr[1];
		fvmap_header[i].block_addr[2] = header[i].block_addr[2];
		fvmap_header[i].o_members = header[i].o_members;
		fvmap_header[i].o_ratevolt = header[i].o_ratevolt;
		fvmap_header[i].o_tables = header[i].o_tables;

		vclk = cmucal_get_node(ACPM_VCLK_TYPE | i);
		if (vclk == NULL)
			continue;
		pr_info("dvfs_type : %s - id : %x\n",
			vclk->name, fvmap_header[i].dvfs_type);
		pr_info("  num_of_lv      : %d\n", fvmap_header[i].num_of_lv);
		pr_info("  num_of_members : %d\n", fvmap_header[i].num_of_members);

		old = sram_base + fvmap_header[i].o_ratevolt;
		new = map_base + fvmap_header[i].o_ratevolt;

		check_percent_margin(old, fvmap_header[i].num_of_lv);

		margin = init_margin_table[vclk->margin_id];
		if (margin)
		  cal_dfs_set_volt_margin(i | ACPM_VCLK_TYPE, margin);
/*<6>[    0.871658]  [0:      swapper/0:    1] fvmap_init:fvmap initialize 0000000000000000
<6>[    0.871671]  [0:      swapper/0:    1] dvfs_type : dvfs_mif - id : 0
<6>[    0.871679]  [0:      swapper/0:    1]   num_of_lv      : 10
<6>[    0.871688]  [0:      swapper/0:    1]   num_of_members : 10
<6>[    0.871699]  [0:      swapper/0:    1]   lv : [2093000], volt = 787500 uV (0 %) 
<6>[    0.871710]  [0:      swapper/0:    1]   lv : [2002000], volt = 775000 uV (0 %) 
<6>[    0.871720]  [0:      swapper/0:    1]   lv : [1794000], volt = 762500 uV (0 %) 
<6>[    0.871731]  [0:      swapper/0:    1]   lv : [1539000], volt = 725000 uV (0 %) 
<6>[    0.871741]  [0:      swapper/0:    1]   lv : [1352000], volt = 712500 uV (0 %) 
<6>[    0.871752]  [0:      swapper/0:    1]   lv : [1014000], volt = 662500 uV (0 %) 
<6>[    0.871762]  [0:      swapper/0:    1]   lv : [ 845000], volt = 637500 uV (0 %) 
<6>[    0.871773]  [0:      swapper/0:    1]   lv : [ 676000], volt = 637500 uV (0 %) 
<6>[    0.871783]  [0:      swapper/0:    1]   lv : [ 546000], volt = 637500 uV (0 %) 
<6>[    0.871794]  [0:      swapper/0:    1]   lv : [ 419000], volt = 637500 uV (0 %) 
<6>[    0.871807]  [0:      swapper/0:    1] dvfs_type : dvfs_int - id : 1
<6>[    0.871815]  [0:      swapper/0:    1]   num_of_lv      : 5
<6>[    0.871823]  [0:      swapper/0:    1]   num_of_members : 10
<6>[    0.871834]  [0:      swapper/0:    1]   lv : [ 667000], volt = 812500 uV (0 %) 
<6>[    0.871844]  [0:      swapper/0:    1]   lv : [ 533000], volt = 743750 uV (0 %) 
<6>[    0.871855]  [0:      swapper/0:    1]   lv : [ 400000], volt = 681250 uV (0 %) 
<6>[    0.871865]  [0:      swapper/0:    1]   lv : [ 200000], volt = 593750 uV (0 %) 
<6>[    0.871875]  [0:      swapper/0:    1]   lv : [ 100000], volt = 587500 uV (0 %) 
<6>[    0.871887]  [0:      swapper/0:    1] dvfs_type : dvfs_cpucl0 - id : 2
<6>[    0.871895]  [0:      swapper/0:    1]   num_of_lv      : 16
<6>[    0.871904]  [0:      swapper/0:    1]   num_of_members : 2
<6>[    0.871914]  [0:      swapper/0:    1]   lv : [2054000], volt = 1300000 uV (0 %) 
<6>[    0.871924]  [0:      swapper/0:    1]   lv : [1950000], volt = 1256250 uV (0 %) 
<6>[    0.871935]  [0:      swapper/0:    1]   lv : [1846000], volt = 1156250 uV (0 %) 
<6>[    0.871945]  [0:      swapper/0:    1]   lv : [1742000], volt = 1093750 uV (0 %) 
<6>[    0.871955]  [0:      swapper/0:    1]   lv : [1638000], volt = 1037500 uV (0 %) 
<6>[    0.871966]  [0:      swapper/0:    1]   lv : [1534000], volt = 981250 uV (0 %) 
<6>[    0.871976]  [0:      swapper/0:    1]   lv : [1456000], volt = 937500 uV (0 %) 
<6>[    0.871986]  [0:      swapper/0:    1]   lv : [1326000], volt = 887500 uV (0 %) 
<6>[    0.871996]  [0:      swapper/0:    1]   lv : [1222000], volt = 850000 uV (0 %) 
<6>[    0.872007]  [0:      swapper/0:    1]   lv : [1118000], volt = 812500 uV (0 %) 
<6>[    0.872017]  [0:      swapper/0:    1]   lv : [1053000], volt = 793750 uV (0 %) 
<6>[    0.872027]  [0:      swapper/0:    1]   lv : [ 910000], volt = 750000 uV (0 %) 
<6>[    0.872037]  [0:      swapper/0:    1]   lv : [ 806000], volt = 718750 uV (0 %) 
<6>[    0.872047]  [0:      swapper/0:    1]   lv : [ 702000], volt = 687500 uV (0 %) 
<6>[    0.872058]  [0:      swapper/0:    1]   lv : [ 598000], volt = 656250 uV (0 %) 
<6>[    0.872068]  [0:      swapper/0:    1]   lv : [ 403000], volt = 600000 uV (0 %) 
<6>[    0.872080]  [0:      swapper/0:    1] dvfs_type : dvfs_cpucl1 - id : 3
<6>[    0.872088]  [0:      swapper/0:    1]   num_of_lv      : 20
<6>[    0.872097]  [0:      swapper/0:    1]   num_of_members : 2
<6>[    0.872107]  [0:      swapper/0:    1]   lv : [2704000], volt = 1225000 uV (0 %) 
<6>[    0.872118]  [0:      swapper/0:    1]   lv : [2600000], volt = 1225000 uV (0 %) 
<6>[    0.872128]  [0:      swapper/0:    1]   lv : [2496000], volt = 1225000 uV (0 %) 
<6>[    0.872138]  [0:      swapper/0:    1]   lv : [2314000], volt = 1225000 uV (0 %) 
<6>[    0.872149]  [0:      swapper/0:    1]   lv : [2210000], volt = 1175000 uV (0 %) 
<6>[    0.872159]  [0:      swapper/0:    1]   lv : [2184000], volt = 1131250 uV (0 %) 
<6>[    0.872170]  [0:      swapper/0:    1]   lv : [2080000], volt = 1087500 uV (0 %) 
<6>[    0.872180]  [0:      swapper/0:    1]   lv : [1976000], volt = 1031250 uV (0 %) 
<6>[    0.872190]  [0:      swapper/0:    1]   lv : [1898000], volt = 987500 uV (0 %) 
<6>[    0.872201]  [0:      swapper/0:    1]   lv : [1768000], volt = 943750 uV (0 %) 
<6>[    0.872211]  [0:      swapper/0:    1]   lv : [1664000], volt = 900000 uV (0 %) 
<6>[    0.872222]  [0:      swapper/0:    1]   lv : [1508000], volt = 831250 uV (0 %) 
<6>[    0.872232]  [0:      swapper/0:    1]   lv : [1456000], volt = 812500 uV (0 %) 
<6>[    0.872243]  [0:      swapper/0:    1]   lv : [1352000], volt = 781250 uV (0 %) 
<6>[    0.872253]  [0:      swapper/0:    1]   lv : [1248000], volt = 750000 uV (0 %) 
<6>[    0.872263]  [0:      swapper/0:    1]   lv : [1144000], volt = 725000 uV (0 %) 
<6>[    0.872274]  [0:      swapper/0:    1]   lv : [1040000], volt = 706250 uV (0 %) 
<6>[    0.872284]  [0:      swapper/0:    1]   lv : [ 936000], volt = 687500 uV (0 %) 
<6>[    0.872294]  [0:      swapper/0:    1]   lv : [ 832000], volt = 668750 uV (0 %) 
<6>[    0.872304]  [0:      swapper/0:    1]   lv : [ 728000], volt = 643750 uV (0 %) 
<6>[    0.872316]  [0:      swapper/0:    1] dvfs_type : dvfs_g3d - id : 4
<6>[    0.872324]  [0:      swapper/0:    1]   num_of_lv      : 11
<6>[    0.872333]  [0:      swapper/0:    1]   num_of_members : 1
<6>[    0.872343]  [0:      swapper/0:    1]   lv : [1053000], volt = 993750 uV (0 %) 
<6>[    0.872354]  [0:      swapper/0:    1]   lv : [ 949000], volt = 900000 uV (0 %) 
<6>[    0.872364]  [0:      swapper/0:    1]   lv : [ 839000], volt = 818750 uV (0 %) 
<6>[    0.872375]  [0:      swapper/0:    1]   lv : [ 764000], volt = 775000 uV (0 %) 
<6>[    0.872385]  [0:      swapper/0:    1]   lv : [ 683000], volt = 737500 uV (0 %) 
<6>[    0.872396]  [0:      swapper/0:    1]   lv : [ 572000], volt = 687500 uV (0 %) 
<6>[    0.872406]  [0:      swapper/0:    1]   lv : [ 546000], volt = 681250 uV (0 %) 
<6>[    0.872416]  [0:      swapper/0:    1]   lv : [ 455000], volt = 643750 uV (0 %) 
<6>[    0.872426]  [0:      swapper/0:    1]   lv : [ 385000], volt = 618750 uV (0 %) 
<6>[    0.872437]  [0:      swapper/0:    1]   lv : [ 338000], volt = 606250 uV (0 %) 
<6>[    0.872448]  [0:      swapper/0:    1]   lv : [ 260000], volt = 581250 uV (0 %) 
<6>[    0.872460]  [0:      swapper/0:    1] dvfs_type : dvfs_intcam - id : 5
<6>[    0.872468]  [0:      swapper/0:    1]   num_of_lv      : 5
<6>[    0.872476]  [0:      swapper/0:    1]   num_of_members : 6
<6>[    0.872487]  [0:      swapper/0:    1]   lv : [ 690000], volt = 843750 uV (0 %) 
<6>[    0.872497]  [0:      swapper/0:    1]   lv : [ 680000], volt = 712500 uV (0 %) 
<6>[    0.872508]  [0:      swapper/0:    1]   lv : [ 670000], volt = 706250 uV (0 %) 
<6>[    0.872518]  [0:      swapper/0:    1]   lv : [ 660000], volt = 662500 uV (0 %) 
<6>[    0.872528]  [0:      swapper/0:    1]   lv : [ 650000], volt = 587500 uV (0 %) 
<6>[    0.872540]  [0:      swapper/0:    1] dvfs_type : dvfs_cam - id : 6
<6>[    0.872548]  [0:      swapper/0:    1]   num_of_lv      : 7
<6>[    0.872557]  [0:      swapper/0:    1]   num_of_members : 6
<6>[    0.872567]  [0:      swapper/0:    1]   lv : [ 700000], volt = 956250 uV (0 %) 
<6>[    0.872578]  [0:      swapper/0:    1]   lv : [ 690000], volt = 887500 uV (0 %) 
<6>[    0.872588]  [0:      swapper/0:    1]   lv : [ 680000], volt = 825000 uV (0 %) 
<6>[    0.872598]  [0:      swapper/0:    1]   lv : [ 670000], volt = 681250 uV (0 %) 
<6>[    0.872608]  [0:      swapper/0:    1]   lv : [ 660000], volt = 681250 uV (0 %) 
<6>[    0.872619]  [0:      swapper/0:    1]   lv : [ 650000], volt = 593750 uV (0 %) 
<6>[    0.872629]  [0:      swapper/0:    1]   lv : [ 640000], volt = 581250 uV (0 %) 
<6>[    0.872641]  [0:      swapper/0:    1] dvfs_type : dvfs_disp - id : 7
<6>[    0.872650]  [0:      swapper/0:    1]   num_of_lv      : 5
<6>[    0.872658]  [0:      swapper/0:    1]   num_of_members : 2
<6>[    0.872669]  [0:      swapper/0:    1]   lv : [ 533000], volt = 775000 uV (0 %) 
<6>[    0.872741]  [0:      swapper/0:    1]   lv : [ 400000], volt = 725000 uV (0 %) 
<6>[    0.872752]  [0:      swapper/0:    1]   lv : [ 333000], volt = 643750 uV (0 %) 
<6>[    0.872762]  [0:      swapper/0:    1]   lv : [ 167000], volt = 606250 uV (0 %) 
<6>[    0.872772]  [0:      swapper/0:    1]   lv : [ 111000], volt = 581250 uV (0 %) 
<6>[    0.872784]  [0:      swapper/0:    1] dvfs_type : dvfs_aud - id : 8
<6>[    0.872793]  [0:      swapper/0:    1]   num_of_lv      : 4
<6>[    0.872801]  [0:      swapper/0:    1]   num_of_members : 4
<6>[    0.872812]  [0:      swapper/0:    1]   lv : [1180000], volt = 825000 uV (0 %) 
<6>[    0.872823]  [0:      swapper/0:    1]   lv : [ 800000], volt = 806250 uV (0 %) 
<6>[    0.872833]  [0:      swapper/0:    1]   lv : [ 590000], volt = 662500 uV (0 %) 
<6>[    0.872843]  [0:      swapper/0:    1]   lv : [ 394000], volt = 581250 uV (0 %) 
<6>[    0.872855]  [0:      swapper/0:    1] dvfs_type : dvs_cp - id : 9
<6>[    0.872864]  [0:      swapper/0:    1]   num_of_lv      : 1
<6>[    0.872872]  [0:      swapper/0:    1]   num_of_members : 1
<6>[    0.872882]  [0:      swapper/0:    1]   lv : [1000000], volt = 762500 uV (0 %) 
<6>[    0.872935]  [0:      swapper/0:    1] Initialized dma-buf trace successfully.
<6>[    0.873167]  [0:      swapper/0:    1] Trying to unpack rootfs image as initramfs...
*/

/*
for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			if(fvmap_header[i].num_of_lv==10&&fvmap_header[i].num_of_members==7) //for mif
			{
				if(old->table[j].rate==2093000)
					new->table[j].volt = old->table[j].volt=1300000;//812500
				if(old->table[j].rate==2002000)
					new->table[j].volt = old->table[j].volt=1300000;//812500
				if(old->table[j].rate==1794000)
					new->table[j].volt = old->table[j].volt=668750-6250-6250-6250;//668750
				if(old->table[j].rate==1539000)
					new->table[j].volt = old->table[j].volt=656250-6250-6250-6250;//668750
				if(old->table[j].rate==1352000)
					new->table[j].volt = old->table[j].volt=600000-6250-6250-6250;//600000
				if(old->table[j].rate==1014000)
					new->table[j].volt = old->table[j].volt=587500-6250-6250;//600000
				if(old->table[j].rate==845000)
					new->table[j].volt = old->table[j].volt=575000-6250-6250;//600000
				if(old->table[j].rate==676000)
					new->table[j].volt = old->table[j].volt=562500-6250-6250;//600000
				if(old->table[j].rate==546000)
					new->table[j].volt = old->table[j].volt=550000-6250-6250;//600000
				if(old->table[j].rate==420000)
					new->table[j].volt = old->table[j].volt=537500-6250-6250-6250;//600000
				new->table[j].rate = old->table[j].rate;
				new->table[j].volt = old->table[j].volt;
				pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==5&&fvmap_header[i].num_of_members==4) //for int
			{
				if(old->table[j].rate==533000)
					new->table[j].volt = old->table[j].volt=693750-6250-6250;//706250
				if(old->table[j].rate==333000)
					new->table[j].volt = old->table[j].volt=600000-6250-6250;//600000
				if(old->table[j].rate==267000)
					new->table[j].volt = old->table[j].volt=587500-6250-6250;//600000
				if(old->table[j].rate==133000)
					new->table[j].volt = old->table[j].volt=575000-6250-6250;//600000
				if(old->table[j].rate==107000)
					new->table[j].volt = old->table[j].volt=562500-6250-6250-6250;//600000
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==17&&fvmap_header[i].num_of_members==2) //for big
			{
				if(old->table[j].rate==2496000)
					new->table[j].volt = old->table[j].volt=1300000;//1143750
				if(old->table[j].rate==2392000)
					new->table[j].volt = old->table[j].volt=1300000;//1143750
				if(old->table[j].rate<=2184000&&old->table[j].rate>1352000)
					{
						old->table[j].volt=old->table[j].volt-6250;//1143750
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate<=1352000&&old->table[j].rate>520000)
					{
						old->table[j].volt=old->table[j].volt-12500;//1143750
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==2704000)
					new->table[j].volt = old->table[j].volt=1225000;//1225000
				/*if(old->table[j].rate==2600000)
					new->table[j].volt = old->table[j].volt=1225000-1250;//1143750
				if(old->table[j].rate==2496000)
					new->table[j].volt = old->table[j].volt=1225000-12500;//1087500
				if(old->table[j].rate==2314000)
					new->table[j].volt = old->table[j].volt=1225000-12500;//1037500
				if(old->table[j].rate==2210000)
					new->table[j].volt = old->table[j].volt=1175000-12500;//987500
				if(old->table[j].rate==2184000)
					new->table[j].volt = old->table[j].volt=1143750-1250;//1143750
				if(old->table[j].rate==2080000)
					new->table[j].volt = old->table[j].volt=1087500-12500;//1087500
				if(old->table[j].rate==1976000)
					new->table[j].volt = old->table[j].volt=1037500-12500;//1037500
				if(old->table[j].rate==1872000)
					new->table[j].volt = old->table[j].volt=987500-12500;//987500
				if(old->table[j].rate==1768000)
					new->table[j].volt = old->table[j].volt=943750-12500;//943750
				if(old->table[j].rate==1664000)
					new->table[j].volt = old->table[j].volt=918750-12500;//918750
				if(old->table[j].rate==1560000)
					new->table[j].volt = old->table[j].volt=875000-12500;//875000
				if(old->table[j].rate==1352000)
					new->table[j].volt = old->table[j].volt=806250-12500;//806250
				if(old->table[j].rate==1144000)
					new->table[j].volt = old->table[j].volt=743750-12500;//743750
				if(old->table[j].rate==936000)
					new->table[j].volt = old->table[j].volt=675000;//687500
				if(old->table[j].rate==728000)
					new->table[j].volt = old->table[j].volt=618700;//631250
*/
				if(old->table[j].rate==520000)
					new->table[j].volt = old->table[j].volt=587500-6250;//600000
				if(old->table[j].rate==312000)
					new->table[j].volt = old->table[j].volt=575000-6250;//600000
				if(old->table[j].rate==208000)
					new->table[j].volt = old->table[j].volt=562500-6250;//600000
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}

			 if(fvmap_header[i].num_of_lv==18&&fvmap_header[i].num_of_members==4) //for litle
			{
				if(old->table[j].rate==2002000)
					new->table[j].volt = old->table[j].volt=1171250;//1031250
				if(old->table[j].rate==1898000)
					new->table[j].volt = old->table[j].volt=1087500;//1031250
				if(old->table[j].rate<=1690000&&old->table[j].rate>1352000)
					{
						old->table[j].volt=old->table[j].volt-6250;//1143750
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate<=1352000&&old->table[j].rate>546000)
					{
						old->table[j].volt=old->table[j].volt-12500;//1143750
						new->table[j].volt = old->table[j].volt;
					}
/*
				if(old->table[j].rate==1794000)
					new->table[j].volt = old->table[j].volt=1031250;//1031250
				if(old->table[j].rate==1690000)
					new->table[j].volt = old->table[j].volt=1031250-12500;//1031250
				if(old->table[j].rate==1586000)
					new->table[j].volt = old->table[j].volt=950000-12500;//950000
				if(old->table[j].rate<1586000&&old->table[j].rate>546000)
					{
						old->table[j].volt=old->table[j].volt-12500;
						new->table[j].volt = old->table[j].volt;
					}
					//new->table[j].volt = old->table[j].volt-12500;//??
*/
				if(old->table[j].rate==546000)
					new->table[j].volt = old->table[j].volt=587500-6250;//600000
				if(old->table[j].rate==449000)
					new->table[j].volt = old->table[j].volt=575000-6250;//600000
				if(old->table[j].rate==343000)
					new->table[j].volt = old->table[j].volt=562500-6250;//600000
				if(old->table[j].rate==208000)
					new->table[j].volt = old->table[j].volt=550000-6250;//600000
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}

			 if(fvmap_header[i].num_of_lv==9&&fvmap_header[i].num_of_members==1) //for G3d
			{
				if(old->table[j].rate==1053000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//1106250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==949000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//1006250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==839000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//931250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==764000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//856250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==683000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//762500
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==572000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//681250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==455000)
					{
						old->table[j].volt=old->table[j].volt-25000-6250-12500;//650000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==385000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500-12500;//612500
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==260000)
					{
						old->table[j].volt=old->table[j].volt-25000-18750-12500;//600000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}

			 if(fvmap_header[i].num_of_lv==4&&fvmap_header[i].num_of_members==2) //for fsys
			{
				if(old->table[j].rate==267000)
					{
						old->table[j].volt=old->table[j].volt-12500;//700000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==200000)
					{
						old->table[j].volt=old->table[j].volt-18750;//625000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate>=100000)
					{
						old->table[j].volt=old->table[j].volt-25000;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate>=50000)
					{
						old->table[j].volt=old->table[j].volt-25000-6250;//600000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==5&&fvmap_header[i].num_of_members==9) //for cam
			{
				if(old->table[j].rate==690000)
					{
						old->table[j].volt=old->table[j].volt-6250;//725000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==680000)
					{
						old->table[j].volt=old->table[j].volt-18750;//656250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==670000)
					{
						old->table[j].volt=old->table[j].volt-18750;//656250
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==660000)
					{
						old->table[j].volt=old->table[j].volt-25000;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==650000)
					{
						old->table[j].volt=old->table[j].volt-25000-6250;//600000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==5&&fvmap_header[i].num_of_members==2) //for disp
			{
				if(old->table[j].rate==333000)
					{
						old->table[j].volt=old->table[j].volt-12500;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==267000)
					{
						old->table[j].volt=old->table[j].volt-18750;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==222000)
					{
						old->table[j].volt=old->table[j].volt-25000;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==167000)
					{
						old->table[j].volt=old->table[j].volt-25000-6250;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==80000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//600000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==4&&fvmap_header[i].num_of_members==3) //for aud
			{
				if(old->table[j].rate==1180000)
					{
						old->table[j].volt=old->table[j].volt-25000;//700000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==800000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==590000)
					{
						old->table[j].volt=old->table[j].volt-25000-18750;//600000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==394000)
					{
						old->table[j].volt=old->table[j].volt-50000;//600000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
			}
			 if(fvmap_header[i].num_of_lv==2&&fvmap_header[i].num_of_members==1) //for cp
			{
				if(old->table[j].rate==1000000)
					{
						old->table[j].volt=old->table[j].volt-25000;//750000
						new->table[j].volt = old->table[j].volt;
					}
				if(old->table[j].rate==800000)
					{
						old->table[j].volt=old->table[j].volt-25000-12500;//750000
						new->table[j].volt = old->table[j].volt;
					}
				new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) ,Topser99 edit\n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);

		for (j = 0; j < fvmap_header[i].num_of_lv; j++) {
			new->table[j].rate = old->table[j].rate;
			new->table[j].volt = old->table[j].volt;
			pr_info("  lv : [%7d], volt = %d uV (%d %%) \n",
				new->table[j].rate, new->table[j].volt,
				volt_offset_percent);
		}

		for (j = 0; j < fvmap_header[i].num_of_pll; j++) {
			clks = sram_base + fvmap_header[i].o_members;
			plls = sram_base + clks->addr[j];
			clk_node = cmucal_get_node(vclk->list[j]);
			if (clk_node == NULL)
				continue;
			paddr_offset = clk_node->paddr & 0xFFFF;
			fvaddr_offset = plls->addr & 0xFFFF;
			if (paddr_offset == fvaddr_offset)
				continue;

			clk_node->paddr += fvaddr_offset - paddr_offset;
			clk_node->pll_con0 += fvaddr_offset - paddr_offset;
			if (clk_node->pll_con1)
				clk_node->pll_con1 += fvaddr_offset - paddr_offset;
		}
	}
}

int fvmap_init(void __iomem *sram_base)
{
	void __iomem *map_base;

	map_base = kzalloc(FVMAP_SIZE, GFP_KERNEL);

	fvmap_base = map_base;
	sram_fvmap_base = sram_base;
	pr_info("%s:fvmap initialize %pK\n", __func__, sram_base);
	fvmap_copy_from_sram(map_base, sram_base);

	return 0;
}

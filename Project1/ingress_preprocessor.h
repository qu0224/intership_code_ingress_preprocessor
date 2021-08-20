#pragma once 

#include "hls_stream.h"
#include "ap_int.h"
#include "ap_axi_sdata.h"

struct demac_os
{
	ap_uint<48> dmac;
	ap_uint<12> vlan_id_os;
};

struct cnt_is
{
	ap_uint<5> ts[8];
	ap_uint<5> rs[8];
	ap_uint<5> rts[8];
	ap_uint<5> be[8];
};

struct emp_is
{
	ap_uint<9> ts[8];
	ap_uint<9> rs[8];
	ap_uint<9> rts[8];
	ap_uint<9> be[8];
};

void ingress_preprocessor(
	hls::stream<ap_axiu<64, 0, 0, 0>>& is,
	ap_uint<80>& time_stamp,
	ap_uint<1>& tstamp_valid,
	hls::stream<ap_uint<8>>& dport_is,
	hls::stream<demac_os>& dmac_os,
	hls::stream<ap_axiu<64, 9, 1, 3>>& os_be_rs,
	hls::stream<ap_axiu<64, 9, 1, 3>>& os_ts,
	hls::stream<ap_axiu<64, 0, 0, 0>>& os_ps_time,
	hls::stream<ap_axiu<64, 0, 0, 0>>& os_ps,
	ap_uint<40>& ts_cnt,
	ap_uint<40>& rs_cnt,
	ap_uint<40>& rts_cnt,
	ap_uint<40>& be_cnt,
	ap_uint<72>& ts_emp,
	ap_uint<72>& rs_emp,
	ap_uint<72>& rts_emp,
	ap_uint<72>& be_emp);
#pragma once

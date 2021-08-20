#include "ingress_preprocessor.h"

// #include "hls_stream.h"
// #include "ap_axi_sdata.h"
int oneNum(int x);
int build(int x);
template<int o_I, int o_U, int o_N, int i_I, int i_U, int i_N> int frame_output(
	int remain,
	ap_axiu<64, i_I, i_U, i_N> t2,
	hls::stream<ap_axiu<64, 0, 0, 0>>& is,
	hls::stream<ap_axiu<64, o_I, o_U, o_N>>& out_port
);

ap_axiu<64, 9, 1, 3> frame_cache2[188];

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
	ap_uint<72>& be_emp)
{
#pragma HLS interface axis port = is
#pragma HLS interface axis port = os_be_rs
#pragma HLS interface axis port = os_ts
	//#pragma HLS interface axis port = dport_is
	//#pragma HLS interface axis port = dmac_os
#pragma HLS interface axis port = os_ps_time
#pragma HLS interface axis port = os_ps

	ap_uint<80> tstamp = time_stamp;
	ap_uint<48> dmac;
	ap_uint<48> smac;
	ap_uint<32> vlan;
	ap_uint<16> vlan_type;
	ap_uint<12> vlan_id;
	ap_uint<16> et;
	ap_uint<3> vlan_pri;
	ap_uint<48> local_mac = 0x10369f683512;
	ap_uint<11> frame_count = 0;
	;
	ap_uint<8> dport;
	ap_uint<32> frame_header;
	ap_axiu<64, 9, 1, 3> frameout;
	ap_axiu<64, 0, 0, 0> frameout1;
	cnt_is cnt;
	emp_is emp;

	ap_axiu<64, 0, 0, 0> in0 = is.read();
	ap_axiu<64, 0, 0, 0> in1 = is.read();
	ap_axiu<64, 0, 0, 0> t1;
	dmac = in0.data(63, 16);
	smac = (in0.data(15, 0), in1.data(63, 32));
	ap_axiu<64, 0, 0, 0> t2 = is.read();
	if (in1.data(31, 16) == 0x8100)//含有VLAN字段的以太网帧，需要进一步判断TS\BE\RS帧
	{
		vlan = in1.data(31, 0);
		vlan_type = vlan(31, 16);//（标签协议标识符），表示数据帧类型
		vlan_pri = vlan(15, 13);//表示数据帧的802.1p优先级,值越大优先级越高。
		vlan_id = vlan(11, 0);//表示该数据帧所属VLAN的编号
		et = t2.data(63, 48);
	}
	else
	{
		et = in1.data(31, 16);
	}
	ap_uint<8> max_cnt_ts;
	ap_uint<8> max_cnt_rts;
	ap_uint<8> max_cnt_rs;
	ap_uint<8> max_cnt_be;
	struct demac_os dmac_check;
	//cnt
	for (int i = 0; i < 8; ++i)
	{
#pragma HLS PIPELINE II = 1
		cnt.ts[i] = ts_cnt((5 * i + 4), (5 * i));
		cnt.rs[i] = rs_cnt((5 * i + 4), (5 * i));
		cnt.rts[i] = rts_cnt((5 * i + 4), (5 * i));
		cnt.be[i] = be_cnt((5 * i + 4), (5 * i));
	}
	//emp
	for (int i = 0; i < 8; ++i)
	{
#pragma HLS PIPELINE II = 1
		emp.ts[i] = ts_emp((9 * i + 8), (9 * i));
		emp.rs[i] = rs_emp((9 * i + 8), (9 * i));
		emp.rts[i] = rts_emp((9 * i + 8), (9 * i));
		emp.be[i] = be_emp((9 * i + 8), (9 * i));
	}
	ap_uint<1> broadcast = 0;
	if (dmac == 0xFFFFFFFF)
		broadcast = 1;
	else
		broadcast = 0;

	if (dmac == 0x0180C200000E && et == 0x88f7 && tstamp_valid)
	{
		frameout1.data = tstamp(79, 16);
		frameout1.last = 0;
		frameout1.keep = 0xff;
		frameout1.data = (tstamp(15, 0), dmac);
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps_time.write(frameout1);
		frameout1.data = (smac, et);
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps_time.write(frameout1);
		frameout1.data = (in1.data(15, 0), t2.data(63, 16));
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps_time.write(frameout1);

		frame_output<0, 0, 0, 0, 0, 0>(16, t2, is, os_ps_time);
	}
	else if (dmac == local_mac)
	{
		frame_output<0, 0, 0, 0, 0, 0>(48, t2, is, os_ps);
	}
	else if (et == 0x0806)
	{
		frameout1.data = (in1.data(15, 0), t2.data(63, 16));
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps.write(frameout1);
		t1 = is.read();
		frameout1.data = (t2.data(15, 0), t1.data(63, 16));
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps.write(frameout1);
		t2 = is.read();
		frameout1.data = (t1.data(15, 0), t2.data(63, 16));
		frameout1.last = 0;
		frameout1.keep = 0xff;
		os_ps.write(frameout1);
		t1 = is.read();
		frameout1.data = (t2.data(15, 0), t1.data(63, 16));
		frameout1.last = 1;
		frameout1.keep = 0xf0;
		os_ps.write(frameout1);
	}
	else if (vlan_type == 0x8100)
	{
		frame_count = frame_output<9, 1, 3, 0, 0, 0>(48, t2, is, os_ts);

		ap_axiu<64, 9, 1, 3> d;// be0 lost
		for (int i = 0; i < 10; i++) {
			d.data = frame_cache2[i].data;
		}

		dmac_check.dmac = dmac;
		dmac_check.vlan_id_os = vlan_id;
		dmac_os.write(dmac_check);
		dport = dport_is.read();
		frame_header(31, 24) = dport;
		frame_header(23, 21) = vlan_pri;
		frame_header[20] = broadcast;
		frame_header(19, 9) = frame_count;
		max_cnt_ts = 0x00;
		max_cnt_rts = 0x00;
		if (vlan_pri == 0x0 || vlan_pri == 0x1 || vlan_pri == 0x2)
		{
			max_cnt_ts(7, 5) = 0;
			max_cnt_ts(4, 0) = cnt.ts[0];
			max_cnt_rts(7, 5) = 0;
			max_cnt_rts(4, 0) = cnt.rts[0];
			for (int j = 0; j < 7; ++j)
			{
#pragma HLS PIPELINE II = 1
				if (max_cnt_ts(4, 0) < cnt.ts[j + 1])
				{
					max_cnt_ts(7, 5) = j + 1;
					max_cnt_ts(4, 0) = cnt.ts[j + 1];
				}
				else
					continue;
			}
			for (int j = 0; j < 7; ++j)
			{
#pragma HLS PIPELINE II = 1
				if (max_cnt_rts(4, 0) < cnt.rts[j + 1])
				
{
					max_cnt_rts(7, 5) = j + 1;
					max_cnt_rts(4, 0) = cnt.rts[j + 1];
				}
				else
					continue;
			}
			if (max_cnt_ts(4, 0) > max_cnt_rts(4, 0))
				frame_header(8, 0) = emp.ts[max_cnt_ts(7, 5)];
			else
				frame_header(8, 0) = emp.rts[max_cnt_rts(7, 5)];
			max_cnt_ts = 0x00;
			max_cnt_rts = 0x00;
			frameout.data(63, 32) = frame_header;
			frameout.data(31, 0) = frame_cache2[0].data(63, 32);
			frameout.last = 0;
			frameout.keep = 0xff;
			os_ts.write(frameout);

			frame_output<9, 1, 3, 9, 1, 3>(32, frame_cache2[0], is, os_ts);
		}
		else
		{
			if (vlan_pri == 0x3 || vlan_pri == 0x4 || vlan_pri == 0x5)
			{
				max_cnt_rs(7, 5) = 0;
				max_cnt_rs(4, 0) = cnt.rs[0];
				max_cnt_rts(7, 5) = 0;
				max_cnt_rts(4, 0) = cnt.rts[0];
				for (int j = 0; j < 7; ++j)
				{

#pragma HLS PIPELINE II = 1
					if (max_cnt_rs(4, 0) < cnt.rs[j + 1])
					{
						max_cnt_rs(7, 5) = j + 1;
						max_cnt_rs(4, 0) = cnt.rs[j + 1];
					}
					else
						continue;
				}
				for (int j = 0; j < 7; ++j)
				{

#pragma HLS PIPELINE II = 1
					if (max_cnt_rts(4, 0) < cnt.rts[j + 1])
					{
						max_cnt_rts(7, 5) = j + 1;
						max_cnt_rts(4, 0) = cnt.rts[j + 1];
					}
					else
						continue;
				}
				if (max_cnt_rs(4, 0) > max_cnt_rts(4, 0))
					frame_header(8, 0) = emp.rs[max_cnt_rs(7, 5)];
				else
					frame_header(8, 0) = emp.rts[max_cnt_rts(7, 5)];
			}
			else if (vlan_pri == 0x6 || vlan_pri == 0x7)
			{
				max_cnt_be(7, 5) = 0x0;
				max_cnt_be(4, 0) = cnt.be[0];
				for (int j = 0; j < 7; ++j)
				{
#pragma HLS PIPELINE II = 1
					if (max_cnt_be(4, 0) < cnt.be[j + 1])
					{
						max_cnt_be(7, 5) = j + 1;
						max_cnt_be(4, 0) = cnt.be[j + 1];
					}
					else
						continue;
				}

				frame_header(8, 0) = emp.be[max_cnt_be(7, 5)];
				max_cnt_be = 0x00;
			}
			frameout.data(63, 32) = frame_header;
			frameout.data(31, 0) = frame_cache2[0].data(63, 32);
			frameout.last = 0;
			frameout.keep = 0xff;
			os_be_rs.write(frameout);

			frame_output<9, 1, 3, 9, 1, 3>(32, frame_cache2[0], is, os_be_rs);
		}
	}
}

template<int o_I, int o_U, int o_N, int i_I, int i_U, int i_N> int frame_output(
	int remain,
	ap_axiu<64, i_I, i_U, i_N> t2,
	hls::stream<ap_axiu<64, 0, 0, 0>>& is,
	hls::stream<ap_axiu<64, o_I, o_U, o_N>>& out_port
) {
	int frame_count = 0;
	ap_axiu<64, o_I, o_U, o_N> frame;
	for (int i = 0; i < 188; i++) {
#pragma HLS PIPELINE II = 1
		ap_axiu<64, i_I, i_U, i_N> t1;
		if (i_I == 0) {
			ap_axiu<64, 0, 0, 0> read_data = is.read();
			t1.data = read_data.data;
			t1.keep = read_data.keep;
			t1.last = read_data.last;
		}
		else {
			t1.data = frame_cache2[i + 1].data;
			t1.keep = frame_cache2[i + 1].keep;
			t1.last = frame_cache2[i + 1].last;
		}
		if (!t1.last)
		{
			frame.data = (t2.data(remain - 1, 0), t1.data(63, remain));
			frame.last = 0;
			frame.keep = 0xff;
			frame_count += 8;

			if (o_I == i_I) out_port.write(frame);
			else {
				frame_cache2[i].data = frame.data;
				frame_cache2[i].keep = frame.keep;
				frame_cache2[i].last = frame.last;
			}

			t2 = t1;
		}
		else {
			int one = oneNum(t1.keep) * 8;
			if (one + remain > 64) {
				frame.data = (t2.data(remain - 1, 0), t1.data(63, remain));
				frame.keep = 0xff;
				frame_count += 8;

				if (o_I == i_I) out_port.write(frame);
				else {
					frame_cache2[i].data = frame.data;
					frame_cache2[i].keep = frame.keep;
					frame_cache2[i].last = frame.last;
				}

				frame.data(63, 128 - remain - one) = t1.data(remain - 1, 64 - one);
				frame.data(127 - remain - one, 0) = 0;
				frame.last = 1;
				frame.keep = build((remain + one - 64) / 8);
				frame_count += (remain + one - 64) / 8;

				if (o_I == i_I) out_port.write(frame);
				else {
					frame_cache2[i + 1].data = frame.data;
					frame_cache2[i + 1].keep = frame.keep;
					frame_cache2[i + 1].last = frame.last;
				}
				break;
			}
			else if (one + remain < 64) {
				frame.data(63, 64 - remain) = t2.data(remain - 1, 0);
				if (one != 0) {
					frame.data(63 - remain, 64 - remain - one) = t1.data(63, 64 - one);
				}
				frame.last = 1;
				frame.keep = build((one + remain) / 8);
				frame_count += (one + remain) / 8;

				if (o_I == i_I) out_port.write(frame);
				else {
					frame_cache2[i].data = frame.data;
					frame_cache2[i].keep = frame.keep;
					frame_cache2[i].last = frame.last;
				}
				break;
			}
			else {
				frame.data(63, 0) = (t2.data(remain - 1, 0), t1.data(63, 64 - one));

				if (o_I == i_I) out_port.write(frame);
				else {
					frame_cache2[i].data = frame.data;
					frame_cache2[i].keep = frame.keep;
					frame_cache2[i].last = frame.last;
				}
				break;
			}
		}
	}
	return frame_count;
}

int oneNum(int x) {
	int count = 0;
	while (x) {
		if (x % 2 != 0) count++;
		x = x / 2;
	}
	return count;
}


int build(int x) {
	int res = 0;
	int idx = x;
	for (int i = 0; i < x; i++) {
		res = res * 2 + 1;
	}
	for (int i = 0; i < 8 - x; i++) {
		res = res * 2;
	}
	return res;
}
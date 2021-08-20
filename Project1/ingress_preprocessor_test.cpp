#include "ingress_preprocessor.h"
// #include "hls_stream.h"
// #include "ap_int.h"
// #include "ap_axi_sdata.h"

int main()
{
	hls::stream<ap_axiu<64, 0, 0, 0> > is;
	ap_uint<64> tstamp0 = 0x1678cda2178bd4e5;
	ap_uint<16> tstamp1 = 0x8bef;
	ap_uint<80> time_stamp = (tstamp0, tstamp1);
	ap_uint<1> tstamp_valid = 1;
	hls::stream<ap_uint<8> >  dport_is;
	hls::stream<demac_os>  dmac_os;
	hls::stream<ap_axiu<64, 9, 1, 3> > os_be_rs;
	hls::stream<ap_axiu<64, 9, 1, 3> > os_ts;
	hls::stream<ap_axiu<64, 0, 0, 0> > os_ps_time;
	hls::stream<ap_axiu<64, 0, 0, 0> > os_ps;


	ap_axiu<64, 0, 0, 0> framein;
	ap_axiu<64, 0, 0, 0> frameout;
	ap_uint<40> ts_cnt = 0x1200010f11;
	ap_uint<40> rs_cnt = 0x107e0f1110;
	ap_uint<40> rts_cnt = 0x120e010f11;
	ap_uint<40> be_cnt = 0x120e010f11;
	ap_uint<64> emp0 = 0xf40123456789abcd;
	ap_uint<8> emp1 = 0xef;
	ap_uint<72> ts_emp = (emp0, emp1);
	ap_uint<72> rs_emp = (emp0, emp1);
	ap_uint<72> rts_emp = (emp0, emp1);
	ap_uint<72> be_emp = (emp0, emp1);
	//CNC

	ap_uint<48> demac = 0x10369f683512;
	ap_uint<48> semac = 0x000A3500010e;
	ap_uint<32> vlan = 0x81008000;
	ap_uint<16> ethertype = 0x0800;
	ap_uint<48> data0 = 0x7f9e3c2b1d0f;
	ap_uint<64> data1 = 0x1123456789abcdef;
	ap_uint<64> data2 = 0x34bd6f7c8e9a12dc;

	framein.data = (demac, semac(47, 32));
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = (semac(31, 0), vlan);
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = (ethertype, data0);
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = data1;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = data2;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = data1;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = data1;
	framein.last = 1;
	framein.keep = 0x00;
	is.write(framein);

	//调用函数
	ingress_preprocessor(is, time_stamp, tstamp_valid, dport_is, dmac_os, os_be_rs, os_ts,
		os_ps_time, os_ps, ts_cnt, rs_cnt, rts_cnt, be_cnt, ts_emp, rs_emp, rts_emp, be_emp);

	ap_uint<48> tdemac;
	ap_uint<48> tsemac;
	ap_uint<32> tvlan;
	ap_uint<16> tethertype;
	ap_uint<304> tdata;

	frameout = os_ps.read();
	tdata(303, 240) = frameout.data;
	frameout = os_ps.read();
	tdata(239, 176) = frameout.data;
	frameout = os_ps.read();
	tdata(175, 112) = frameout.data;
	frameout = os_ps.read();
	tdata(111, 48) = frameout.data;
	//frameout = os_ps.read();
	//tdata(47, 0) = frameout.data(63, 16);
	printf("%s\n", tdata.to_string(16).c_str());

	//ARP
	ap_uint<64> in0 = 0xffffffffffff0021;
	ap_uint<64> in1 = 0xccc5036a08060001;
	ap_uint<64> in2 = 0x0800060400010021;
	ap_uint<64> in3 = 0xccc5036a0a02196e;
	ap_uint<64> in4 = 0x0000000000000a02;
	ap_uint<64> in5 = 0x1921000000000000;
	framein.data = in0;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = in1;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = in2;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = in3;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = in4;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = in5;
	framein.last = 0;
	framein.keep = 0xc0;
	is.write(framein);
	ingress_preprocessor(is, time_stamp, tstamp_valid, dport_is, dmac_os, os_be_rs, os_ts,
		os_ps_time, os_ps, ts_cnt, rs_cnt, rts_cnt, be_cnt, ts_emp, rs_emp, rts_emp, be_emp);
	ap_uint<224> arp_data;
	frameout = os_ps.read();
	arp_data(223, 160) = frameout.data;
	frameout = os_ps.read();
	arp_data(159, 96) = frameout.data;
	frameout = os_ps.read();
	arp_data(95, 32) = frameout.data;
	frameout = os_ps.read();
	arp_data(31, 0) = frameout.data(63, 32);
	printf("%s\n", arp_data.to_string(16).c_str());

	//PTP
	ap_uint<64> ptp0 = 0x0180C200000ee454;
	ap_uint<64> ptp1 = 0xe8deca6688f70001;
	ap_uint<64> ptp2 = 0x080006040001e454;
	ap_uint<64> ptp3 = 0xe8deca660a0219b8;
	ap_uint<64> ptp4 = 0x670450123000a9fe;
	ap_uint<64> ptp5 = 0xa9fe123456789abc;
	framein.data = ptp0;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = ptp1;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = ptp2;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = ptp3;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = ptp4;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = ptp5;
	framein.last = 1;
	framein.keep = 0xfe;
	is.write(framein);
	ingress_preprocessor(is, time_stamp, tstamp_valid, dport_is, dmac_os, os_be_rs, os_ts,
		os_ps_time, os_ps, ts_cnt, rs_cnt, rts_cnt, be_cnt, ts_emp, rs_emp, rts_emp, be_emp);
	ap_uint<464> ptp_data;
	frameout = os_ps_time.read();
	ptp_data(463, 400) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(399, 336) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(335, 272) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(271, 208) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(207, 144) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(143, 80) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(79, 16) = frameout.data;
	frameout = os_ps_time.read();
	ptp_data(15, 0) = frameout.data(63, 48);
	printf("%s\n", frameout.keep.to_string(16).c_str());
	printf("%s\n", ptp_data.to_string(16).c_str());

	//be ֡2360391A 06040001E454E8DECA660A0219B8670450123000A9FEA9FE123456789ABC
	ap_uint<64> be0 = 0x357dfe5932dbe454;
	ap_uint<64> be1 = 0xe8deca6681006000;
	ap_uint<64> be2 = 0x080006040001e454;
	ap_uint<64> be3 = 0xe8deca660a0219b8;
	ap_uint<64> be4 = 0x670450123000a9fe;
	ap_uint<64> be5 = 0xa9fe123456789abc;

	ap_axiu<64, 9, 1, 3> frameout1;
	ap_uint<8> dport = 0x23;
	dport_is.write(dport);
	framein.data = be0;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = be1;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = be2;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = be3;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = be4;
	framein.last = 0;
	framein.keep = 0xff;
	is.write(framein);
	framein.data = be5;
	framein.last = 1;
	framein.keep = 0xfc;
	is.write(framein);
	ingress_preprocessor(is, time_stamp, tstamp_valid, dport_is, dmac_os, os_be_rs, os_ts,
		os_ps_time, os_ps, ts_cnt, rs_cnt, rts_cnt, be_cnt, ts_emp, rs_emp, rts_emp, be_emp);
	ap_uint<272> be_data;
	frameout1 = os_be_rs.read();
	be_data(271, 208) = frameout1.data;
	frameout1 = os_be_rs.read();
	be_data(207, 144) = frameout1.data;
	frameout1 = os_be_rs.read();
	be_data(143, 80) = frameout1.data;
	frameout1 = os_be_rs.read();
	be_data(79, 16) = frameout1.data;
	frameout1 = os_be_rs.read();
	be_data(15, 0) = frameout1.data(63, 48);
	printf("%s\n", be_data.to_string(16).c_str());

	ap_uint<272> ts_data;
	frameout1 = os_ts.read();
	ts_data(271, 208) = frameout1.data;
	frameout1 = os_ts.read();
	ts_data(207, 144) = frameout1.data;
	frameout1 = os_ts.read();
	ts_data(143, 80) = frameout1.data;
	frameout1 = os_ts.read();
	ts_data(79, 16) = frameout1.data;
	frameout1 = os_ts.read();
	ts_data(15, 0) = frameout1.data(63, 48);
	printf("%s\n", ts_data.to_string(16).c_str());

	return 0;
}

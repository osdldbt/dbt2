create table warehouse ( w_id integer, w_name varchar(10), w_street_1 varchar(20), w_street_2 varchar(20), w_city varchar(20), w_state char(2), w_zip char(9), w_tax real, w_ytd numeric(24, 12), constraint pk_warehouse primary key (w_id) );

create table district ( d_id integer, d_w_id integer, d_name varchar(10), d_street_1 varchar(20), d_street_2 varchar(20), d_city varchar(20), d_state char(2), d_zip char(9), d_tax real, d_ytd numeric(24, 12), d_next_o_id integer, constraint pk_district primary key (d_w_id, d_id) );

create table customer ( c_id integer, c_d_id integer, c_w_id integer, c_first varchar(16), c_middle char(2), c_last varchar(16), c_street_1 varchar(20), c_street_2 varchar(20), c_city varchar(20), c_state char(2), c_zip char(9), c_phone char(16), c_since timestamp, c_credit char(2), c_credit_lim numeric(24, 12), c_discount real, c_balance numeric(24, 12), c_ytd_payment numeric(24, 12), c_payment_cnt real, c_delivery_cnt real, c_data varchar(500), constraint pk_customer primary key (c_w_id, c_d_id, c_id) );

create table history ( h_c_id integer, h_c_d_id integer, h_c_w_id integer, h_d_id integer, h_w_id integer, h_date timestamp, h_amount real, h_data varchar(24) );

create table new_order ( no_o_id integer, no_d_id integer, no_w_id integer, constraint pk_new_order primary key (no_w_id, no_d_id, no_o_id) );

create table orders ( o_id integer, o_d_id integer, o_w_id integer, o_c_id integer, o_entry_d timestamp, o_carrier_id integer, o_ol_cnt integer, o_all_local real, constraint pk_orders primary key (o_w_id, o_d_id, o_id) );

create table order_line ( ol_o_id integer, ol_d_id integer, ol_w_id integer, ol_number integer, ol_i_id integer, ol_supply_w_id integer, ol_delivery_d timestamp, ol_quantity real, ol_amount real, ol_dist_info varchar(24), constraint pk_order_line primary key (ol_w_id, ol_d_id, ol_o_id, ol_number) );

create table item ( i_id integer, i_im_id integer, i_name varchar(24), i_price real, i_data varchar(50), constraint pk_item primary key (i_id) );

create table stock ( s_i_id integer, s_w_id integer, s_quantity real, s_dist_01 varchar(24), s_dist_02 varchar(24), s_dist_03 varchar(24), s_dist_04 varchar(24), s_dist_05 varchar(24), s_dist_06 varchar(24), s_dist_07 varchar(24), s_dist_08 varchar(24), s_dist_09 varchar(24), s_dist_10 varchar(24), s_ytd numeric(16, 8), s_order_cnt real, s_remote_cnt real, s_data varchar(50), constraint pk_stock primary key (s_w_id, s_i_id, s_quantity) );

commit;

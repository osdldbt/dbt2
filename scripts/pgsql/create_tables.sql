create table warehouse ( w_id numeric(9), w_name varchar(10), w_street_1 varchar(20), w_street_2 varchar(20), w_city varchar(20), w_state char(2), w_zip char(9), w_tax numeric(8, 4), w_ytd numeric(24, 12), constraint pk_warehouse primary key (w_id) );

create table district ( d_id numeric(2), d_w_id numeric(9), d_name varchar(10), d_street_1 varchar(20), d_street_2 varchar(20), d_city varchar(20), d_state char(2), d_zip char(9), d_tax numeric(8, 4), d_ytd numeric(24, 12), d_next_o_id numeric(8), constraint pk_district primary key (d_w_id, d_id) );

create table customer ( c_id numeric(5), c_d_id numeric(2), c_w_id numeric(9), c_first varchar(16), c_middle char(2), c_last varchar(16), c_street_1 varchar(20), c_street_2 varchar(20), c_city varchar(20), c_state char(2), c_zip char(9), c_phone char(16), c_since timestamp, c_credit char(2), c_credit_lim numeric(24, 12), c_discount numeric(8, 4), c_balance numeric(24, 12), c_ytd_payment numeric(24, 12), c_payment_cnt numeric(4), c_delivery_cnt numeric(4), c_data varchar(500), constraint pk_customer primary key (c_w_id, c_d_id, c_id) );

create table history ( h_c_id numeric(5), h_c_d_id numeric(2), h_c_w_id numeric(9), h_d_id numeric(2), h_w_id numeric(9), h_date timestamp, h_amount numeric(12, 6), h_data varchar(24) );

create table new_order ( no_o_id numeric(8), no_d_id numeric(2), no_w_id numeric(9), constraint pk_new_order primary key (no_w_id, no_d_id, no_o_id) );

create table orders ( o_id numeric(8), o_d_id numeric(2), o_w_id numeric(9), o_c_id numeric(5), o_entry_d timestamp, o_carrier_id numeric(2), o_ol_cnt numeric(2), o_all_local numeric(1), constraint pk_orders primary key (o_w_id, o_d_id, o_id) );

create table order_line ( ol_o_id numeric(8), ol_d_id numeric(2), ol_w_id numeric(9), ol_number numeric(2), ol_i_id numeric(6), ol_supply_w_id numeric(9), ol_delivery_d timestamp, ol_quantity numeric(4), ol_amount numeric (12, 6), ol_dist_info varchar(24), constraint pk_order_line primary key (ol_w_id, ol_d_id, ol_o_id, ol_number) );

create table item ( i_id numeric(6), i_im_id numeric(6), i_name varchar(24), i_price numeric(10, 5), i_data varchar(50), constraint pk_item primary key (i_id) );

create table stock ( s_i_id numeric(6), s_w_id numeric(9), s_quantity numeric(4), s_dist_01 varchar(24), s_dist_02 varchar(24), s_dist_03 varchar(24), s_dist_04 varchar(24), s_dist_05 varchar(24), s_dist_06 varchar(24), s_dist_07 varchar(24), s_dist_08 varchar(24), s_dist_09 varchar(24), s_dist_10 varchar(24), s_ytd numeric(16, 8), s_order_cnt numeric(8, 4), s_remote_cnt numeric(8, 4), s_data varchar(50), constraint pk_stock primary key (s_w_id, s_i_id, s_quantity) );

commit;

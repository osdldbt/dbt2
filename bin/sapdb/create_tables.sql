sql_execute create table warehouse ( w_id fixed(9), w_name varchar(10), w_street_1 varchar(20), w_street_2 varchar(20), w_city varchar(20), w_state char(2), w_zip char(9), w_tax fixed(8, 4), w_ytd fixed(24, 12), primary key(w_id) )

sql_execute create table district ( d_id fixed(2), d_w_id fixed(9), d_name varchar(10), d_street_1 varchar(20), d_street_2 varchar(20), d_city varchar(20), d_state char(2), d_zip char(9), d_tax fixed(8, 4), d_ytd fixed(24, 12), d_next_o_id fixed(8), primary key(d_w_id, d_id) )

sql_execute create table customer ( c_id fixed(5), c_d_id fixed(2), c_w_id fixed(9), c_first varchar(16), c_middle char(2), c_last varchar(16), c_street_1 varchar(20), c_street_2 varchar(20), c_city varchar(20), c_state char(2), c_zip char(9), c_phone char(16), c_since timestamp, c_credit char(2), c_credit_lim fixed(24, 12), c_discount fixed(8, 4), c_balance fixed(24, 12), c_ytd_payment fixed(24, 12), c_payment_cnt fixed(4), c_delivery_cnt fixed(4), c_data varchar(500), primary key(c_w_id, c_d_id, c_id) )

sql_execute create table history ( h_c_id fixed(5), h_c_d_id fixed(2), h_c_w_id fixed(9), h_d_id fixed(2), h_w_id fixed(9), h_date timestamp, h_amount fixed(12, 6), h_data varchar(24) )

sql_execute create table new_order ( no_o_id fixed(8), no_d_id fixed(2), no_w_id fixed(9), primary key(no_w_id, no_d_id, no_o_id) )

sql_execute create table orders ( o_id fixed(8), o_d_id fixed(2), o_w_id fixed(9), o_c_id fixed(5), o_entry_d timestamp, o_carrier_id fixed(2), o_ol_cnt fixed(2), o_all_local fixed(1), primary key(o_w_id, o_d_id, o_id) )

sql_execute create table order_line ( ol_o_id fixed(8), ol_d_id fixed(2), ol_w_id fixed(9), ol_number fixed(2), ol_i_id fixed(6), ol_supply_w_id fixed(9), ol_delivery_d timestamp, ol_quantity fixed(4), ol_amount fixed (12, 6), ol_dist_info varchar(24), primary key(ol_w_id, ol_d_id, ol_o_id, ol_number) )

sql_execute create table item ( i_id fixed(6), i_im_id fixed(6), i_name varchar(24), i_price fixed(10, 5), i_data varchar(50), primary key(i_id) )

sql_execute create table stock ( s_i_id fixed(6), s_w_id fixed(9), s_quantity fixed(4), s_dist_01 varchar(24), s_dist_02 varchar(24), s_dist_03 varchar(24), s_dist_04 varchar(24), s_dist_05 varchar(24), s_dist_06 varchar(24), s_dist_07 varchar(24), s_dist_08 varchar(24), s_dist_09 varchar(24), s_dist_10 varchar(24), s_ytd fixed(16, 8), s_order_cnt fixed(8, 4), s_remote_cnt fixed(8, 4), s_data varchar(50), primary key(s_w_id, s_i_id, s_quantity) )

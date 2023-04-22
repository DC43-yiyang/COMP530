
CREATE TABLE part (

	p_partkey		INT,
	p_name		 	STRING,	
	p_mfgr			STRING,
	p_brand			STRING,
	p_type			STRING,
	p_size			INT,
	p_container		STRING,
	p_retailprice		DOUBLE,	
	p_comment		STRING
);

CREATE TABLE supplier (
	s_suppkey		INT,
	s_name			STRING,
	s_address		STRING,
	s_nationkey		INT,
	s_phone			STRING,
	s_acctbal		DOUBLE,
	s_comment		STRING
);

CREATE TABLE partsupp (
	ps_partkey	INT,	
	ps_suppkey	INT,	
	ps_availqty	INT,
	ps_supplycost	DOUBLE,
	ps_comment	STRING
);

CREATE TABLE customer (
	c_custkey	INT,
	c_name		STRING,
	c_address	STRING,
	c_nationkey	INT,
	c_phone		STRING,
	c_acctbal	DOUBLE,
	c_mktsegment	STRING,
	c_comment	STRING
);

CREATE TABLE orders (
	o_orderkey	INT,
	o_custkey	INT,	
	o_orderstatus	STRING,
	o_totalprice	DOUBLE,
	o_orderdate	STRING,
	o_orderpriority	STRING,
	o_clerk		STRING,
	o_shippriority	INT,
	o_comment	STRING
);

CREATE TABLE lineitem (
	l_orderkey	INT,
	l_partkey	INT,
	l_suppkey	INT,
	l_linenumber	INT,
	l_quantity	INT,
	l_extendedprice	DOUBLE,
	l_discount	DOUBLE,
	l_tax		DOUBLE,
	l_returnflag	STRING,
	l_linestatus	STRING,
	l_shipdate	STRING,
	l_commitdate	STRING,
	l_receiptdate	STRING,
	l_shipinstruct	STRING,
	l_shipmode	STRING,
	l_comment	STRING
);

CREATE TABLE nation (
	n_nationkey		INT,
	n_name			STRING,
	n_regionkey		INT,	
	n_comment		STRING
);

CREATE TABLE region (
	r_regionkey	INT,
	r_name		STRING,
	r_comment	STRING
);

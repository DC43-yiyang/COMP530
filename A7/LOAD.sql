-- you need to create a directory called TPC-H , which includes all the TPC-H database files. And put it in the Build directory.


LOAD lineitem FROM TPC-H/lineitem.tbl;
LOAD orders FROM TPC-H/orders.tbl;

LOAD part FROM TPC-H/part.tbl;
LOAD partsupp FROM TPC-H/partsupp.tbl;
LOAD supplier FROM TPC-H/supplier.tbl;
LOAD nation FROM TPC-H/nation.tbl;
LOAD region FROM TPC-H/region.tbl;

LOAD customer FROM TPC-H/customer.tbl;

/*
 * Note: this file originally auto-generated by mib2c using
 *  : generic-table-oids.m2c 12855 2005-09-27 15:56:08Z rstory $
 *
 * $Id:$
 */
#ifndef IPIFSTATSTABLE_OIDS_H
#define IPIFSTATSTABLE_OIDS_H

#ifdef __cplusplus
extern "C"
{
#endif


    /*
     * column number definitions for table ipIfStatsTable 
     */
#define IPIFSTATSTABLE_OID              1,3,6,1,2,1,4,31,3
#define IPIFSTATSTABLELASTCHANGE_OID    1,3,6,1,2,1,4,31,2

#define COLUMN_IPIFSTATSIPVERSION         1

#define COLUMN_IPIFSTATSIFINDEX         2

#define COLUMN_IPIFSTATSINRECEIVES         3

#define COLUMN_IPIFSTATSHCINRECEIVES         4

#define COLUMN_IPIFSTATSINOCTETS         5

#define COLUMN_IPIFSTATSHCINOCTETS         6

#define COLUMN_IPIFSTATSINHDRERRORS         7

#define COLUMN_IPIFSTATSINNOROUTES         8

#define COLUMN_IPIFSTATSINADDRERRORS         9

#define COLUMN_IPIFSTATSINUNKNOWNPROTOS         10

#define COLUMN_IPIFSTATSINTRUNCATEDPKTS         11

#define COLUMN_IPIFSTATSINFORWDATAGRAMS         12

#define COLUMN_IPIFSTATSHCINFORWDATAGRAMS         13

#define COLUMN_IPIFSTATSREASMREQDS         14

#define COLUMN_IPIFSTATSREASMOKS         15

#define COLUMN_IPIFSTATSREASMFAILS         16

#define COLUMN_IPIFSTATSINDISCARDS         17

#define COLUMN_IPIFSTATSINDELIVERS         18

#define COLUMN_IPIFSTATSHCINDELIVERS         19

#define COLUMN_IPIFSTATSOUTREQUESTS         20

#define COLUMN_IPIFSTATSHCOUTREQUESTS         21

#define COLUMN_IPIFSTATSOUTFORWDATAGRAMS         23

#define COLUMN_IPIFSTATSHCOUTFORWDATAGRAMS         24

#define COLUMN_IPIFSTATSOUTDISCARDS         25

#define COLUMN_IPIFSTATSOUTFRAGREQDS         26

#define COLUMN_IPIFSTATSOUTFRAGOKS         27

#define COLUMN_IPIFSTATSOUTFRAGFAILS         28

#define COLUMN_IPIFSTATSOUTFRAGCREATES         29

#define COLUMN_IPIFSTATSOUTTRANSMITS         30

#define COLUMN_IPIFSTATSHCOUTTRANSMITS         31

#define COLUMN_IPIFSTATSOUTOCTETS         32

#define COLUMN_IPIFSTATSHCOUTOCTETS         33

#define COLUMN_IPIFSTATSINMCASTPKTS         34

#define COLUMN_IPIFSTATSHCINMCASTPKTS         35

#define COLUMN_IPIFSTATSINMCASTOCTETS         36

#define COLUMN_IPIFSTATSHCINMCASTOCTETS         37

#define COLUMN_IPIFSTATSOUTMCASTPKTS         38

#define COLUMN_IPIFSTATSHCOUTMCASTPKTS         39

#define COLUMN_IPIFSTATSOUTMCASTOCTETS         40

#define COLUMN_IPIFSTATSHCOUTMCASTOCTETS         41

#define COLUMN_IPIFSTATSINBCASTPKTS         42

#define COLUMN_IPIFSTATSHCINBCASTPKTS         43

#define COLUMN_IPIFSTATSOUTBCASTPKTS         44

#define COLUMN_IPIFSTATSHCOUTBCASTPKTS         45

#define COLUMN_IPIFSTATSDISCONTINUITYTIME         46

#define COLUMN_IPIFSTATSREFRESHRATE         47


#define IPIFSTATSTABLE_MIN_COL   COLUMN_IPIFSTATSINRECEIVES
#define IPIFSTATSTABLE_MAX_COL   COLUMN_IPIFSTATSREFRESHRATE



#ifdef __cplusplus
}
#endif
#endif                            /* IPIFSTATSTABLE_OIDS_H */

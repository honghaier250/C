LDAP Libraries for C - Windows, Linux


1.0  LDAP Libraries for C

2.0  Platform-specific Information
     2.1  Windows
     2.2  Linux
          2.2.1  System Requirements
          2.2.2  Installation on Linux
          2.2.3  Novell Import Convert Export Utility

3.0  Sample Programs

4.0  LDAP v2 and v3 Support

5.0  Bind

6.0  SSL Support

7.0  LDAP Controls

8.0  LDAP Extensions

9.0 LDAP Utilities

10.0 Dependencies

11.0 Known Issues and Solutions

12.0 OpenLDAP Copyrights

13.0 OpenSSL Acknowledgement

14.0 Revision History
     14.1 Changes for the June 2005 NDK
          15.1.1 New Functionality
     14.2 Changes for the October 2004 NDK
          15.2.1 New Functionality
          15.2.2 Sample Code

15.0 Legal Notices


1.0  LDAP Libraries for C

     LDAP (Lightweight Directory Access Protocol) is an
     emerging Internet standard for accessing directory
     information, allowing LDAP-enabled applications to
     access multiple directories. LDAP v3 supports such
     features as secure connections (through SSL and SASL),
     entry management, schema management, and LDAP controls
     and extensions for expanding LDAP's functionality.

     The LDAP Libraries for C kit enables you to write
     applications to access, manage, update, and search for
     information stored in Novell eDirectory and other
     LDAP-aware directories.


2.0  Platform-specific Information

     2.1  Windows

          The LDAP Libraries for C are supported on:

	  32-bit:
          -  Microsoft Windows 2003 Server
          -  Microsoft Windows 2008 Server
          -  Microsoft Windows XP
          -  Microsoft Windows vista
          -  Microsoft Windows 7
          -  Microsoft Windows 8
	  64-bit:
          -  Microsoft Windows 2008 Server
          -  Microsoft Windows 2012 Server
          -  Microsoft Windows vista
          -  Microsoft Windows 7
	  -  Microsoft Windows 2008 R2 Server
          -  Microsoft Windows 8

          During installation, your PATH variable is
          updated to include the following directories

	  32-bit:
              <Unzipped_Location>\pkg\win32\bin
              <Unzipped_Location>\pkg\win32\tools
	  64-bit:
              <Unzipped_Location>\pkg\win64\bin
              <Unzipped_Location>\pkg\win64\tools

          The kit includes the following libraries:

              ldapsdk.dll
              ldapssl.dll
              ldapx.dll

          Additionally, several template files are included
          in the tools directory for use with the DirLoad
          data handler. These files can be used to generate
          any number of unique test objects to populate a
          directory.  The tools directory contains 2
          sub-directories - tables and templates. 

          The tables directory contains the files named
          cities, company, domain, first, initial,
          lastnames, titles, etc. 

          The templates directory contains the file named
          attrs.


     2.2  Linux

          2.2.1  System Requirements

                 The LDAP Libraries for C are supported on:

                 - SLES 11 SP1, SP2, SP3 and later Support Packs
                 - SLES 10 SP4 and later Support Packs
		 - RHEL 5.4, 5.5, 5.6, 5.7, 5.8, 5.9 and later Support Packs
		 - RHEL 6.2, 6.3, 6.4 and later Support Packs


          2.2.2  Installation on Linux

                 Extract the files to a directory on your
                 Unix system. This process creates the
                 following directories and populates with
                 the files provided by the kit.

                 -  include  
                    This directory contains the header
                    files needed to built your client
                    application.
                 -  lib
                    This directory contains the following
                    library files provided with the kit.

                        libldapsdk.so
                        libldapx.so
                        libldapssl.so
                        libldapgss.so

                    These library files were built with the
                    platform's native compiler.
                    The specific library files that you
                    link depend on the type of
                    application(s) you are building.
                    The following libraries are used by the
                    Novell Import Convert Export Utility
                    and are also included:

                        libldaphdlr.so
                        libldif.so
                        libdelim.so
                        libdirload.so
                        libsal.so
                        libschhdlr.so

                    Additionally, several template files
                    are included in the tools directory for
                    use with the DirLoad data handler.
                    These files can be used to generate any
                    number of unique test objects to
                    populate a directory.  The tools
                    directory contains 2 sub-directories -
                    tables and templates. 
                    The tables directory contains the files
                    named cities, company, domain, first,
                    initial, lastnames, titles, etc. The
                    templates directory contains the file
                    named attrs.
                    When running applications, add the
                    following path to the LD_LIBRARY_PATH
                    environment variable:
		    32-bit:
                        <install directory>/cldapsdk/lib
		    64-bit:
                        <install directory>/cldapsdk/lib64
                 -  tools
                    This directory contains the LDAP
                    command-line tools. 
                 -  etc
                    This directory contains Novell Import
                    Convert Export Utility (ICE) default
                    configuration file, ice.conf.
                 -  res
                    This directory contains the following
                    xlf files used by Novell Import Convert
                    Export Utility.

                        delim_en.xlf
                        dirload_en.xlf
                        ldaphdlr_en.xlf
                        ldif_en.xlf
                        schhdlr_en.xlf

                 -  man
                    This directory contains the LDAP
                    command-line tools man pages.
                    To view the man pages, add the
                    following path to the MANPATH
                    environment variable :
                        <install directory>/cldapsdk/man


          2.2.3  Novell Import Convert Export Utility

                 Set the following environment variables
                 before running the Import Convert Export
                 (ICE) utility.

                  1. Set ICE_CONF to <install
                     directory>/cldapsdk/etc/ice.conf 
                  2. Set ICE_RES_PATH to <install
                     directory>/cldapsdk/res


3.0  Sample Programs

     A directory named "samples" contains dozens of
     complete sample programs demonstrating the use of most
     LDAP APIs.

     The directory "samples\extensions" contains samples of
     Novell LDAP extensions. Novell's eDirectory supports
     several LDAP extensions such as obtaining effective
     rights, managing replicas and partitions, and
     refreshing the LDAP server.

     A directory named "samples" contains dozens of  
     complete sample programs demonstrating the
     use of most LDAP APIs.  

     The directory "samples\extensions"
     contains samples of Novell LDAP extensions. Novell's
     eDirectory supports several LDAP extensions such as
     obtaining effective rights, managing replicas and
     partitions, and refreshing the LDAP server.  

     A simple batch file or makefile is also supplied in
     the samples directory to aid you in building samples
     or test programs.  You must edit a few lines in the
     batch files to indicate the location of the LDAP SDK
     and the compiler in your environment.

         bldw32.bat  - For Win32 platforms, using the MSVC
     compiler

         bldw64.bat  - For Win64 platforms, using the MSVC
     compiler

         Makefile    - For Unix systems.


4.0  LDAP v2 and v3 Support

     This version of the LDAP Libraries for C supports
     mainly LDAPv3. Since the consensus in the LDAP
     community is to encourage everyone to update to
     LDAPv3, Novell has not made a commitment to ensure
     that these libraries support LDAPv2 servers.


5.0  Bind

     eDirectory supports simple bind. However, for
     non-encrypted connections, the LDAP server must be
     configured to allow clear-text passwords. The LDAP
     Libraries for C kit includes SASL functions and bind
     functions that perform simple binds.

     For secure, encrypted connections to eDirectory, use
     the SSL connections with certificates.


6.0  SSL Support

     SSL support is provided by a companion library
     (ldapssl or libldapssl).  Applications using SSL need
     to link with this library.  It provides functions for
     initializing SSL and creating LDAP session handles
     that are SSL enabled. An SSL-enabled session handle
     ensures that all data is encrypted between the client
     and the server.

     This product uses OpenSSL Version 0.9.6k.


7.0  LDAP Controls

     LDAP servers may support controls.  Query the root DSE
     to get a list of supported controls.  See the GetDSE.c
     sample for coding example.

     The LDAP Libraries for C kit supplies functions to
     help applications use these controls.


8.0  LDAP Extensions

     SDK support for LDAP extensions are provided by a
     companion library (ldapx or libldapx).  Applications
     using extensions need to link with this library.

     eDirectory supports the following LDAP extensions for
     obtaining effective rights, managing replicas and
     partitions, and refreshing the LDAP server:

     2.16.840.1.113719.1.27.100.1   Nds To Ldap Response

     2.16.840.1.113719.1.27.100.2   Nds To Ldap Request

     2.16.840.1.113719.1.27.100.3   Split Partition Request

     2.16.840.1.113719.1.27.100.4   Split Partition
     Response

     2.16.840.1.113719.1.27.100.5   Merge Partition Request

     2.16.840.1.113719.1.27.100.6   Merge Partition
     Response

     2.16.840.1.113719.1.27.100.7   Add Replica Request

     2.16.840.1.113719.1.27.100.8   Add Replica Response

     2.16.840.1.113719.1.27.100.9   Refresh Server Request

     2.16.840.1.113719.1.27.100.10  Refresh Server Response

     2.16.840.1.113719.1.27.100.11  Remove Replica Request

     2.16.840.1.113719.1.27.100.12  Remove Replica Response

     2.16.840.1.113719.1.27.100.13  Partition Entry Count
     Request

     2.16.840.1.113719.1.27.100.14  Partition Entry Count
     Response

     2.16.840.1.113719.1.27.100.15  Change Replica Type
     Request

     2.16.840.1.113719.1.27.100.16  Change Replica Type
     Response

     2.16.840.1.113719.1.27.100.17  Get Replica Info
     Request

     2.16.840.1.113719.1.27.100.18  Get Replica Info
     Response

     2.16.840.1.113719.1.27.100.19  List Replica Request

     2.16.840.1.113719.1.27.100.20  List Replica Response

     2.16.840.1.113719.1.27.100.21  Receive All Updates
     Request

     2.16.840.1.113719.1.27.100.22  Receive All Updates
     Response

     2.16.840.1.113719.1.27.100.23  Send All Updates
     Request

     2.16.840.1.113719.1.27.100.24  Send All Updates
     Response

     2.16.840.1.113719.1.27.100.25  Request Partition Sync
     Request

     2.16.840.1.113719.1.27.100.26  Request Partition Sync
     Response

     2.16.840.1.113719.1.27.100.27  Request Schema Sync
     Request

     2.16.840.1.113719.1.27.100.28  Request Schema Sync
     Response

     2.16.840.1.113719.1.27.100.29  Abort Partition
     Operation Request

     2.16.840.1.113719.1.27.100.30  Abort Partition
     Operation Response

     2.16.840.1.113719.1.27.100.31  Get Bind DN Request

     2.16.840.1.113719.1.27.100.32  Get Bind DN Response

     2.16.840.1.113719.1.27.100.33  Get Effective
     Privileges Request

     2.16.840.1.113719.1.27.100.34  Get Effective
     Privileges Response

     2.16.840.1.113719.1.27.100.35  Set Replication Filter
     Request                           

     2.16.840.1.113719.1.27.100.36  Set Replication Filter
     Response

     2.16.840.1.113719.1.27.100.37  Get Replication Filter
     Request

     2.16.840.1.113719.1.27.100.38  Get Replication Filter
     Response

     2.16.840.1.113719.1.27.100.39  Create Orphan Partition
     Request

     2.16.840.1.113719.1.27.100.40  Create Orphan Partition
     Response

     2.16.840.1.113719.1.27.100.41  Remove Orphan Partition
     Request

     2.16.840.1.113719.1.27.100.42  Remove Orphan Partition
     Response

     Extensions to trigger eDirectory background processes
     via ldap_trigger_back_process():

     2.16.840.1.113719.1.27.100.43  Trigger Backlinker
     Request

     2.16.840.1.113719.1.27.100.44  Trigger Backlinker
     Response

     2.16.840.1.113719.1.27.100.47  Trigger Janitor Request
     

     2.16.840.1.113719.1.27.100.48  Trigger Janitor
     Response

     2.16.840.1.113719.1.27.100.49  Trigger Limber Request 

     2.16.840.1.113719.1.27.100.50  Trigger Limber Response

     2.16.840.1.113719.1.27.100.51  Trigger Skulker Request
     

     2.16.840.1.113719.1.27.100.52  Trigger Skulker
     Response

     2.16.840.1.113719.1.27.100.53  Trigger Schema Synch
     Request 

     2.16.840.1.113719.1.27.100.54  Trigger Schema Synch
     Response

     2.16.840.1.113719.1.27.100.55  Trigger Partition Purge
     Request 

     2.16.840.1.113719.1.27.100.56  Trigger Partition Purge
     Response

     2.16.840.1.113719.1.27.100.79  Monitor Events Request
            
     2.16.840.1.113719.1.27.100.80  Monitor Events Response

     2.16.840.1.113719.1.27.100.81  Event Notification  

     2.16.840.1.113719.1.27.100.84  filteredEventMonitorRequest

     2.16.840.1.113719.1.27.103.1   createGroupingRequest
   
     2.16.840.1.113719.1.27.103.2   endGroupingRequest
  
     2.16.840.1.113719.1.148.100.1  Put Login Configuration
  
     2.16.840.1.113719.1.27.103.2   endGroupingRequest


     The Novell Import Convert Export utility uses the
     following extensions. They are not general extensions
     designed for developer use but are designed to support
     the LDAP Bulk Update Replication Protocol (LBURP).

     2.16.840.1.113719.1.142.100.1  Start Framed Protocol
     Request

     2.16.840.1.113719.1.142.100.2  Start Framed Protocol
     Response

     2.16.840.1.113719.1.142.100.4  End Framed Protocol
     Request

     2.16.840.1.113719.1.142.100.5  End Framed Protocol
     Response

     2.16.840.1.113719.1.142.100.6  Lburp Operation Request

     2.16.840.1.113719.1.142.100.7  Lburp Operation
     Response

     To use any Novell extensions, the LDAP server must be
     running on eDirectory version 8.5 or higher. To obtain
     a copy, see Novell's developer product site:

         http://www.novell.com/download/

     The LDAP Libraries for C kit includes functions to
     help applications use these extensions.

     If you used an early beta version of eDirectory 8.5,
     be aware that the OIDs for the extensions changed for
     beta 5 and the final release.


9.0 LDAP Utilities

     This release includes the command line version of the
     Novell Import Convert Export utility (ice, ice.exe, or
     ice.nlm). This utility performs all the functions of
     the legacy LDAP utilities. Novell recommends using
     this utility instead of the legacy LDAP utilities.
     Note that the Novell Import Convert Export utility
     will work on any LDAP v2 or v3 server.  However, it
     will take advantage of the fast LBURP protocol with
     eDirectory 8.5 or higher.

     NetWare NLMs are limited to names with a maximum of 8
     characters; therefore the NetWare platform requires
     shorter names for the LDAP utilities. They have been
     renamed as follows for the NetWare platform:

     LDAP Utility
                                                             
     ldapadd    
                                                             
     ldapdelete
                                                             
     ldapmodify
                                                             
     ldapsearch
                                                             
     ldapmodrdn
                                                             
     ndsindex 
                                                             
     ice 
                                                             
     These utilities are located in the tools directory.
     You need to either copy them to the system directory
     or include the directory in the PATH variable.


10.0 Dependencies

     For full functionality, the LDAP Libraries for C kit
     is dependent upon the following:

     -  LDAP Extensions. The LDAP extensions for partition
        and replica management, getting effective rights,
        and refreshing the LDAP server require eDirectory
        8.5. To obtain a version, see the Novell downloads
        page at (http://www.novell.com/download/). 


11.0 Known Issues and Solutions

     -  Unlike eDirectory, the LDAP standard does not
        define an "alias" structural class. When using LDIF
        tools such as the Novell Import Convert Export
        Utility, you may encounter errors adding aliased
        objects.
     -  There is currently no standard mechanism in LDAP to
        notify the application that the password has
        expired and a grace login was used when binding.
        eDirectory has provided an interim solution. See
        the sample "gracelogin.c" for further information. 


12.0 OpenLDAP Copyrights

     Portions of the source for the LDAP Libraries for C
     are based on the Directory SDK (Version 2.0) available
     from OpenLDAP.org. The following have contributed to
     the source code.

     -  Copyright   1990, 1995 Regents of the University of
        Michigan. All rights reserved.
     -  Copyright   1987 Regents of the University of
        California
     -  Copyright 1991 by the Massachusetts Institute of
        Technology
     -  Copyright   1994 Enrique Silvestre Mora,
        Universitat Jaume I, Spain.
     -  Copyright   1992, 1993, 1994 Henry Spencer.
     -  Copyright   1992, 1993, 1994 The Regents of the
        University of California.
     -  Copyright   1998, 1999 The OpenLDAP Foundation,
        Redwood City, CA All rights reserved.
     -  Copyright   1997, 1998, 1999 Computing Research
        Labs, New Mexico State University
     -  Copyright   1999 PADL Software Pty Ltd.


13.0 OpenSSL Acknowledgement

     This product includes software developed by the
     OpenSSL Project for use in the OpenSSL Toolkit
     (http://www.openssl.org/).


14.0 Revision History

     14.1 Changes for the June 2005 NDK

          14.1.1 New Functionality

                 -  Support for Subnordinate Subtree search
                    scope for Novell Virtual Directory
                    Services (NVDS).
                 -  Added the following LDAP functions:
                    -  ldap_create_geteffective_control 
                    -  ldap_create_reference_control 
                    -  ldap_create_sstatus_control 
                    -  ldap_parse_reference_control 
                    -  ldap_parse_sstatus_control 


     14.2 Changes for the October 2004 NDK

          14.2.1 New Functionality

                 -  SASL-GSSAPI support: We assume that
                    SASL-GSSAPI is already installed on
                    your machine. If not, you might want to
                    download and install SASL-GSSAPI. 
                    Most of the OS vendors provide GSSAPI
                    libraries. Check their distribution or
                    Website for these libraries. 
                    Alternatively, other free
                    implementations include Heimdal, MIT,
                    and others.
                    Heimdal: http://www.pdc.kth.se/heimdal/
                     (http://www.pdc.kth.se/heimdal)
                    MIT:
                    http://web.mit.edu/kerberos/www/dist/
                    (http://web.mit.edu/kerberos/www/dist) 
                 -  LDAP based backup


          14.2.2 Sample Code

                 -  gssbind.c: Demonstrates how to do a
                    SASL-GSSAPI bind with an LDAP server.
                 -  backup.c: Demonstrates how to do backup
                    and restore of eDirectory objects
                    through LDAP.


15.0 Legal Notices

     Novell, Inc. makes no representations or warranties
     with respect to the contents or use of this
     documentation, and specifically disclaims any express
     or implied warranties of merchantability or fitness
     for any particular purpose. Further, Novell, Inc.
     reserves the right to revise this publication and to
     make changes to its content, at any time, without
     obligation to notify any person or entity of such
     revisions or changes.

     Further, Novell, Inc. makes no representations or
     warranties with respect to any software, and
     specifically disclaims any express or implied
     warranties of merchantability or fitness for any
     particular purpose. Further, Novell, Inc. reserves the
     right to make changes to any and all parts of Novell
     software, at any time, without any obligation to
     notify any person or entity of such changes.


     You may not use, export, or re-export this product in
     violation of any applicable laws or regulations
     including, without limitation, U.S. export regulations
     or the laws of the country in which you reside.

     All files provided in this release are subject to the
     Novell Developer License Agreement, which can be found
     in the license.txt file provided in this download.


     Copyright  2004 Novell, Inc. All rights reserved. No
     part of this publication may be reproduced,
     photocopied, stored on a retrieval system, or
     transmitted without the express written consent of the
     publisher.


     Novell is a registered trademark of Novell, Inc. in
     the United States and other countries.


     All third-party trademarks are the property of their
     respective owners.


(C) 2013 NetIQ Corporation and its affiliates. All Rights Reserved.

For information about NetIQ trademarks, see http://www.netiq.com/company/legal/.


/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.iterate.conf,v 1.5 2001/12/04 21:36:27 hardaker Exp $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "nsModuleTable.h"

netsnmp_feature_require (table_dataset)
     void nsModuleTable_free (void *context, netsnmp_iterator_info * dont_care)
{
    free (context);
}

/** Initialize the nsModuleTable table by defining it's contents and how it's structured */
void initialize_table_nsModuleTable (void)
{
    const oid nsModuleTable_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 1, 2, 1 };
    netsnmp_table_registration_info *table_info;

    netsnmp_handler_registration *my_handler;

    netsnmp_iterator_info *iinfo;

    /*
     * create the table structure itself 
     */
    table_info = SNMP_MALLOC_TYPEDEF (netsnmp_table_registration_info);
    iinfo = SNMP_MALLOC_TYPEDEF (netsnmp_iterator_info);

    /*
     * if your table is read only, it's easiest to change the
     * HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY 
     */
    my_handler = netsnmp_create_handler_registration ("nsModuleTable",
                                                      nsModuleTable_handler,
                                                      nsModuleTable_oid,
                                                      OID_LENGTH (nsModuleTable_oid), HANDLER_CAN_RWRITE);

    if (!my_handler || !table_info || !iinfo)
    {
        if (my_handler)
            netsnmp_handler_registration_free (my_handler);
        SNMP_FREE (table_info);
        SNMP_FREE (iinfo);
        return;                    /* mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */
    netsnmp_table_helper_add_indexes (table_info, ASN_OCTET_STR,    /* context name */
                                      ASN_OBJECT_ID,    /* reg point */
                                      ASN_INTEGER,    /* priority */
                                      0);

    table_info->min_column = 4;
    table_info->max_column = 6;

    /*
     * iterator access routines 
     */
    iinfo->get_first_data_point = nsModuleTable_get_first_data_point;
    iinfo->get_next_data_point = nsModuleTable_get_next_data_point;
    iinfo->free_loop_context_at_end = nsModuleTable_free;
    iinfo->table_reginfo = table_info;

    /***************************************************
     * registering the table with the master agent
     */
    DEBUGMSGTL (("initialize_table_nsModuleTable", "Registering table nsModuleTable as a table iterator\n"));
    netsnmp_register_table_iterator2 (my_handler, iinfo);
}

/** Initializes the nsModuleTable module */
void init_nsModuleTable (void)
{

    /*
     * here we initialize all the tables we're planning on supporting 
     */
    initialize_table_nsModuleTable ();
}

/** returns the first data point within the nsModuleTable table data.

    Set the my_loop_context variable to the first data point structure
    of your choice (from which you can find the next one).  This could
    be anything from the first node in a linked list, to an integer
    pointer containing the beginning of an array variable.

    Set the my_data_context variable to something to be returned to
    you later that will provide you with the data to return in a given
    row.  This could be the same pointer as what my_loop_context is
    set to, or something different.

    The put_index_data variable contains a list of snmp variable
    bindings, one for each index in your table.  Set the values of
    each appropriately according to the data matching the first row
    and return the put_index_data variable at the end of the function.
*/
typedef struct context_tree_ptr_s
{
    netsnmp_subtree *tree;
    subtree_context_cache *context_ptr;
} context_tree_ptr;

netsnmp_variable_list *nsModuleTable_get_first_data_point (void **my_loop_context,
                                                           void **my_data_context,
                                                           netsnmp_variable_list * put_index_data,
                                                           netsnmp_iterator_info * otherstuff)
{

    struct variable_list *vptr;

    u_long ultmp;

    context_tree_ptr *ctree;

    ctree = SNMP_MALLOC_TYPEDEF (context_tree_ptr);

    ctree->context_ptr = get_top_context_cache ();
    /* Skip empty context registrations */
    while (!ctree->context_ptr->first_subtree)
    {
        ctree->context_ptr = ctree->context_ptr->next;
        if (!ctree->context_ptr)
        {
            SNMP_FREE (ctree);
            return NULL;
        }
    }
    ctree->tree = ctree->context_ptr->first_subtree;

    *my_loop_context = ctree;
    *my_data_context = ctree->tree;

    vptr = put_index_data;
    snmp_set_var_value (vptr, ctree->context_ptr->context_name, strlen (ctree->context_ptr->context_name));

    vptr = vptr->next_variable;
    snmp_set_var_value (vptr,
                        ctree->context_ptr->first_subtree->name_a,
                        ctree->context_ptr->first_subtree->namelen * sizeof (oid));

    ultmp = ctree->context_ptr->first_subtree->priority;
    vptr = vptr->next_variable;
    snmp_set_var_value (vptr, &ultmp, sizeof (ultmp));

    return put_index_data;
}

/** functionally the same as nsModuleTable_get_first_data_point, but
   my_loop_context has already been set to a previous value and should
   be updated to the next in the list.  For example, if it was a
   linked list, you might want to cast it and the return
   my_loop_context->next.  The my_data_context pointer should be set
   to something you need later and the indexes in put_index_data
   updated again. */

struct variable_list *nsModuleTable_get_next_data_point (void **my_loop_context,
                                                         void **my_data_context,
                                                         struct variable_list *put_index_data,
                                                         netsnmp_iterator_info * otherstuff)
{

    struct variable_list *vptr;

    context_tree_ptr *ctree = (context_tree_ptr *) * my_loop_context;

    u_long ultmp;

    if (ctree->tree->next)
        ctree->tree = ctree->tree->next;
    else
    {
        ctree->context_ptr = ctree->context_ptr->next;
        if (!ctree->context_ptr)
        {
            return NULL;
        }
        ctree->tree = ctree->context_ptr->first_subtree;
    }

    *my_data_context = ctree->tree;

    vptr = put_index_data;
    snmp_set_var_value (vptr, ctree->context_ptr->context_name, strlen (ctree->context_ptr->context_name));

    vptr = vptr->next_variable;
    snmp_set_var_value (vptr, ctree->tree->name_a, ctree->tree->namelen * sizeof (oid));

    ultmp = ctree->tree->priority;
    vptr = vptr->next_variable;
    snmp_set_var_value (vptr, &ultmp, sizeof (ultmp));

    return put_index_data;
}

/** handles requests for the nsModuleTable table, if anything else needs to be done */
int
nsModuleTable_handler (netsnmp_mib_handler * handler,
                       netsnmp_handler_registration * reginfo,
                       netsnmp_agent_request_info * reqinfo, netsnmp_request_info * requests)
{

    netsnmp_table_request_info *table_info;

    netsnmp_request_info *request;

    netsnmp_variable_list *var;

    netsnmp_subtree *tree;

    u_long ultmp;

    u_char modes[1];

    for (request = requests; request; request = request->next)
    {
        var = request->requestvb;
        if (request->processed != 0)
            continue;

        /*
         * perform anything here that you need to do.  The request have
         * already been processed by the master table_dataset handler, but
         * this gives you chance to act on the request in some other way if 
         * need be. 
         */

        /*
         * the following extracts the my_data_context pointer set in the
         * loop functions above.  You can then use the results to help
         * return data for the columns of the nsModuleTable table in
         * question 
         */
        tree = (netsnmp_subtree *) netsnmp_extract_iterator_context (request);
        if (tree == NULL)
        {
            if (reqinfo->mode == MODE_GET)
            {
                netsnmp_set_request_error (reqinfo, request, SNMP_NOSUCHINSTANCE);
                continue;
            }
            /*
             * XXX: no row existed, if you support creation and this is a
             * set, start dealing with it here, else continue 
             */
        }

        /*
         * extracts the information about the table from the request 
         */
        table_info = netsnmp_extract_table_info (request);

        /*
         * table_info->colnum contains the column number requested 
         */
        /*
         * table_info->indexes contains a linked list of snmp variable
         * bindings for the indexes of the table.  Values in the list have 
         * been set corresponding to the indexes of the request 
         */
        if (table_info == NULL)
        {
            continue;
        }

        switch (reqinfo->mode)
        {
                /*
                 * the table_iterator helper should change all GETNEXTs into
                 * GETs for you automatically, so you don't have to worry
                 * about the GETNEXT case.  Only GETs and SETs need to be
                 * dealt with here 
                 */
            case MODE_GET:
                switch (table_info->colnum)
                {
                    case COLUMN_NSMODULENAME:
                        if (tree->reginfo->handlerName)
                        {
                            snmp_set_var_typed_value (var, ASN_OCTET_STR,
                                                      tree->reginfo->handlerName, strlen (tree->reginfo->handlerName));
                        }
                        else
                        {
                            snmp_set_var_typed_value (var, ASN_OCTET_STR, "", 0);
                        }
                        break;

                    case COLUMN_NSMODULEMODES:
                        /*
                         * basically, these BITS needs to be inverted in order 
                         */
                        modes[0] =
                            ((HANDLER_CAN_GETANDGETNEXT & tree->reginfo->modes) << 7) | ((HANDLER_CAN_SET & tree->
                                                                                          reginfo->modes) << 5) |
                            ((HANDLER_CAN_GETBULK & tree->reginfo->modes) << 3);
                        /*  yuck  */
                        snmp_set_var_typed_value (var, ASN_OCTET_STR, modes, 1);
                        break;

                    case COLUMN_NSMODULETIMEOUT:
                        ultmp = tree->timeout;
                        snmp_set_var_typed_value (var, ASN_INTEGER, (u_char *) & ultmp, sizeof (u_long));
                        break;

                    default:
                        /*
                         * We shouldn't get here 
                         */
                        snmp_log (LOG_ERR, "problem encountered in nsModuleTable_handler: unknown column\n");
                }
                break;

            default:
                snmp_log (LOG_ERR, "problem encountered in nsModuleTable_handler: unsupported mode\n");
        }
    }
    return SNMP_ERR_NOERROR;
}

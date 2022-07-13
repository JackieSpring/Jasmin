
#include "breakpoint.h"

#define MAX_BREAKPOINT  0xffff


static int breakpoint_table_init_trap();



typedef struct breakpoint_table_struct {
    size_t nbreakpoint;
    hashtable * bp_id_table;
    numhashtable * bp_addr_table;
    void * init_trap;
} breakpoint_table;




breakpoint_table bp_table = {
    .nbreakpoint = 0,
    .bp_id_table = NULL,
    .bp_addr_table = NULL,
    .init_trap = breakpoint_table_init_trap,
};


static int breakpoint_table_init_trap() {
    bp_table.bp_id_table = create_hashtable();
    if ( bp_table.bp_id_table == NULL )
        goto cleanup;
    
    bp_table.bp_addr_table = create_numhashtable();
    if ( bp_table.bp_addr_table == NULL )
        goto cleanup;
    
    bp_table.init_trap = NULL;
    
    return 0;

cleanup:
    if ( bp_table.bp_id_table != NULL )
        destroy_hashtable(bp_table.bp_id_table);
    
    if ( bp_table.bp_addr_table != NULL )
        destroy_numhashtable(bp_table.bp_addr_table);
    
    return -1;   
}


int add_breakpoint( breakpoint_id id, breakpoint_addr bp ) {
    if ( bp_table.init_trap != NULL )
        if( (( int (*)() )bp_table.init_trap)() < 0)
            return -1;
    
    if( id == NULL )
        return -1;
    
    if( add_to_hashtable( bp_table.bp_id_table, id, bp) < 0)
       return -1;

    if ( add_to_numhashtable( bp_table.bp_addr_table, bp, bp ) < 0 ) {
        del_from_hashtable( bp_table.bp_id_table, id );
        return -1;
    }
    
    bp_table.nbreakpoint++;
    return 0;
}

int del_breakpoint( breakpoint_id id ) {
    if ( bp_table.init_trap != NULL )
        return -1;
    
    breakpoint_addr address;
    
    if( get_value_hashtable(bp_table.bp_id_table, id, &address) < 0 )
        return -1;
    
    if ( del_from_hashtable(bp_table.bp_id_table, id) < 0 )
        return -1;
    
    if ( del_from_numhashtable( bp_table.bp_addr_table, address ) < 0 ) {
        add_to_hashtable(bp_table.bp_id_table, id, address);
        return -1;
    }
    
    
    return 0;
}

bool is_breakpoint( breakpoint_addr bp ) {
    if ( bp_table.init_trap != NULL )
        return false;
        
    breakpoint_addr buff;
    
    if( get_value_numhashtable( bp_table.bp_addr_table , bp, &buff ) == 0)
        return true;
    
    return false;
}

int iter_breakpoint( bool (* iter)( breakpoint_id id, breakpoint_addr ) );

